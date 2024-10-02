#pragma once

#include "Common.h"

NAMESPACE_BEGIN

class FrameUI : public UIItem {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    typedef std::shared_ptr<FrameUI> Ptr;
    typedef std::shared_ptr<const FrameUI> ConstPtr;

    FrameUI(SE3 Twi, Vec3 color, float line_width, float width = 3.0, float height = 2.0, float depth = 2.0);

    /// 渲染函数
    virtual void Render() override;

    /// 清理函数
    virtual void Clear() override;

    /// ui元素的更新函数
    virtual void Update() override;

    /// 重置frameui的世界坐标
    virtual void ResetTwi(const SE3 &Twi);

private:
    std::vector<Vec3> points_;        ///< 世界坐标系下的坐标点
    std::vector<Vec3> origin_points_; ///< 自身坐标系下的坐标点
};

NAMESPACE_END