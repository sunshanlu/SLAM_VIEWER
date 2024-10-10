#include <fstream>
#include <sstream>
#include <thread>
#include <unordered_set>

#include <Eigen/Geometry>

#include "slam_viewer/core/WindowImpl.h"

using namespace slam_viewer;

int main(int argc, char **argv) {

    /// 1. 读取plot可视化数据，imu、gnss和odom数据
    std::string plot_path = argv[1];
    std::ifstream fin(plot_path);
    std::string line_str;

    std::vector<std::vector<float>> gnss_data, odom_data, imu_data;
    while (std::getline(fin, line_str)) {
        std::stringstream ss(line_str);
        std::string data_type;
        ss >> data_type;

        double timestamp;
        if (data_type == "ODOM") {
            int left_pulse, right_pulse;
            ss >> timestamp >> left_pulse >> right_pulse;
            odom_data.push_back({(float)left_pulse, (float)right_pulse});
        } else if (data_type == "GNSS") {
            float latitude, longitude, height, angle;
            ss >> latitude >> longitude >> height >> angle;
            gnss_data.push_back({latitude, longitude, height, angle});
        } else {
            float gx, gy, gz, ax, ay, az;
            ss >> timestamp >> gx >> gy >> gz >> ax >> ay >> az;
            imu_data.push_back({gx, gy, gx, ax, ay, az});
        }
    }

    /// 2. 创建一个可视化窗口
    auto viewer = std::make_shared<WindowImpl>();
    auto plotter = std::make_shared<Plotter>("plotter");
    viewer->AddView(plotter, 0, 1, 0, 1);
    plotter->AddPlotterItem("ODOM", {"left_pulse", "right_pulse"});
    plotter->AddPlotterItem("GNSS", {"latitude", "longitude", "height", "angle"});
    plotter->AddPlotterItem("IMU", {"gyro_x", "gyro_y", "gyro_z", "acc_x", "acc_y", "acc_z"});

    std::thread viewer_thread(&WindowImpl::Run, viewer);

    /// 测试AddPt函数
    int odom_id = 0, gnss_id = 0, imu_id = 0;
    while (odom_id < odom_data.size() && gnss_id < gnss_data.size() && imu_id < imu_data.size()) {
        plotter->UpdatePlotterItem("ODOM", odom_data[odom_id++]);
        plotter->UpdatePlotterItem("GNSS", gnss_data[gnss_id++]);
        plotter->UpdatePlotterItem("IMU", imu_data[imu_id]);
        imu_id += 10;
        std::this_thread::sleep_for(10ms);
    }

    viewer_thread.join();

    return 0;
}
