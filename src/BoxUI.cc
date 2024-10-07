#include "BoxUI.h"

namespace slam_viewer{

/**
 * @brief Bounding Box3d 的构造函数
 *
 * @param Twi       输入的3d框的世界坐标系位姿
 * @param x_length  输入的3d框的自身坐标系下x方向长度
 * @param y_length  输入的3d框的自身坐标系下y方向长度
 * @param z_length  输入的3d框的自身坐标系下z方向长度
 * @param color     输入的3d框的颜色，默认绿
 */
BoxUI::BoxUI(SE3 Twi, float x_length, float y_length, float z_length, Vec3 color)
    : UIItem(color, 3.0, 2.0, Twi) {
    Vec3 p0(x_length / 2, y_length / 2, z_length / 2);
    Vec3 p1(x_length / 2, -y_length / 2, z_length / 2);
    Vec3 p2(-x_length / 2, -y_length / 2, z_length / 2);
    Vec3 p3(-x_length / 2, y_length / 2, z_length / 2);
    
    Vec3 p4(x_length / 2, y_length / 2, -z_length / 2);
    Vec3 p5(x_length / 2, -y_length / 2, -z_length / 2);
    Vec3 p6(-x_length / 2, -y_length / 2, -z_length / 2);
    Vec3 p7(-x_length / 2, y_length / 2, -z_length / 2);
    origin_points_ = {p0, p1, p1, p2, p2, p3, p3, p0, p4, p5, p5, p6, p6, p7, p7, p4, p0, p4, p1, p5, p2, p6, p3, p7};

    points_.resize(origin_points_.size());
    for (int i = 0; i < origin_points_.size(); ++i)
        points_[i] = Twi_ * origin_points_[i];
}

/// 渲染函数
void BoxUI::Render() {
    if (!IsValid())
        return;

    glColor3f(color_[0], color_[1], color_[2]);
    glLineWidth(line_width_);
    pangolin::RenderVbo(vbo_, GL_LINES);
    glLineWidth(1.0);
}

/// 清理函数
void BoxUI::Clear() {
    UIItem::Clear();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        points_.clear();
    }
    origin_points_.clear();
}

/// ui元素的更新函数
void BoxUI::Update() {
    if (need_update_.load()) {
        need_update_.store(false);

        std::lock_guard<std::mutex> lock(mutex_);
        vbo_ = pangolin::GlBuffer(pangolin::GlArrayBuffer, points_);
    }
}

/// 重置item的世界坐标
void BoxUI::ResetTwi(const SE3 &Twi) {
    SE3 Tiw_ = Twi_.inverse();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (int i = 0; i < origin_points_.size(); ++i)
            points_[i] = Twi_ * origin_points_[i];
    }
    Twi_ = Twi;
    need_update_.store(true);
}

}