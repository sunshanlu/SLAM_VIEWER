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
    , threed_display_name_("3d_display")
    , menu_display_name_("menu")
    , plot_display_name_("plotters")
    , request_stop_(false)
    , add_plot_(false)
    , add_menu_(false)
    , add_imgs_(false)
    , add_3d_(false) {
    pangolin::CreateWindowAndBind(window_name_, width_, height_);
    pangolin::GetBoundWindow()->RemoveCurrent(); ///< 将该窗口从主线程中移除
}

/**
 * @brief 添加图像可视化窗口
 *
 * @param win_name  输入的图像可视化窗口名称
 */
void WindowImpl::AddImageShow(std::string win_name) {
    add_imgs_ = true;

    if (images_.find(win_name) != images_.end())
        return;
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    images_.insert({std::move(win_name), cv::Mat()});
}

/**
 * @brief 添加数据图可视化窗口
 *
 * @param plot_name 输入的数据图窗口名称
 * @param labels    输入的数据图的标签
 * @param left      输入的左边界，default -10
 * @param right     输入的右边界，default 600
 * @param bottom    输入的下边界，default -11
 * @param top       输入的上边界，default 11
 * @param tickx     输入的x轴刻度间隔，default 75
 * @param ticky     输入的y轴刻度间隔，default 2
 */
void WindowImpl::AddPlotterShow(std::string plot_name, std::vector<std::string> labels, float left, float right,
                                float bottom, float top, float tickx, float ticky) {
    add_plot_ = true;

    if (plotters_.find(plot_name) != plotters_.end())
        return;

    auto data_log = std::make_shared<pangolin::DataLog>();
    data_log->SetLabels(std::move(labels));
    plotters_.insert({plot_name, data_log});
    plotter_param_.insert({std::move(plot_name), {left, right, bottom, top, tickx, ticky}});
}

/**
 * @brief 添加菜单可视化接口布局
 * @note 该函数为虚函数，可供用户自由定制菜单的接口
 * @details 给定了默认的菜单接口
 *      1. 相机跟踪模式
 *      2. 设置上帝视角
 *      3. 设置前置视角
 */
void WindowImpl::AddMenuShow() {
    add_menu_ = true;

    menu_ = std::make_shared<Menu>(menu_display_name_);
    AddMenuItems();
}

/**
 * @brief 添加菜单元素，设计成可继承的形式，便于拓展
 *
 */
void WindowImpl::AddMenuItems() {
    /// 1. follow 相机跟踪模式
    menu_->AddCheckBoxItem("Follow", [this](bool checked) {
        if (checked)
            default_camera_->SetFollow();
    });

    /// 2. 设置上帝视角
    menu_->AddButtonItem("God View", [this](bool pushed) {
        if (pushed)
            default_camera_->SetModelView(pangolin::ModelViewLookAt(0, 0, 1000, 0, 0, 0, pangolin::AxisX));
    });

    /// 3. 设置前置视角
    menu_->AddButtonItem("Front View", [this](bool pushed) {
        if (pushed)
            default_camera_->SetModelView(pangolin::ModelViewLookAt(-50, 0, 10, 0, 0, 0, pangolin::AxisZ));
    });
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

        menu_->Update();           ///< menu菜单功能函数运行
        default_camera_->Update(); ///< 相机渲染状态更新

        Render3d();
        RenderImgs();
        pangolin::FinishFrame();
        std::this_thread::sleep_for(30ms);
    }
}

/**
 * @brief 创建布局
 * @details
 *      1. 设置为四部分，其中图像可视化使用opencv进行
 *      2. 0.2部分的menu可视化
 *      3. 0.6部分的3d空间可视化
 *      4. 0.2部分的数据plot可视化
 */
