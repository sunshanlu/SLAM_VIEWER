#include "slam_viewer/core/View3D.h"

namespace slam_viewer{

/**
 * @brief View3D空间渲染函数
 *
 */
void View3D::Render() {
    camera_->Update();

    auto &display_3d = pangolin::Display(name_);
    if (display_3d.IsShown()) {
        display_3d.Activate(camera_->RenderState());
        camera_->BindDisplay(name_);

        std::lock_guard<std::mutex> lock(mutex_);
        if (ui_items_.empty())
            return;

        for (const auto &item : ui_items_) {
            item->Update();
            item->Render();
        }
    }
}

/// 创建3d布局
void View3D::CreateDisplayLayout(pangolin::Layout layout) {
    View::CreateDisplayLayout(layout);

    handler_ = std::make_shared<pangolin::Handler3D>(camera_->RenderState());
    pangolin::Display(name_).SetHandler(handler_.get());
}

/**
 * @brief View 3d添加渲染的UIItem，非初始化阶段，线程安全
 * @note View 3d的添加UIItem api并不是初始化过程，可以随时向其中添加渲染物体
 * @param ui_item 待添加的UIItem
 */
void View3D::AddUIItem(UIItem::Ptr ui_item) {
    if (!ui_item)
        return;

    std::lock_guard<std::mutex> lock(mutex_);
    ui_items_.push_back(std::move(ui_item));
}

/**
 * @brief 向View 3d中设置相机，仅初始化阶段调用，非线程安全
 *
 * @param camera 设置的相机指针
 */
void View3D::SetCamera(Camera::Ptr camera) {
    camera_ = std::move(camera);
    camera_->BindDisplay(name_);
}

}