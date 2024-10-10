#pragma once

#include "slam_viewer/core/Camera.h"
#include "slam_viewer/core/Common.h"

namespace slam_viewer{

/// 渲染3d窗口
class View3D : public View {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef std::shared_ptr<View3D> Ptr;
    typedef std::shared_ptr<const View3D> ConstPtr;

    typedef std::shared_ptr<pangolin::Handler3D> Handler3D;

    View3D(std::string name)
        : View(std::move(name)) {}

    /// 3d渲染函数
    void Render() override;

    /// 设置相机
    void SetCamera(Camera::Ptr camera);

    /// 添加ui_item
    void AddUIItem(UIItem::Ptr ui_item);

    /// 创建3d窗口布局
    void CreateDisplayLayout(pangolin::Layout layout = pangolin::LayoutEqualVertical) override;

private:
    std::vector<UIItem::Ptr> ui_items_; ///< View3D待渲染的3d元素
    Camera::Ptr camera_;                ///< 渲染View3D的相机
    Handler3D handler_;                 ///< 3d窗口的handler
    std::mutex mutex_;                  ///< 维护ui_items_的互斥量
};

}
