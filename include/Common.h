#pragma once

#include <atomic>
#include <deque>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include <Eigen/Core>
#include <flann/flann.hpp>
#include <opencv2/opencv.hpp>
#include <pangolin/pangolin.h>
#include <pcl/point_cloud.h>
#include <sophus/se3.hpp>

using namespace std::chrono_literals;

#define NAMESPACE_BEGIN namespace slam_viewer {

#define NAMESPACE_END }

NAMESPACE_BEGIN

typedef Eigen::Vector3f Vec3;
typedef Eigen::Vector4f Vec4;
typedef Sophus::SE3f SE3;
typedef Sophus::SO3f SO3;

class UIItem {
public:
    typedef std::shared_ptr<UIItem> Ptr;
    typedef std::shared_ptr<const UIItem> ConstPtr;

    UIItem(Vec3 color, float line_width, float point_size)
        : line_width_(line_width)
        , point_size_(point_size)
        , color_(std::move(color))
        , need_update_(false) {}

    /// 渲染函数
    virtual void Render() = 0;

    virtual bool IsValid() { return vbo_.IsValid(); }

    virtual void Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        vbo_.Free();
    }

    /// ui元素的更新函数
    virtual void Update() {}

    virtual ~UIItem() { this->Clear(); };

    /// 重置item的世界坐标
    virtual void ResetTwi(const SE3 &Twi) {
        need_update_.store(true);
        
        std::lock_guard<std::mutex> lock(mutex_);
        Twi_ = Twi;
    }

protected:
    pangolin::GlBuffer vbo_;        ///< 显存顶点信息
    std::mutex mutex_;              ///< 更新UI状态的互斥量，保证线程安全
    SE3 Twi_;                       ///< Item在世界坐标下的位置
    float line_width_;              ///< 涉及到的线宽
    float point_size_;              ///< 涉及到的点大小
    Vec3 color_;                    ///< 颜色
    std::atomic<bool> need_update_; ///< 是否需要更新
};

NAMESPACE_END
