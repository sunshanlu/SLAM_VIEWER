#include <thread>
#include <unordered_set>

#include <pcl/io/impl/pcd_io.hpp>

#include "slam_viewer/ui/CloudUI.hpp"
#include "slam_viewer/core/Menu.hpp"
#include "slam_viewer/core/PointTypes.h"
#include "slam_viewer/core/View3D.h"
#include "slam_viewer/core/WindowImpl.h"

using namespace slam_viewer;

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
        if (pushed)
            camera->SetModelView(pangolin::ModelViewLookAt(0, 0, 1000, 0, 0, 0, pangolin::AxisX));
    });

    /// 4.3 设置前置视角
    menu->AddButtonItem("Front View", [=](bool pushed) {
        if (pushed)
            camera->SetModelView(pangolin::ModelViewLookAt(-50, 0, 10, 0, 0, 0, pangolin::AxisZ));
    });
}

int main(int argc, char **argv) {

    /// 1. 读取点云数据
    const std::string pcd_path(argv[1]);
    pcl::PointCloud<PointXYZR>::Ptr cloud_ptr = pcl::make_shared<pcl::PointCloud<PointXYZR>>();
    pcl::io::loadPCDFile<PointXYZR>(pcd_path, *cloud_ptr);

    /// 2. 创建一个点云UI
    SE3 Twi;
    CloudUI::Ptr cloud_ui = std::make_shared<CloudUI>();
    ColorFactory<PointXYZR>::Ptr ring_factory = std::make_shared<RingColor<PointXYZR>>(cloud_ptr);
    cloud_ui->SetCloud<PointXYZR>(cloud_ptr, Twi, ring_factory);

    /// 3. 创建一个可视化窗口和3d可视化和菜单可视化
    auto viewer = std::make_shared<WindowImpl>();
    auto camera = std::make_shared<Camera>("3d_view_camera", cloud_ui);
    auto view3d = std::make_shared<View3D>("3d_view");
    auto menu = std::make_shared<Menu>("menu");

    view3d->AddUIItem(cloud_ui);
    view3d->SetCamera(camera);
    viewer->AddView(view3d, 0, 1, 0.2, 1);
    viewer->AddView(menu, 0, 1, 0, 0.2);

    /// 4. 对菜单进行配置
    ConfigMenu(menu, camera);

    std::thread viewer_thread(&WindowImpl::Run, viewer);

    /// 测试ResetTwi函数
    for (int i = 0; i < 1000; ++i) {
        Twi.translation().x() += 1;
        cloud_ui->ResetTwi(Twi);

        std::this_thread::sleep_for(100ms);
    }

    viewer_thread.join();

    return 0;
}
