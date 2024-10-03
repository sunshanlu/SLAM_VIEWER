#include "ImageShower.h"
#include "WindowImpl.h"

using namespace slam_viewer;

int main(int argc, char **argv) {
    /// 1. 获取图片路径
    const std::string lpath = std::string(argv[1]) + "/image_0/";
    const std::string rpath = std::string(argv[1]) + "/image_1/";
    std::stringstream lstream, rstream;

    /// 2. 创建窗口和图像显示器
    auto viewer = std::make_shared<WindowImpl>();
    auto image_view = std::make_shared<ImageShower>("kitti_image", 2, 1, 40, 10);
    viewer->AddView(image_view, 0, 0, 0, 0);

    /// 3. 添加图像显示
    image_view->AddImage("left_image", 376, 1241);
    image_view->AddImage("right_image", 376, 1241);

    /// 4. 可视化线程启动
    std::thread vthread(&WindowImpl::Run, viewer);

    /// 5. 读取图像并更新显示
    int i = 0;
    while (1) {
        std::string lpath_i, rpath_i;
        lstream.clear();
        rstream.clear();
        lstream << lpath << std::setfill('0') << std::setw(6) << i << ".png";
        rstream << rpath << std::setfill('0') << std::setw(6) << i++ << ".png";
        lstream >> lpath_i;
        rstream >> rpath_i;
        cv::Mat limg = cv::imread(lpath_i, cv::IMREAD_COLOR);
        cv::Mat rimg = cv::imread(rpath_i, cv::IMREAD_COLOR);

        if (limg.empty() || rimg.empty())
            break;

        image_view->UpdateImage("left_image", lpath_i + "\nhaha next line", limg);
        image_view->UpdateImage("right_image", rpath_i + "\nhaha next line", rimg);

        std::this_thread::sleep_for(30ms);
    }

    vthread.join();

    return 0;
}