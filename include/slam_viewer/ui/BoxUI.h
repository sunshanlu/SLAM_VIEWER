#pragma once

#include "slam_viewer/core/Common.h"

namespace slam_viewer {

class BoxUI : public UIItem {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef std::shared_ptr<BoxUI> Ptr;
    typedef std::shared_ptr<const BoxUI> ConstPtr;

    BoxUI(SE3 Twi, float x_length, float y_length, float z_length, Vec3 color = Vec3(0, 1, 0));

    /// 渲染函数
    void Render() override;

    /// 清理函数
    void Clear() override;

    /// ui元素的更新函数
    void Update() override;

    /// 重置item的世界坐标
    void ResetTwi(const SE3 &Twi) override;

private:
    std::vector<Vec3> points_;        ///< 世界坐标系下的点
    std::vector<Vec3> origin_points_; ///< 自身坐标系下的点
};

} // namespace slam_viewer
