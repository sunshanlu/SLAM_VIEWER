#pragma once
#include <cmath>

#include "slam_viewer/core/Common.h"

namespace slam_viewer{

/// @brief 绘制直线箭头
class ArrowUI : public UIItem {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef std::shared_ptr<ArrowUI> Ptr;
    typedef std::shared_ptr<const ArrowUI> ConstPtr;

    /// Arrow的箭头类型
    enum class HeadType {
        FixedHead,     ///< 固定头部长度
        MaxHead,       ///< 最大头部长度，需给定
        FixedHeadRatio ///< 固定头部比例长度
    };

    ArrowUI(SE3 Twi, float arrow_length, Vec3 color, HeadType head_type = HeadType::FixedHead,
            float head_length_ratio = 0.2, float head_max_length = 2.0, float head_fixed_length = 0.5,
            float line_width = 8.0);

    /// 箭头的清除函数，线程安全
    virtual void Clear() override {
        UIItem::Clear();
        {
            std::lock_guard<std::mutex> lock(mutex_);
            points_.clear();
        }
    }

    /// ui元素的更新函数
    virtual void Update() override;

    /// 重置Twi，箭头在世界坐标系下的位姿
    virtual void ResetTwi(const SE3 &Twi) override;

    /// 渲染函数
    virtual void Render() override;

    /// 重置箭头长度，主线程调用
    void ResetArrowLength(float arrow_length);

private:
    /// 计算头部长度
    float ComputeHeadLength(float head_length);

    std::vector<Vec3> points_; ///< 自身坐标系下的4个点

    HeadType head_type_;      ///< 箭头头部类型
    float head_length_ratio_; ///< 箭头头部比例长度
    float head_max_length_;   ///< 箭头头部最大长度
    float head_fixed_length_; ///< 箭头固定头部长度
    float arrow_length_;      ///< 箭头长度
    float tan15_;             ///< 15度正切值
};

}