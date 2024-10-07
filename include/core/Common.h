#pragma once

#include <atomic>
#include <deque>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <flann/flann.hpp>
#include <opencv2/opencv.hpp>
#include <pangolin/pangolin.h>
#include <pcl/point_cloud.h>
#include <sophus/se3.hpp>

using namespace std::chrono_literals;

namespace slam_viewer {

typedef Eigen::Vector3f Vec3;
typedef Eigen::Vector4f Vec4;
typedef Eigen::Vector2f Vec2;
typedef Sophus::SE3f SE3;
typedef Sophus::SO3f SO3;

class UIItem {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef std::shared_ptr<UIItem> Ptr;
    typedef std::shared_ptr<const UIItem> ConstPtr;

    UIItem(Vec3 color, float line_width, float point_size);

    UIItem(Vec3 color, float line_width, float point_size, SE3 Twi);

    /// 渲染函数
    virtual void Render() = 0;

    /// 是否合法，渲染之前进行验证
    virtual bool IsValid() { return vbo_.IsValid(); }

    /// 清理函数
    virtual void Clear();

    /// ui元素的更新函数
    virtual void Update() {}

    /// 重置item的世界坐标
    virtual void ResetTwi(const SE3 &Twi);

    /// 获取item的世界位姿
    SE3 GetTwi() const { return Twi_; }

    virtual ~UIItem() { this->Clear(); };

protected:
    pangolin::GlBuffer vbo_;        ///< 显存顶点信息
    std::mutex mutex_;              ///< 更新UI状态的互斥量，保证线程安全
    SE3 Twi_;                       ///< Item在世界坐标下的位置
    float line_width_;              ///< 涉及到的线宽
    float point_size_;              ///< 涉及到的点大小
    Vec3 color_;                    ///< 颜色
    std::atomic<bool> need_update_; ///< 是否需要更新
};

/// 可视基类，一个窗口内有很多View
class View {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef std::shared_ptr<View> Ptr;
    typedef std::shared_ptr<const View> ConstPtr;
    typedef std::queue<std::function<void(void)>> TaskQueue;

    View(std::string name)
        : name_(name) {}

    /// 设置View边界，用于布局使用
    void SetBounds(pangolin::Attach bottom, pangolin::Attach top, pangolin::Attach left, pangolin::Attach right) {
        auto &view = pangolin::Display(name_);
        view.SetBounds(bottom, top, left, right);
    }

    /// 创建View布局
    virtual void CreateDisplayLayout(pangolin::Layout layout = pangolin::LayoutEqualVertical) {
        auto &view = pangolin::Display(name_);
        view.SetLayout(layout);

        while (!tasks_queue_.empty()) {
            auto task = tasks_queue_.front();
            tasks_queue_.pop();
            task();
        }
    }

    /// View的渲染函数，需要再Update之后调用
    virtual void Render() = 0;

protected:
    std::string name_;      ///< View名称，用于找到pangolin对应的渲染对象
    TaskQueue tasks_queue_; ///< 任务队列
};

} // namespace slam_viewer
