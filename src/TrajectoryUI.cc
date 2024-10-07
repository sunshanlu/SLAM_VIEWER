#include "TrajectoryUI.h"

namespace slam_viewer{

void TrajectoryUI::Render() {
    if (!IsValid())
        return;

    glColor3f(color_[0], color_[1], color_[2]);

    // 点线形式绘制
    {
        std::lock_guard<std::mutex> lock(mutex_);
        glLineWidth(line_width_);
        pangolin::RenderVbo(vbo_, GL_LINE_STRIP);
        glLineWidth(1.0);

        glPointSize(point_size_);
        pangolin::RenderVbo(vbo_, GL_POINTS);
        glPointSize(1.0);
    }
}

/**
 * @brief TrajectoryUI的构造函数
 *
 * @param color         颜色
 * @param line_width    线宽
 * @param point_size    点大小
 * @param max_capicity  轨迹ui最大容量
 */
TrajectoryUI::TrajectoryUI(Vec3 color, float line_width, float point_size, std::size_t max_capicity)
    : UIItem(color, line_width, point_size)
    , max_capicity_(std::move(max_capicity)) {
    poses_.reserve(max_capicity_);
}

/**
 * @brief 向轨迹UI中添加轨迹点
 *
 * @param pt 输入的轨迹点
 */
void TrajectoryUI::AddPt(const Vec3 &pt) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (poses_.size() >= max_capicity_)
            poses_.erase(poses_.begin(), poses_.end() + 2e7);

        poses_.push_back(pt);
    }
    need_update_.store(true);
}

/**
 * @brief 判断元素是否需要更新，若需要更新，需要将need_update_标识置为false
 *
 */
void TrajectoryUI::Update() {
    if (!need_update_.load())
        return;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        vbo_ = pangolin::GlBuffer(pangolin::GlArrayBuffer, poses_);
    }
    need_update_.store(false);
}

/**
 * @brief 清除轨迹的内容
 *
 */
void TrajectoryUI::Clear() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        poses_.clear();
        poses_.reserve(max_capicity_);
    }

    vbo_.Free();
}

/**
 * @brief 重置轨迹在世界坐标系下的位姿
 *
 * @param Twi   输入的重之后的世界坐标系下的位姿
 */
void TrajectoryUI::ResetTwi(const SE3 &Twi) {
    std::vector<Vec3> poses(poses_.size());
    auto Tiw_ = Twi_.inverse();
    for (int i = 0; i < poses_.size(); ++i)
        poses[i] = Twi * Tiw_ * poses_[i];

    std::lock_guard<std::mutex> lock(mutex_);
    std::swap(poses, poses_);
    Twi_ = Twi;

    need_update_.store(true);
}

}