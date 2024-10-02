#pragma once
#include <execution>

#include <pcl/point_types.h>

#include "Common.h"

NAMESPACE_BEGIN

extern std::vector<Vec4, Eigen::aligned_allocator<Vec4>> intensity_table_; ///< 颜色表
extern std::vector<Vec4, Eigen::aligned_allocator<Vec4>> contrast_table_;  ///< 对比色表
extern bool intensity_table_init_;                                         ///< 颜色表是否初始化

/// 创建稠密颜色表
void BuildIntensityTable();

Vec4 IntensityToRgbPCL(const float &intensity);

/// 颜色位置工厂模式基类
// clang-format off
template <typename PointType> 
class ColorFactory {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef typename pcl::PointCloud<PointType>::Ptr PointCloudPtr;
    typedef std::shared_ptr<ColorFactory> Ptr;
    typedef std::shared_ptr<const ColorFactory> ConstPtr;

    ColorFactory(PointCloudPtr cloud)
        : cloud_(std::move(cloud)) {
        if (!intensity_table_init_) {
            BuildIntensityTable();
            intensity_table_init_ = true;
        }
    }

    virtual void CreateColor(const std::vector<Vec3> &cloud_xyz, std::vector<Vec4> &cloud_color) = 0;

    virtual ~ColorFactory() = default;

protected:
    PointCloudPtr cloud_; ///< 点云信息
};

/// PCL颜色工厂
template <typename PointType> 
class PCLColor : public ColorFactory<PointType> {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef typename pcl::PointCloud<PointType>::Ptr PointCloudPtr;
    typedef std::shared_ptr<PCLColor> Ptr;
    typedef std::shared_ptr<const PCLColor> ConstPtr;

    PCLColor(PointCloudPtr cloud)
        : ColorFactory<PointType>(cloud) {}

    void CreateColor(const std::vector<Vec3> &cloud_xyz, std::vector<Vec4> &cloud_color) override {
        cloud_color.resize(this->cloud_->size());

        std::vector<int> idx(this->cloud_->size());
        std::iota(idx.begin(), idx.end(), 0);

        std::for_each(std::execution::par_unseq, idx.begin(), idx.end(),
                      [&](const int &id) { cloud_color[id] = IntensityToRgbPCL(this->cloud_->points[id].intensity); });
    }
};

/// intensity颜色工厂
template <typename PointType> 
class IntensityColor : public ColorFactory<PointType> {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef typename pcl::PointCloud<PointType>::Ptr PointCloudPtr;
    typedef std::shared_ptr<IntensityColor> Ptr;
    typedef std::shared_ptr<const IntensityColor> ConstPtr;

    IntensityColor(PointCloudPtr cloud)
        : ColorFactory<PointType>(cloud) {}

    void CreateColor(const std::vector<Vec3> &cloud_xyz, std::vector<Vec4> &cloud_color) override {
        cloud_color.resize(this->cloud_->size());

        std::vector<int> idx(this->cloud_->size());
        std::iota(idx.begin(), idx.end(), 0);

        std::for_each(std::execution::par_unseq, idx.begin(), idx.end(), [&](const int &id) {
            const auto &pt = this->cloud_->points[id];
            float intensity_color = pt.intensity / 255.0 * 3.0;
            cloud_color[id] = Vec4(intensity_color, intensity_color, intensity_color, 0.2);
        });
    }
};

/// height颜色工厂
template <typename PointType> 
class HeightColor : public ColorFactory<PointType> {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef typename pcl::PointCloud<PointType>::Ptr PointCloudPtr;
    typedef std::shared_ptr<HeightColor> Ptr;
    typedef std::shared_ptr<const HeightColor> ConstPtr;

    HeightColor(PointCloudPtr cloud)
        : ColorFactory<PointType>(cloud) {}

    void CreateColor(const std::vector<Vec3> &cloud_xyz, std::vector<Vec4> &cloud_color) override {
        cloud_color.resize(this->cloud_->size());

        std::vector<int> idx(this->cloud_->size());
        std::iota(idx.begin(), idx.end(), 0);

        std::for_each(std::execution::par_unseq, idx.begin(), idx.end(), [&](const int &id) {
            const auto &pt = this->cloud_->points[id];
            cloud_color[id] = IntensityToRgbPCL(pt.z * 10);
        });
    }
};

/// Gray颜色工厂
template <typename PointType> 
class GrayColor : public ColorFactory<PointType> {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef typename pcl::PointCloud<PointType>::Ptr PointCloudPtr;
    typedef std::shared_ptr<GrayColor> Ptr;
    typedef std::shared_ptr<const GrayColor> ConstPtr;
    

    GrayColor(PointCloudPtr cloud)
        : ColorFactory<PointType>(cloud) {}

