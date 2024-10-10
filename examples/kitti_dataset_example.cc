#include <pcl/filters/impl/voxel_grid.hpp>

#include "slam_viewer/ui/CloudUI.hpp"
#include "slam_viewer/ui/CoordinateUI.h"
#include "slam_viewer/ui/FrameUI.h"
#include "slam_viewer/core/ImageShower.h"
#include "KittiHelper/KittiHelper.h"
#include "slam_viewer/core/PointTypes.h"
#include "slam_viewer/ui/TrajectoryUI.h"
#include "slam_viewer/core/WindowImpl.h"

using namespace slam_viewer;

/// 配置菜单函数
void ConfigMenu(Menu::Ptr menu, Camera::Ptr camera) {
    assert(menu && camera && "menu or camera is nullptr!");

    /// 4.1 follow 相机跟踪模式
    menu->AddCheckBoxItem("Follow", [=](bool checked) {
        if (checked)
            camera->SetFollow();
        else
            camera->SetFree();
    });

    /// 4.2 设置上帝视角
    menu->AddButtonItem("God View", [=](bool pushed) {
        if (pushed) {
            camera->SetFixedPose(SE3());
            camera->SetModelView(pangolin::ModelViewLookAt(0, 0, 300, 0, 0, 0, pangolin::AxisX));
            camera->SetFree();
        }
    });

    /// 4.3 设置前置视角
    menu->AddButtonItem("Front View", [=](bool pushed) {
        if (pushed) {
            camera->SetFollow();
            camera->SetModelView(pangolin::ModelViewLookAt(-50, 0, 10, 0, 0, 0, pangolin::AxisZ));
            camera->SetFree();
        }
    });
}

/// 进行点云体素滤波，缩小点云体积
void VoxelGrid(PointCloudXYZRT::Ptr &cloud) {
    auto voxel_grid = pcl::VoxelGrid<PointXYZRT>();
    voxel_grid.setLeafSize(0.5, 0.5, 0.5);
    voxel_grid.setInputCloud(cloud);
    voxel_grid.filter(*cloud);
}

/// 融合各个线束的点云
PointCloudXYZRT::Ptr FuseRingClouds(std::vector<PointCloudXYZRT::Ptr> &clouds) {
    PointCloudXYZRT::Ptr cloud_out = pcl::make_shared<PointCloudXYZRT>();
    for (auto &cloud : clouds)
        *cloud_out += *cloud;

    VoxelGrid(cloud_out);
    return cloud_out;
}

