#include "Camera.h"

namespace slam_viewer{

/**
 * @brief 自由相机和固定相机构造函数
 *
 * @param name          输入的相机名称
 * @param Twi           输入的相机初始位姿或固定位姿
 * @param fixed         输入的相机是否固定
 * @param render_width  输入的渲染宽度
 * @param render_height 输入的渲染高度
 * @param focus_x       输入的x轴焦距
 * @param focus_y       输入的y轴焦距
 * @param camera_znear  输入的近裁剪平面
 * @param camera_zfar   输入的远裁剪平面
 */
Camera::Camera(std::string name, SE3 Twi, bool fixed, float render_width, float render_height, float focus_x,
               float focus_y, float camera_znear, float camera_zfar)
    : camera_name_(std::move(name))
    , last_render_height_(render_height)
    , last_render_width_(render_width)
    , camera_focus_x_(focus_x)
    , camera_focus_y_(focus_y)
    , camera_znear_(camera_znear)
    , camera_zfar_(camera_zfar) {
    CreateRenderState();

    if (!fixed) {
        render_state_.Follow(Twi.matrix());
        SetFree();
    } else
        SetFixedPose(Twi);
}

/**
 * @brief Follow相机的构造函数
 *
 * @param name          输入的相机名称
 * @param follow_item   输入的跟踪元素
 * @param render_width  输入的渲染宽度
 * @param render_height 输入的渲染高度
 * @param focus_x       输入的x轴焦距
 * @param focus_y       输入的y轴焦距
 * @param camera_znear  输入的近裁剪平面
 * @param camera_zfar   输入的远裁剪平面
 *
 * @exception std::runtime_error 当follow_item为空指针时抛出异常
 */
Camera::Camera(std::string name, UIItem::Ptr follow_item, float render_width, float render_height, float focus_x,
               float focus_y, float camera_znear, float camera_zfar)
    : follow_item_(std::move(follow_item))
    , camera_name_(std::move(name))
    , last_render_height_(render_height)
    , last_render_width_(render_width)
    , camera_focus_x_(focus_x)
    , camera_focus_y_(focus_y)
    , camera_znear_(camera_znear)
    , camera_zfar_(camera_zfar) {
    CreateRenderState();

    SetFollow(follow_item_);
}

/**
 * @brief 设置相机跟踪元素，其他线程调用api
 *
 * @param follow_item 输入的跟踪元素
 */
void Camera::SetFollow(UIItem::Ptr follow_item) {
    if (!follow_item)
        throw std::runtime_error("follow item is nullptr");

    std::lock_guard<std::mutex> lock(render_pose_mutex_);
    camera_state_ = CameraState::FollowCamera;
    follow_item_ = std::move(follow_item);
    render_state_.Follow(follow_item_->GetTwi().matrix());
}

/**
 * @brief 设置相机的固定位姿，其他线程调用api，线程安全
 *
 * @param Twi 输入的固定位姿
 */
void Camera::SetFixedPose(SE3 Twi) {
    std::lock_guard<std::mutex> lock(render_pose_mutex_);
    camera_state_ = CameraState::FixedCamera;
    camera_fixed_Twi_ = std::move(Twi);
    render_state_.Follow(camera_fixed_Twi_.matrix());
}

/**
 * @brief 设置为自由相机，可用鼠标控制相机位姿，线程安全
 * 
 */
void Camera::SetFree() {
    std::lock_guard<std::mutex> lock(render_pose_mutex_);
    camera_state_ = CameraState::FreeCamera;
}

/**
 * @brief 更新相机的跟踪状态，渲染线程中调用
 *
 */
void Camera::Update() {
    Keep3dScale();

    std::lock_guard<std::mutex> lock(render_pose_mutex_);
    switch (camera_state_) {
    case CameraState::FollowCamera:
        render_state_.Follow(follow_item_->GetTwi().matrix());
        break;

    case CameraState::FixedCamera:
        render_state_.Follow(camera_fixed_Twi_.matrix());
        break;

    default:
        break;
    }
}

/**
 * @brief 保证当窗口的尺寸发生改变时，3d空间的可视化不会改变
 *
 */
void Camera::Keep3dScale() {
    if (bind_display_name_.empty())
        return;

    auto &threed_display = pangolin::Display(bind_display_name_);
    if (threed_display.v.h != last_render_height_ || threed_display.v.w != last_render_width_) {
        last_render_height_ = threed_display.v.h;
        last_render_width_ = threed_display.v.w;
        auto proj_mat_main = pangolin::ProjectionMatrix(last_render_width_, last_render_height_, camera_focus_x_,
                                                        camera_focus_y_, 0.5 * last_render_width_,
                                                        0.5 * last_render_height_, camera_znear_, camera_zfar_);
        render_state_.SetProjectionMatrix(proj_mat_main);
    }
}

}