#pragma once

#include "Common.h"

NAMESPACE_BEGIN

class TrajectoryUI : public UIItem {
    friend class UIItem;

public:
    typedef std::shared_ptr<TrajectoryUI> Ptr;
    typedef std::shared_ptr<const TrajectoryUI> ConstPtr;

    TrajectoryUI(Vec3 color, float line_width, float point_size, std::size_t max_capicity = 1e8)
        : UIItem(color, line_width, point_size)
        , max_capicity_(std::move(max_capicity)) {
        poses_.reserve(max_capicity_);
    }

    TrajectoryUI(const TrajectoryUI &) = delete;

    TrajectoryUI &operator=(const TrajectoryUI &) = delete;

    /// 渲染函数
    void Render() override;

    /// 向轨迹中添加点
    void AddPt(const Vec3 &pt) {
        if (poses_.size() >= max_capicity_)
            poses_.erase(poses_.begin(), poses_.end() + 2e7);

        poses_.push_back(pt);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            vbo_ = pangolin::GlBuffer(pangolin::GlArrayBuffer, poses_);
        }
    }

    /// 清空选项
    void Clear() override {
        poses_.clear();
        poses_.reserve(max_capicity_);

        {
            std::lock_guard<std::mutex> lock(mutex_);
            vbo_.Free();
        }
    }

private:
    std::size_t max_capicity_; ///< 轨迹最大容量
    std::vector<Vec3> poses_;  ///< 轨迹点集
};

NAMESPACE_END