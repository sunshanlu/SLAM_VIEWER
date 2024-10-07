#pragma once

#include <memory>
#include <vector>

#include "Common.h"
#include "PointTypes.h"

#define rad2deg ((180.0) / (M_PI));
#define deg2rad ((M_PI) / (180.0));

typedef Eigen::Matrix<float, 3, 4> Mat3_4;
typedef Eigen::Quaternionf Quat;
typedef Eigen::Matrix3f Mat3;
typedef pcl::PointCloud<slam_viewer::PointXYZRT> PointCloudXYZRT;

class DataHelper {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef std::shared_ptr<DataHelper> Ptr;
    typedef std::shared_ptr<const DataHelper> ConstPtr;

    /// 数据包
    struct DataBag {
        typedef std::vector<PointCloudXYZRT::Ptr> PointClouds;

        double stamp_;            ///< 时间戳
        PointClouds pointclouds_; ///< 点云数据，分线束存储
        cv::Mat right_image_;     ///< 右侧图像
        cv::Mat left_image_;      ///< 左侧图像
        slam_viewer::SE3 Tll;     ///< 里程计参考位姿
        slam_viewer::SE3 Tcc;     ///< 相机参考位姿
        bool valid_;              ///< 数据是否有效
    };

    DataHelper() = default;

    virtual DataBag LoadNext() = 0;

    virtual ~DataHelper() = default;

private:
};
