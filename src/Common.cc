#include "slam_viewer/core/Common.h"

namespace slam_viewer{

/**
 * @brief UIItem的自身坐标系和世界坐标系相同的构造函数
 *
 * @param color         输入的颜色信息
 * @param line_width    输入的线宽
 * @param point_size    输入的点大小
 */
UIItem::UIItem(Vec3 color, float line_width, float point_size)
    : line_width_(line_width)
    , point_size_(point_size)
    , color_(std::move(color))
    , need_update_(true) {}

/**
 * @brief 需要提供UIItem在世界坐标系中的坐标
 *
 * @param color         输入的颜色信息
 * @param line_width    输入的线宽
 * @param point_size    输入的点大小
 * @param Twi           输入的UIItem在世界坐标系中的位姿
 */
UIItem::UIItem(Vec3 color, float line_width, float point_size, SE3 Twi)
    : Twi_(std::move(Twi))
    , line_width_(std::move(line_width))
    , point_size_(std::move(point_size))
    , color_(std::move(color))
    , need_update_(true) {}

/// 清除UIItem的相关内容
void UIItem::Clear() {
    need_update_.store(true);
    std::lock_guard<std::mutex> lock(mutex_);
    vbo_.Free();
}

/**
 * @brief 重置UIItem在世界坐标系下的位姿
 *
 * @param Twi 输入的新的UIItem在世界坐标系下的位姿
 */
void UIItem::ResetTwi(const SE3 &Twi) {
    need_update_.store(true);
    Twi_ = Twi;
}

}