int main(int argc, char **argv) {

    if (argc != 3) {
        std::cout << "Usage: ./bin/kitti_viewer <dataset_path> <sequence>" << std::endl;
        return -1;
    }

    /// 1. 定义相机坐标系和雷达坐标系初始位姿
    Mat3 Rlc;
    Rlc << 0, 0, 1, -1, 0, 0, 0, -1, 0;
    Vec3 tlc(1.5, 0, -0.5);
    SE3 Tlc(Rlc, tlc);

    Mat3 Rwl = Mat3::Identity();
    Vec3 twl(0, 0, 1.5);
    SE3 Twl0(Rwl, twl);
    SE3 Twc0 = Twl0 * Tlc;

    /// 2. 创建KITTI数据加载器
    KittiHelper::Options kitti_options = {argv[1], argv[2], SE3(Rlc, tlc)};
    KittiHelper kitti_helper(kitti_options);

    /// 3. 创建可视化窗口和三个坐标系，world camera lidar 坐标系 和雷达轨迹
    auto world_coord = std::make_shared<CoordinateUI>(5, SE3());
    auto lidar_coord = std::make_shared<CoordinateUI>(0.2, Twl0);
    auto camera_coord = std::make_shared<CoordinateUI>(0.2, Twc0);
    auto lidar_trajectory = std::make_shared<TrajectoryUI>(Vec3(1.0, 0.1, 0.1), 3.0, 3.0);

    auto viewer = std::make_shared<WindowImpl>("KITTI Viewer");             ///< 窗口操作句柄
    auto camera = std::make_shared<Camera>("camera", lidar_coord);          ///< 相机操作句柄
    auto view_3d = std::make_shared<View3D>("view_3d");                     ///< 3d渲染空间
    auto view_menu = std::make_shared<Menu>("menu");                        ///< 菜单空间
    auto view_plotter = std::make_shared<Plotter>("plotter");               ///< 绘图空间
    auto view_image = std::make_shared<ImageShower>("image", 2, 1, 30, 10); ///< 图片显示空间

    view_3d->SetCamera(camera);
    view_3d->AddUIItem(world_coord);
    view_3d->AddUIItem(lidar_coord);
    view_3d->AddUIItem(camera_coord);
    view_3d->AddUIItem(lidar_trajectory);

    viewer->AddView(view_menu, 0, 1, 0.0, 0.1);
    viewer->AddView(view_3d, 0, 1, 0.1, 0.8);
    viewer->AddView(view_plotter, 0, 1, 0.8, 1.0);
    viewer->AddView(view_image, 0, 0, 0, 0);

    /// 4. 配置菜单、绘图和图片显示空间，配置后布局无法改变
    ConfigMenu(view_menu, camera);

    view_plotter->AddPlotterItem("lidar_position", {"lx", "ly", "lz"}, -10, 600, -100, 100, 74, 10);
    view_plotter->AddPlotterItem("camera_position", {"cx", "cy", "cz"}, -10, 600, -100, 100, 74, 10);
    view_plotter->AddPlotterItem("lidar_quat", {"lqx", "lqy", "lqz", "lqw"}, -10, 600, -1, 1, 75, 0.2);
    view_plotter->AddPlotterItem("camera_quat", {"cqx", "cqy", "cqz", "cqw"}, -10, 600, -1, 1, 75, 0.2);

    view_image->AddImage("left_image", 376, 1241);
    view_image->AddImage("right_image", 376, 1241);

    /// 5. 启动可视化线程
    std::thread viewer_thread(&WindowImpl::Run, viewer);

    /// 6. 数据读取并更新
    while (1) {
        auto db = kitti_helper.LoadNext();
        if (!db.valid_)
            break;

        SE3 Twl = Twl0 * db.Tll;
        SE3 Twc = Twc0 * db.Tcc;

        auto point_cloud = FuseRingClouds(db.pointclouds_);
        auto frame_ui = std::make_shared<FrameUI>(Twc0 * db.Tcc, Vec3(0, 1, 0), 3); ///< frame ui 设置
        auto cloud_ui = std::make_shared<CloudUI>();                                ///< cloud ui 设置

        /// 各种ui的数据更新设置
        auto gray_factory = std::make_shared<GrayColor<PointXYZRT>>(point_cloud);
        cloud_ui->SetCloud<PointXYZRT>(point_cloud, Twl0 * db.Tll, gray_factory);
        camera_coord->ResetTwi(Twc);
        lidar_coord->ResetTwi(Twl);
        lidar_trajectory->AddPt(Twl);

        view_3d->AddUIItem(frame_ui);
        view_3d->AddUIItem(cloud_ui);

        /// 图像区域更新
        cv::Mat left_image, right_image;
        cv::cvtColor(db.left_image_, left_image, cv::COLOR_GRAY2BGR);
        cv::cvtColor(db.right_image_, right_image, cv::COLOR_GRAY2BGR);

        view_image->UpdateImage("left_image", "timestamp offset " + std::to_string(db.stamp_), left_image);
        view_image->UpdateImage("right_image", "timestamp offset " + std::to_string(db.stamp_), right_image);

        /// 绘图区域更新
        view_plotter->UpdatePlotterItem("lidar_position", Twl.translation().cast<float>());
        view_plotter->UpdatePlotterItem("camera_position", Twc.translation().cast<float>());
        view_plotter->UpdatePlotterItem("lidar_quat", Twl.unit_quaternion().coeffs().cast<float>());
        view_plotter->UpdatePlotterItem("camera_quat", Twc.unit_quaternion().coeffs().cast<float>());

        std::this_thread::sleep_for(10ms);
    }

    viewer_thread.join();

    return 0;
}
