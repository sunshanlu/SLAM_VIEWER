#include "ArrowUI.h"

NAMESPACE_BEGIN

/**
 * @brief ArrowUI的构造函数，非线程安全
 *
 * @param Twi               输入的Arrow在世界坐标系下的位姿
 * @param arrow_length      输入的Arrow的长度
 * @param color             输入的Arrow的颜色
 * @param head_type         输入的Arrow的头部类型
 * @param head_length_ratio 输入的Arrow的头部长度比例
 * @param head_max_length   输入的Arrow的头部最大长度
 * @param head_fixed_length 输入的Arrow的头部固定长度
 * @param line_width        输入的Arrow的线宽
 */
ArrowUI::ArrowUI(SE3 Twi, float arrow_length, Vec3 color, HeadType head_type, float head_length_ratio,
                 float head_max_length, float head_fixed_length, float line_width)
    : UIItem(color, line_width, 2.0, Twi)
    , head_type_(std::move(head_type))
    , head_length_ratio_(std::move(head_length_ratio))
    , head_max_length_(std::move(head_max_length))
    , arrow_length_(std::move(arrow_length))
    , head_fixed_length_(std::move(head_fixed_length)) {

    tan15_ = std::tan(15 * M_PI / 180);
    Vec3 start_point = Twi_ * Vec3(0, 0, 0);
    Vec3 end_point = Twi_ * Vec3(arrow_length, 0, 0);

    float head_length = ComputeHeadLength(head_fixed_length_);
    float y = tan15_ * head_length;
    Vec3 up_point = Twi_ * Vec3(arrow_length - head_length, y, 0);
    Vec3 down_point = Twi_ * Vec3(arrow_length - head_length, -y, 0);

    points_ = {start_point, end_point, up_point, end_point, down_point, end_point};
    need_update_.store(true);
}

/**
 * @brief 计算头部长度，内部使用api
 *
 * @param head_length   输入的未调整的头部长度
 * @return float        输出的调整后的头部长度
 */
float ArrowUI::ComputeHeadLength(float head_length) {

    float ratio_length = arrow_length_ * head_length_ratio_;
    switch (head_type_) {
    case HeadType::FixedHead: ///< 固定头部大小
        head_length = head_fixed_length_;
        break;
    case HeadType::FixedHeadRatio: ///< 固定头部比例
        head_length = arrow_length_ * head_length_ratio_;
        break;
    case HeadType::MaxHead: ///< 固定头部比例，单有最大长度
        head_length = ratio_length <= head_max_length_ ? ratio_length : head_max_length_;
    default:
        break;
    }

    return head_length <= arrow_length_ ? head_length : arrow_length_;
}

/**
 * @brief 当箭头需要更新时，更新箭头数据，need_update_置回
 *
 */
void ArrowUI::Update() {
    if (need_update_.load()) {
        need_update_.store(false);

        std::lock_guard<std::mutex> lock(mutex_);
        vbo_ = pangolin::GlBuffer(pangolin::GlArrayBuffer, points_);
    }
}

/**
 * @brief 重置Twi，箭头在世界坐标系下的位姿，主线程使用，线程安全，不允许多线程同时调用
 *
 * @param Twi 输入的新的Twi
 */
void ArrowUI::ResetTwi(const SE3 &Twi) {
    SE3 Tiw_ = Twi_.inverse();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (int i = 0; i < points_.size(); i++)
            points_[i] = Twi * Tiw_ * points_[i];
    }

    Twi_ = Twi;
    need_update_.store(true);
}

/**
 * @brief 渲染箭头函数，仅渲染线程使用
 *
 */
void ArrowUI::Render() {
    if (!IsValid())
        return;

    glColor3f(color_[0], color_[1], color_[2]);
    glLineWidth(line_width_);
    pangolin::RenderVbo(vbo_, GL_LINES);
    glLineWidth(1.0);
}

/**
 * @brief 重置箭头长度
 *
 * @param arrow_length 输入的新的长度
 */
void ArrowUI::ResetArrowLength(float arrow_length) {
    arrow_length_ = std::move(arrow_length);
    Vec3 start_point = Twi_ * Vec3(0, 0, 0);
    Vec3 end_point = Twi_ * Vec3(arrow_length, 0, 0);

    float head_length = ComputeHeadLength(head_fixed_length_);
    float y = tan15_ * head_length;
    Vec3 up_point = Twi_ * Vec3(arrow_length - head_length, y, 0);
    Vec3 down_point = Twi_ * Vec3(arrow_length - head_length, -y, 0);

    {
        std::lock_guard<std::mutex> lock(mutex_);
        points_ = {start_point, end_point, up_point, end_point, down_point, end_point};
    }

    need_update_.store(true);
}

NAMESPACE_END