    void CreateColor(const std::vector<Vec3> &cloud_xyz, std::vector<Vec4> &cloud_color) override {
        cloud_color.resize(this->cloud_->size());

        std::vector<int> idx(this->cloud_->size());
        std::iota(idx.begin(), idx.end(), 0);

        std::for_each(std::execution::par_unseq, idx.begin(), idx.end(), [&](const int &id) {
            const auto &pt = this->cloud_->points[id];
            cloud_color[id] = Vec4(0.5, 0.5, 0.5, 0.5);
        });
    }
};


/// ring颜色工厂(以线束信息作为颜色指标)
template <typename PointType> 
class RingColor : public ColorFactory<PointType> {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef typename pcl::PointCloud<PointType>::Ptr PointCloudPtr;
    typedef std::shared_ptr<RingColor> Ptr;
    typedef std::shared_ptr<const RingColor> ConstPtr;
    

    RingColor(PointCloudPtr cloud)
        : ColorFactory<PointType>(cloud) {}

    void CreateColor(const std::vector<Vec3> &cloud_xyz, std::vector<Vec4> &cloud_color) override {
        cloud_color.resize(this->cloud_->size());

        std::vector<int> idx(this->cloud_->size());
        std::iota(idx.begin(), idx.end(), 0);

        std::for_each(std::execution::par_unseq, idx.begin(), idx.end(), [&](const int &id) {
            const auto &ring = this->cloud_->points[id].ring;
            
            cloud_color[id] = contrast_table_[ring % 10];
        });
    }
};

/// 自身颜色工厂，使用自身的颜色
template <typename PointType> 
class SelfColor : public ColorFactory<PointType> {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef typename pcl::PointCloud<PointType>::Ptr PointCloudPtr;
    typedef std::shared_ptr<SelfColor> Ptr;
    typedef std::shared_ptr<const SelfColor> ConstPtr;
    

    SelfColor(PointCloudPtr cloud)
        : ColorFactory<PointType>(cloud) {}

    void CreateColor(const std::vector<Vec3> &cloud_xyz, std::vector<Vec4> &cloud_color) override {
        cloud_color.resize(this->cloud_->size());

        std::vector<int> idx(this->cloud_->size());
        std::iota(idx.begin(), idx.end(), 0);

        std::for_each(std::execution::par_unseq, idx.begin(), idx.end(), [&](const int &id) {
            const auto &point = this->cloud_->points[id];
            Vec4 color = {point.r, point.g, point.b, 0.5};
            cloud_color[id] = color;
        });
    }
};

// clang-format on

/// 点云UI
class CloudUI : public UIItem {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    typedef std::shared_ptr<CloudUI> Ptr;
    typedef std::shared_ptr<const CloudUI> ConstPtr;

    CloudUI(Vec3 color = Vec3(0.5, 0.5, 0.5), float line_width = 3.0, float point_size = 1.0)
        : UIItem(color, line_width, point_size) {}

    /// 设置点云信息，位置和颜色
    template <typename PointType>
    void SetCloud(typename pcl::PointCloud<PointType>::Ptr &cloud, SE3 Twi,
                  typename ColorFactory<PointType>::Ptr color_factory) {
        if (!cloud || cloud->empty())
            return;

        need_update_.store(true);
        cloud_xyz_.clear();
        cloud_color_.clear();

        cloud_xyz_.resize(cloud->size());
        std::vector<int> idx(cloud->size());
        std::iota(idx.begin(), idx.end(), 0); ///< iota从0开始递增

        std::lock_guard<std::mutex> lock(mutex_);
        Twi_ = std::move(Twi);
        std::for_each(std::execution::par_unseq, idx.begin(), idx.end(), [&](const int &id) {
            const auto &pt = cloud->points[id];
            Vec3 pt_world = Twi_ * pt.getVector3fMap();
            cloud_xyz_[id] = pt_world;
        });

        color_factory->CreateColor(cloud_xyz_, cloud_color_);
    }

    /// 更新点云的坐标
    void ResetTwi(const SE3 &Twi) override;

    /// 更新渲染函数
    void Update() override {
        if (need_update_.load()) {
            need_update_.store(false);

            std::lock_guard<std::mutex> lock(mutex_);
            vbo_ = pangolin::GlBuffer(pangolin::GlArrayBuffer, cloud_xyz_);
            cbo_ = pangolin::GlBuffer(pangolin::GlArrayBuffer, cloud_color_);
        }
    }

    /// CloudUI是否有效
    bool IsValid() override {
        bool vbo_valid = vbo_.IsValid();
        bool cbo_valid = cbo_.IsValid();
        return vbo_valid && cbo_valid;
    }

    /// 点云UI渲染函数
    void Render() override;

    /// 点云UI清除函数
    void Clear() override;

private:
    pangolin::GlBuffer cbo_;        ///< 显存颜色信息
    std::vector<Vec3> cloud_xyz_;   ///< 点云位置信息
    std::vector<Vec4> cloud_color_; ///< 点云颜色信息
};

NAMESPACE_END