#pragma once

#include "Common.h"

NAMESPACE_BEGIN
class WindowImpl {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    typedef std::shared_ptr<WindowImpl> Ptr;
    typedef std::shared_ptr<const WindowImpl> ConstPtr;
    typedef std::unique_ptr<pangolin::Plotter> PlotUniquePtr;
    typedef std::shared_ptr<pangolin::DataLog> LogPtr;

    WindowImpl(std::string win_name = "SLAM Viewer", int width = 1920, int height = 1080, float cam_focus = 5000,
               float cam_z_near = 0.1, float cam_z_far = 1e10);

    virtual ~WindowImpl() { cv::destroyAllWindows(); }

    /// 添加图像显示
    void AddImageShow(std::string win_name);

    /// 添加数据图显示
    void AddPlotterShow(std::string plot_name, std::vector<std::string> labels);

    /// 由于Menu可能会改变内部元素，因此设计成虚函数的形式
    virtual void AddMenuShow();

    /// 添加ui_item
    void AddUIItem(UIItem::Ptr ui_item) { ui_items_.push_back(std::move(ui_item)); }

    /// 渲染3d窗口内的所有元素，先更新再渲染
    void Render() {
        auto &display_3d = pangolin::Display(threed_display_name_);
        if (display_3d.IsShown()) {
            display_3d.Activate(s_cam_main_);
            if (ui_items_.empty())
                return;

            for (const auto &item : ui_items_) {
                item->Update();
                item->Render();
            }
        }
    }

    void Run();

    /// 创建展示布局
    void CreateDisplayLayout();

    /// 更新图像信息
    void UpdateImages(const std::string &win_name, cv::Mat image);

    /// 更新绘图数据
    void UpdatePlotters(const std::string &plot_name, const std::vector<float> &data);

private:
    std::vector<UIItem::Ptr> ui_items_;                   ///< 窗口待渲染的元素
    std::unordered_map<std::string, cv::Mat> images_;     ///< 窗口名称和待渲染图像
    std::unordered_map<std::string, LogPtr> plotters_;    ///< 窗口名称和绘图数据
    std::vector<PlotUniquePtr> plotter_tables_;           ///< plot画板
    std::vector<std::string> button_names_;               ///< 按钮名称
    std::vector<std::function<void(void)>> button_funcs_; ///< 按钮函数指针
    std::vector<pangolin::Var<bool>> buttons_;            ///< 按钮

    const std::string window_name_;            ///< 窗口名称
    int width_, height_;                       ///< 窗口宽高
    pangolin::OpenGlRenderState s_cam_main_;   ///< opengl的渲染状态
    float cam_focus_, cam_z_near_, cam_z_far_; ///< 焦距,z方向最小距离,z方向最大距离

    std::string threed_display_name_; ///< 3d渲染空间的名称
    std::string menu_display_name_;   ///< 菜单渲染空间名称
    std::string plot_display_name_;   ///< 数据图渲染空间名称
    std::atomic<bool> request_stop_;  ///< 请求停止flag

    std::mutex image_mutex_; ///< 图像锁

    bool add_plot_; ///< 是否添加数据图
    bool add_menu_; ///< 是否添加菜单选项
    bool add_imgs_; ///< 是否添加图像显示模块

    SE3 curr_pose_; ///< 当前位姿
};

NAMESPACE_END