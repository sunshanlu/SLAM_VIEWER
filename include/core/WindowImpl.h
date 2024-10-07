#pragma once

#include "Menu.hpp"
#include "Plotter.hpp"
#include "View3D.h"

namespace slam_viewer{
class WindowImpl {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    typedef std::shared_ptr<WindowImpl> Ptr;
    typedef std::shared_ptr<const WindowImpl> ConstPtr;
    typedef std::queue<std::function<void(void)>> TasksQueue;

    WindowImpl(std::string win_name = "SLAM Viewer", int width = 1920, int height = 1080);

    /// 渲染3d窗口内的所有元素，先更新再渲染
    void Render();

    /// 运行主循环
    void Run();

    /// 添加渲染View
    void AddView(View::Ptr view, pangolin::Attach bottom, pangolin::Attach top, pangolin::Attach left,
                 pangolin::Attach right, pangolin::Layout layout = pangolin::LayoutEqualVertical);

private:
    /// 创建展示布局
    void CreateDisplayLayout();

    const std::string window_name_; ///< 窗口名称
    int width_, height_;            ///< 窗口宽高

    std::atomic<bool> request_stop_; ///< 请求停止flag

    TasksQueue tasks_queue_;       ///< 创建View任务队列
    std::vector<View::Ptr> views_; ///< View列表，待渲染

    std::unordered_map<std::string, cv::Mat> images_; ///< 窗口名称和待渲染图像
    std::mutex image_mutex_;                          ///< 图像锁
};

}