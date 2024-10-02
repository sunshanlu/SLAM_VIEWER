#pragma once

#include "Common.h"

NAMESPACE_BEGIN

class Camera {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /// 相机状态
    enum class CameraState {
        FixedCamera,  ///< 固定相机
        FollowCamera, ///< 跟踪相机
        FreeCamera    ///< 自由相机
    };

    typedef std::shared_ptr<Camera> Ptr;
    typedef std::shared_ptr<const Camera> ConstPtr;

    /// 自由相机和固定相机构造函数
    Camera(std::string name, SE3 Twi = SE3(), bool fixed = false, float render_width = 1280.f,
           float render_height = 720.f, float focus_x = 500.f, float focus_y = 500.f, float camera_znear = 0.1f,
           float camera_zfar = 1000.f);

    /// Follow相机的构造函数
    Camera(std::string name, UIItem::Ptr follow_item, float render_width = 1280.f, float render_height = 720.f,
           float focus_x = 500.f, float focus_y = 500.f, float camera_znear = 0.1f, float camera_zfar = 1000.f);

    /// 设置相机跟踪元素，其他线程调用api
    void SetFollow(UIItem::Ptr follow_item);

    void SetFollow() {
        if (follow_item_)
            SetFollow(follow_item_);
    }

    /// 获取相机的渲染状态，仅渲染线程可用
    const pangolin::OpenGlRenderState &RenderState() const { return render_state_; }

    pangolin::OpenGlRenderState &RenderState() { return render_state_; }

    /// 设置相机的固定位姿，其他线程调用api
    void SetFixedPose(SE3 Twi);

    /// 设置位自由相机，可用鼠标控制相机位姿，其他线程调用api
    void SetFree();

    /// 绑定到View，仅渲染线程调用
    void BindDisplay(std::string display_name) { bind_display_name_ = std::move(display_name); }

    /// 更新相机的跟踪状态，仅渲染线程中调用
    void Update();

    /// 仅渲染线程可用，设置相机的视图矩阵
    void SetModelView(pangolin::OpenGlMatrix model_view) { render_state_.SetModelViewMatrix(std::move(model_view)); }

private:
    /// 创建渲染状态，仅渲染线程可用
    void CreateRenderState() {
        /// 视图矩阵，即Tic，i代表被观测者，c代表相机，即确定了相机在被观测坐标系下的相对位姿
        auto model_view = pangolin::ModelViewLookAt(0, 0, 1000, 0, 0, 0, pangolin::AxisX);
        auto proj = pangolin::ProjectionMatrix(last_render_width_, last_render_height_, camera_focus_x_,
                                               camera_focus_y_, 0.5 * last_render_width_, 0.5 * last_render_height_,
                                               camera_znear_, camera_zfar_);
        render_state_.SetModelViewMatrix(model_view);
    }

    /// 当绑定View发生变化时，保证相机渲染不会产生缩放
    void Keep3dScale();

    UIItem::Ptr follow_item_;                  ///< 相机跟踪的可视化元素
    pangolin::OpenGlRenderState render_state_; ///< 相机的opengl渲染参数
    std::string camera_name_;                  ///< 相机名称
    CameraState camera_state_;                 ///< 相机状态
    std::mutex render_pose_mutex_;             ///< 渲染位姿互斥量
    SE3 camera_fixed_Twi_;                     ///< 固定的相机位姿
    std::string bind_display_name_;            ///< 绑定渲染的View名称

    float last_render_width_;  ///< 上一次渲染窗口的宽度
    float last_render_height_; ///< 上一次渲染窗口的高度
    float camera_focus_x_;     ///< x方向上的焦距
    float camera_focus_y_;     ///< y方向上的焦距
    float camera_znear_;       ///< 相机渲染的z方向上的最近距离
    float camera_zfar_;        ///< 相机渲染的z方向上的最远距离
};

NAMESPACE_END
