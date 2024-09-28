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
WindowImpl::WindowImpl(std::string win_name, int width, int height, float cam_focus, float cam_z_near, float cam_z_far)
    : window_name_(std::move(win_name))
    , width_(std::move(width))
    , height_(std::move(height))
    , cam_focus_(std::move(cam_focus))
    , cam_z_near_(std::move(cam_z_near))
    , cam_z_far_(std::move(cam_z_far))
    , threed_display_name_("3d_display")
    , menu_display_name_("menu")
    , plot_display_name_("plotters")
    , request_stop_(false)
    , add_plot_(false)
    , add_menu_(false)
    , add_imgs_(false) {
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
 */
void WindowImpl::AddPlotterShow(std::string plot_name, std::vector<std::string> labels) {
    add_plot_ = true;

    if (plotters_.find(plot_name) != plotters_.end())
        return;

    auto data_log = std::make_shared<pangolin::DataLog>();
    data_log->SetLabels(std::move(labels));
    plotters_.insert({std::move(plot_name), data_log});
    auto plotter_table = std::make_unique<pangolin::Plotter>(data_log.get(), -10, 600, -11, 11, 75, 2);
    plotter_table->SetBounds(0.02, 0.98, 0.0, 1.0);
    plotter_table->Track("$i");
    plotter_table->SetBackgroundColour(pangolin::Colour(248. / 255, 248. / 255, 255. / 255));
    plotter_tables_.push_back(std::move(plotter_table));
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

    /// 1. follow 相机跟踪模式
    button_names_.push_back(menu_display_name_ + "." + "Follow");
    button_funcs_.push_back([this]() -> void { s_cam_main_.Follow(curr_pose_.matrix()); });

    /// 2. 设置上帝视角
    button_names_.push_back(menu_display_name_ + "." + "God View");
    button_funcs_.push_back(
        [this]() { s_cam_main_.SetModelViewMatrix(pangolin::ModelViewLookAt(0, 0, 1000, 0, 0, 0, pangolin::AxisY)); });

    /// 3. 设置前置视角
    button_names_.push_back(menu_display_name_ + "." + "Front View");
    button_funcs_.push_back([this]() {
        s_cam_main_.SetModelViewMatrix(pangolin::ModelViewLookAt(-50, 0, 10, 50, 0, 10, pangolin::AxisZ));
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

        for (int i = 0; i < buttons_.size(); ++i) {
            auto button = buttons_[i];
            auto button_func = button_funcs_[i];
            if (button)
                button_func();
        }

        Render();

        std::lock_guard<std::mutex> lock(image_mutex_);
        for (auto &image_item : images_) {
            if (!image_item.second.empty())
                cv::imshow(image_item.first, image_item.second);
        }

        pangolin::FinishFrame();
        std::this_thread::sleep_for(10ms);
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

    auto proj_mat_main = pangolin::ProjectionMatrix(width_, height_ * range, cam_focus_, cam_focus_, width_ * 0.5f,
                                                    height_ * range * 0.5, cam_z_near_, cam_z_far_);
    auto model_view_main = pangolin::ModelViewLookAt(0, 0, 1000, 0, 0, 0, pangolin::AxisY);
    s_cam_main_ = pangolin::OpenGlRenderState(std::move(proj_mat_main), std::move(model_view_main));

    /// menu空间窗口
    if (add_menu_) {
        pangolin::View &menu = pangolin::CreatePanel(menu_display_name_).SetBounds(0.0, 1.0, 0.0, left_range);
        for (const auto &button_name : button_names_) {
            pangolin::Var<bool> button(button_name, false, true);
            buttons_.push_back(button);
        }
    }

    /// 3d空间窗口
    pangolin::View &threed_display = pangolin::Display(threed_display_name_)
                                         .SetBounds(0, 1, left_range, 1 - right_range)
                                         .SetHandler(new pangolin::Handler3D(s_cam_main_));

    /// plot空间窗口
    if (add_plot_) {
        pangolin::View &plot_display = pangolin::Display(plot_display_name_)
                                           .SetBounds(0, 1, 1 - right_range, 1)
                                           .SetLayout(pangolin::LayoutEqualVertical);
        for (auto &plotter : plotter_tables_)
            plot_display.AddDisplay(*plotter);
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

NAMESPACE_END