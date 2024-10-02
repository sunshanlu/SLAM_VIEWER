#include "FrameUI.h"

NAMESPACE_BEGIN

FrameUI::FrameUI(SE3 Twi, Vec3 color, float line_width, float width, float height, float depth)
    : UIItem(color, line_width, 1.0, Twi) {
    Vec3 cp = Vec3(0, 0, 0);
    Vec3 lu = Vec3(-width / 2.0, height / 2.0, depth);
    Vec3 ld = Vec3(-width / 2.0, -height / 2.0, depth);
    Vec3 ru = Vec3(width / 2.0, height / 2.0, depth);
    Vec3 rd = Vec3(width / 2.0, -height / 2.0, depth);

    origin_points_ = {cp, lu, cp, ld, cp, ru, cp, rd, lu, ru, ru, rd, rd, ld, ld, lu, ld, ru, lu, rd};
    points_.resize(origin_points_.size());
    for (int i = 0; i < origin_points_.size(); ++i)
        points_[i] = Twi_ * origin_points_[i];
}

/// 渲染函数
void FrameUI::Render() {
    if (IsValid()) {
        std::unique_lock<std::mutex> lock(mutex_);

        glLineWidth(line_width_);
        glColor3f(color_(0), color_(1), color_(2));

        pangolin::RenderVbo(vbo_, GL_LINES);
        glLineWidth(1.0);
    }
}

/// 清理函数
void FrameUI::Clear() {
    points_.clear();
    origin_points_.clear();
    UIItem::Clear();
}

/// ui元素的更新函数
void FrameUI::Update() {
    if (need_update_.load()) {
        need_update_.store(false);

        std::lock_guard<std::mutex> lock(mutex_);
        vbo_ = pangolin::GlBuffer(pangolin::GlArrayBuffer, points_);
    }
}

/// 重置frameui的世界坐标
void FrameUI::ResetTwi(const SE3 &Twi) {
    SE3 Tiw_ = Twi_.inverse();

    for (int i = 0; i < points_.size(); i++)
        points_[i] = Twi * origin_points_[i];

    Twi_ = Twi;
    need_update_.store(true);
}

NAMESPACE_END
