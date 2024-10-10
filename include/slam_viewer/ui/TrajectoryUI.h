#pragma once

#include "slam_viewer/core/Common.h"

namespace slam_viewer{

class TrajectoryUI : public UIItem {
    friend class UIItem;

public:
    typedef std::shared_ptr<TrajectoryUI> Ptr;
    typedef std::shared_ptr<const TrajectoryUI> ConstPtr;

    TrajectoryUI(Vec3 color = Vec3(1.0, 0.0, 0.0), float line_width = 3.0, float point_size = 5.0,
                 std::size_t max_capicity = 1e8);

    TrajectoryUI(const TrajectoryUI &) = delete;

    TrajectoryUI &operator=(const TrajectoryUI &) = delete;

    /// 渲染函数
    void Render() override;

    /// 向轨迹中添加点，非渲染线程调用
    void AddPt(const Vec3 &pt);

    /// 相轨迹中添加位姿，非渲染线程调用
    void AddPt(const SE3 &Twi) { AddPt(Twi.translation()); }

    /// 更新函数
    void Update() override;

    /// 清空选项
    void Clear() override;

    /// 重置Twi位姿，非渲染线程调用
    void ResetTwi(const SE3 &Twi) override;

private:
    std::size_t max_capicity_; ///< 轨迹最大容量
    std::vector<Vec3> poses_;  ///< 轨迹点集
};

}