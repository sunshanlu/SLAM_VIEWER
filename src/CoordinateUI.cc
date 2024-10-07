#include "CoordinateUI.h"

namespace slam_viewer{

/**
 * @brief 坐标系的构造函数
 *
 * @param arrow_length  输入的轴长
 * @param Twi           输入的坐标系的位姿
 */
CoordinateUI::CoordinateUI(float arrow_length, SE3 Twi)
    : UIItem(Vec3(0.0, 0.0, 0.0), 5.0, 5.0, Twi) {
    Eigen::AngleAxisf z_rot(0.5 * M_PI, Vec3(0, 0, 1)); ///< 沿着z轴旋转90度
    Eigen::AngleAxisf y_rot(0.5 * M_PI, Vec3(0, 1, 0)); ///< 沿着y轴旋转90度
    Eigen::AngleAxisf x_rot(0.5 * M_PI, Vec3(1, 0, 0)); ///< 沿着x轴旋转90度

    x_rot_ = SE3(x_rot.toRotationMatrix(), Vec3(0.0, 0.0, 0.0));
    y_rot_ = SE3(y_rot.toRotationMatrix(), Vec3(0.0, 0.0, 0.0));
    z_rot_ = SE3(z_rot.toRotationMatrix(), Vec3(0.0, 0.0, 0.0));

    // clang-format off
    x_axis_ = std::make_shared<ArrowUI>(Twi_, arrow_length, Vec3(1.0, 0.0, 0.0), 
                                        ArrowUI::HeadType::FixedHeadRatio);
    y_axis_ = std::make_shared<ArrowUI>(Twi_ * z_rot_, arrow_length, Vec3(0.0, 1.0, 0.0), 
                                        ArrowUI::HeadType::FixedHeadRatio);
    z_axis_ = std::make_shared<ArrowUI>(Twi_ * y_rot_.inverse(), arrow_length, Vec3(0.0, 0.0, 1.0),
                                        ArrowUI::HeadType::FixedHeadRatio);
    // clang-format on
}

/**
 * @brief 重置坐标轴的长度，非渲染线程调用
 *
 * @param arrow_length 输入的新的坐标轴长度
 */
void CoordinateUI::ResetLength(float arrow_length){
    std::lock_guard<std::mutex> lock(mutex_);

    x_axis_->ResetArrowLength(arrow_length);
    y_axis_->ResetArrowLength(arrow_length);
    z_axis_->ResetArrowLength(arrow_length);

    need_update_.store(true);
}

/**
 * @brief 重置坐标系的位姿，非渲染线程调用
 *
 * @param Twi 输入的新的坐标系位姿
 */
void CoordinateUI::ResetTwi(const SE3 &Twi) {
    SE3 Twi_x = Twi;
    SE3 Twi_y = Twi_x * z_rot_;
    SE3 Twi_z = Twi_x * y_rot_.inverse();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        x_axis_->ResetTwi(Twi_x);
        y_axis_->ResetTwi(Twi_y);
        z_axis_->ResetTwi(Twi_z);
    }

    Twi_ = Twi;
    need_update_.store(true);
}

/**
 * @brief 更新坐标系
 *
 */
void CoordinateUI::Update() {
    if (need_update_.load()) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            x_axis_->Update();
            y_axis_->Update();
            z_axis_->Update();
        }
        need_update_.store(false);
    }
}

/**
 * @brief 坐标系清理函数
 *
 */
void CoordinateUI::Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    x_axis_->Clear();
    y_axis_->Clear();
    z_axis_->Clear();
}

/**
 * @brief 坐标系渲染函数
 *
 */
void CoordinateUI::Render() {
    if (!IsValid())
        return;

    std::lock_guard<std::mutex> lock(mutex_);
    x_axis_->Render();
    y_axis_->Render();
    z_axis_->Render();
}

}