#include "WindowImpl.h"

NAMESPACE_BEGIN

/**
 * @brief WindowImpl的有参和默认构造函数
 *
 * @param win_name  输入的窗口名称
 * @param width     输入的窗口宽度
 * @param height    输入的窗口高度
 * @param cam_focus 输入的焦距
 * @param cam_z_near 输入的最近可视距离
 * @param cam_z_far  输入的远视可视距离
 */
WindowImpl::WindowImpl(std::string win_name, int width, int height)
    : window_name_(std::move(win_name))
    , width_(std::move(width))
    , height_(std::move(height))
    , request_stop_(false) {
    pangolin::CreateWindowAndBind(window_name_, width_, height_);
    pangolin::GetBoundWindow()->RemoveCurrent(); ///< 将该窗口从主线程中移除
}

/**
 * @brief 窗口运行主流程，可以单独线程运行
 * @details
 *      1. 3D空间的可视化
 *      2. 图像可视化
 *      3. plot可视化部分在pangolin中已经做好了，线程安全
 */
void WindowImpl::Run() {
    pangolin::BindToContext(window_name_);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CreateDisplayLayout();

    while (!pangolin::ShouldQuit() && !request_stop_.load()) {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Render();
        pangolin::FinishFrame();
        cv::waitKey(30);
    }
}

/// 添加渲染View
void WindowImpl::AddView(View::Ptr view, pangolin::Attach bottom, pangolin::Attach top, pangolin::Attach left,
                         pangolin::Attach right, pangolin::Layout layout) {
    views_.push_back(view);
    auto task = [=]() {
        view->CreateDisplayLayout(layout);
        view->SetBounds(bottom, top, left, right);
    };

    tasks_queue_.push(task);
}

/**
 * @brief 创建布局
 * @details
 *      在这里执行View的CreateDisplayLayout函数，实现pangolin::View的创建和布局设置
 */
void WindowImpl::CreateDisplayLayout() {
    while (!tasks_queue_.empty()) {
        tasks_queue_.front()();
        tasks_queue_.pop();
    }
}

/**
 * @brief 渲染窗口内的所有View
 *
 */
void WindowImpl::Render() {
    for (size_t i = 0; i < views_.size(); ++i)
        views_[i]->Render();
}

NAMESPACE_END