#include <thread>
#include <unordered_set>

#include <pcl/io/impl/pcd_io.hpp>

#include "CloudUI.hpp"
#include "PointTypes.h"
#include "WindowImpl.h"

using namespace slam_viewer;

int main(int argc, char **argv) {

    /// 1. 读取点云数据
    const std::string pcd_path(argv[1]);
    pcl::PointCloud<PointXYZR>::Ptr cloud_ptr = pcl::make_shared<pcl::PointCloud<PointXYZR>>();
    pcl::io::loadPCDFile<PointXYZR>(pcd_path, *cloud_ptr);

    /// 统计cloud_ptr中的ring信息
    std::unordered_map<int, int> rings;
    for (auto &point : cloud_ptr->points) {
        if (rings.find(point.ring) != rings.end()) {
            rings[point.ring] += 1;
        } else
            rings.insert({point.ring, 1});
    }

    /// 2. 创建一个点云UI
    SE3 Twi;
    CloudUI::Ptr cloud_ui = std::make_shared<CloudUI>();
    ColorFactory<PointXYZR>::Ptr ring_factory = std::make_shared<RingColor<PointXYZR>>(cloud_ptr);
    cloud_ui->SetCloud<PointXYZR>(cloud_ptr, Twi, ring_factory);

    /// 3. 创建一个可视化窗口
    auto viewer = std::make_shared<WindowImpl>();
    viewer->AddUIItem(cloud_ui);
    viewer->AddMenuShow();

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