void WindowImpl::CreateDisplayLayout() {
    float range = 1.0;
    float left_range = 0.0;
    float right_range = 0.0;

    if (add_plot_) {
        range -= 0.2;
        right_range = 0.2;
    }
    if (add_menu_) {
        range -= 0.2;
        left_range = 0.2;
    }

    if (!default_camera_)
        default_camera_ = std::make_shared<Camera>("default_camera_");

    /// menu空间窗口，但是menu窗口不可以单独出现
    if (add_menu_ && (add_3d_ || add_plot_)) {

        /// 让menu接收Bounds函数，用来设置布局信息
        menu_->CreateDisplayLayout();
        if (add_3d_)
            menu_->SetBounds(0.0, 1.0, 0.0, left_range);
        else
            menu_->SetBounds(0.0, 1.0, 0.0, 0.5);
    }

    /// 3d空间窗口
    if (add_3d_) {
        pangolin::View &threed_display = pangolin::Display(threed_display_name_)
                                             .SetBounds(0, 1, left_range, 1 - right_range)
                                             .SetHandler(new pangolin::Handler3D(default_camera_->RenderState()));
    }

    /// plot空间窗口
    if (add_plot_) {
        pangolin::View &plot_display = pangolin::Display(plot_display_name_).SetLayout(pangolin::LayoutEqualVertical);
        /// 如果3d空间存在，则plot空间在3d空间的右侧
        if (add_3d_)
            plot_display.SetBounds(0, 1, 1 - right_range, 1);
        /// 如果3d空间不存在，且menu_存在，则plot空间在menu空间的右侧
        else if (add_menu_)
            plot_display.SetBounds(0, 1, 0.5, 1.0);
        else
            plot_display.SetBounds(0, 1, 0, 1);

        for (auto &plotter : plotters_) {
            const auto &plotter_param = plotter_param_.at(plotter.first);

            auto plotter_table = std::make_unique<pangolin::Plotter>(
                plotter.second.get(), plotter_param[0], plotter_param[1], plotter_param[2], plotter_param[3],
                plotter_param[4], plotter_param[5]);
            plotter_table->SetBounds(0.02, 0.98, 0.0, 1.0);
            plotter_table->Track("$i");
            plotter_table->SetBackgroundColour(pangolin::Colour(248. / 255, 248. / 255, 255. / 255));
            plot_display.AddDisplay(*plotter_table);
            plotter_tables_.push_back(std::move(plotter_table));
        }
    }
}

/**
 * @brief 更新图像信息，是外部其他线程调用的接口
 *
 * @param win_name 输入的图像窗口名称
 * @param image    输入的待可视化的图像
 */
void WindowImpl::UpdateImages(const std::string &win_name, cv::Mat image) {
    if (images_.find(win_name) == images_.end())
        return;

    std::lock_guard<std::mutex> lock(image_mutex_);
    images_[win_name] = image;
}

/**
 * @brief 更新绘图数据，是外部其他线程调用的接口
 * @note 并未使用互斥量来保证线程安全，原因在于pangolin的DataLog类是线程安全的
 *
 * @param plot_name 输入的绘图窗口名称
 * @param data      输入的绘图数据
 */
void WindowImpl::UpdatePlotters(const std::string &plot_name, const std::vector<float> &data) {
    if (plotters_.find(plot_name) == plotters_.end())
        return;

    plotters_[plot_name]->Log(data);
}

/**
 * @brief 渲染3d窗口内的所有元素，先更新再渲染
 *
 */
void WindowImpl::Render3d() {
    auto &display_3d = pangolin::Display(threed_display_name_);
    if (display_3d.IsShown()) {
        display_3d.Activate(default_camera_->RenderState());
        default_camera_->BindDisplay(threed_display_name_);
        if (ui_items_.empty())
            return;

        for (const auto &item : ui_items_) {
            item->Update();
            item->Render();
        }
    }
}

/**
 * @brief 渲染图像
 *
 */
void WindowImpl::RenderImgs() {
    std::lock_guard<std::mutex> lock(image_mutex_);
    for (auto &image_item : images_) {
        if (!image_item.second.empty())
            cv::imshow(image_item.first, image_item.second);
    }
}

NAMESPACE_END