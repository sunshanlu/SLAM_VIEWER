#pragma once

#include "ArrowUI.h"
#include "Common.h"

NAMESPACE_BEGIN

class CoordinateUI : public UIItem {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    typedef std::shared_ptr<CoordinateUI> Ptr;
    typedef std::shared_ptr<const CoordinateUI> ConstPtr;

    CoordinateUI(float arrow_length, SE3 Twi = SE3());
    
    /// 重置坐标轴变换
    void ResetTwi(const SE3 &Twi) override;
    
    /// 更新
    void Update() override;
    
    /// 清理
    void Clear() override;
    
    /// 判断坐标轴是否有效
    bool IsValid() override {
        bool x_valid = x_axis_->IsValid();
        bool y_valid = y_axis_->IsValid();
        bool z_valid = z_axis_->IsValid();
        return x_valid && y_valid && z_valid;
    }

    /// 渲染
    void Render() override;

private:
    ArrowUI::Ptr x_axis_; ///< x轴
    ArrowUI::Ptr y_axis_; ///< y轴
    ArrowUI::Ptr z_axis_; ///< z轴

    SE3 x_rot_, y_rot_, z_rot_; ///< 沿着轴旋转的旋转矩阵
};

NAMESPACE_END