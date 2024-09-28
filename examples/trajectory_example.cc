#include <fstream>
#include <sstream>
#include <thread>
#include <unordered_set>

#include <Eigen/Geometry>

#include "PointTypes.h"
#include "TrajectoryUI.h"
#include "WindowImpl.h"

using namespace slam_viewer;

int main(int argc, char **argv) {

    /// 1. 读取点云数据
    std::string trajectory_path = argv[1];
    std::ifstream fin(trajectory_path);
    std::string line_str;

    std::vector<SE3> trajectory_array;
    while (std::getline(fin, line_str)) {
        std::stringstream ss(line_str);
        double stamp;
        Vec3 twi;
        Eigen::Quaternionf qwi;
        ss >> stamp >> twi.x() >> twi.y() >> twi.z() >> qwi.x() >> qwi.y() >> qwi.z() >> qwi.w();

        SE3 Twi(qwi, twi);
        trajectory_array.push_back(Twi);
    }

    /// 2. 创建一个点云UI
    TrajectoryUI::Ptr trajectory_ui = std::make_shared<TrajectoryUI>();

    /// 3. 创建一个可视化窗口
    auto viewer = std::make_shared<WindowImpl>();
    viewer->AddUIItem(trajectory_ui);

    std::thread viewer_thread(&WindowImpl::Run, viewer);

    /// 测试AddPt函数
    for (const auto &Twi : trajectory_array) {
        trajectory_ui->AddPt(Twi.translation());
        
        std::this_thread::sleep_for(1ms);
    }

    SE3 Twi;
    /// 测试ResetTwi函数
    for (int i = 0; i < 1000; ++i) {
        Twi.translation().x() += 1;
        trajectory_ui->ResetTwi(Twi);

        std::this_thread::sleep_for(100ms);
    }

    viewer_thread.join();

    return 0;
}
