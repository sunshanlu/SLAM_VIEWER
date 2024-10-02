#pragma once

#include "Camera.h"
#include "Menu.hpp"

NAMESPACE_BEGIN
class WindowImpl {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    typedef std::shared_ptr<WindowImpl> Ptr;
    typedef std::shared_ptr<const WindowImpl> ConstPtr;
    typedef std::unique_ptr<pangolin::Plotter> PlotUniquePtr;
    typedef std::shared_ptr<pangolin::DataLog> LogPtr;

    WindowImpl(std::string win_name = "SLAM Viewer", int width = 1920, int height = 1080);

    ~WindowImpl() { cv::destroyAllWindows(); }

    /// 添加图像显示
    void AddImageShow(std::string win_name);

    /// 设置相机
    void SetCamera(Camera::Ptr camera) { default_camera_ = camera; }

    /// 添加数据图显示
    void AddPlotterShow(std::string plot_name, std::vector<std::string> labels, float left = -10, float right = 600,
                        float bottom = -11, float top = 11, float tickx = 75, float ticky = 2);

    /// 由于Menu可能会改变内部元素，因此设计成虚函数的形式
    void AddMenuShow();
    
    /// 添加菜单元素，设计成可继承的形式，便于拓展
    virtual void AddMenuItems();

    /// 添加ui_item
    void AddUIItem(UIItem::Ptr ui_item) {
        add_3d_ = true;
        ui_items_.push_back(std::move(ui_item));
    }

    /// 渲染3d窗口内的所有元素，先更新再渲染
    void Render3d();

    /// 渲染图像
    void RenderImgs();

    /// 运行主循环
    void Run();

    /// 更新图像信息，供外部api调用
    void UpdateImages(const std::string &win_name, cv::Mat image);

    /// 更新绘图数据，供外部api调用
    void UpdatePlotters(const std::string &plot_name, const std::vector<float> &data);

private:
    /// 创建展示布局
    void CreateDisplayLayout();

    std::vector<UIItem::Ptr> ui_items_;                                 ///< 窗口待渲染的元素
    std::unordered_map<std::string, cv::Mat> images_;                   ///< 窗口名称和待渲染图像
    std::unordered_map<std::string, LogPtr> plotters_;                  ///< 窗口名称和绘图数据
    std::unordered_map<std::string, std::vector<float>> plotter_param_; ///< 绘图窗口参数
    std::vector<PlotUniquePtr> plotter_tables_;                         ///< plot画板

    Menu::Ptr menu_; ///< 菜单

    const std::string window_name_; ///< 窗口名称
    int width_, height_;            ///< 窗口宽高
    Camera::Ptr default_camera_;    ///< 默认渲染相机

    std::string threed_display_name_; ///< 3d渲染空间的名称
    std::string menu_display_name_;   ///< 菜单渲染空间名称
    std::string plot_display_name_;   ///< 数据图渲染空间名称
    std::atomic<bool> request_stop_;  ///< 请求停止flag

    std::mutex image_mutex_; ///< 图像锁

    bool add_plot_; ///< 是否添加数据图
    bool add_menu_; ///< 是否添加菜单选项
    bool add_imgs_; ///< 是否添加图像显示模块
    bool add_3d_;   ///< 是否添加3d模块
};

NAMESPACE_END