#include "TrajectoryUI.h"

NAMESPACE_BEGIN

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

NAMESPACE_END