#pragma once

#include <pcl/impl/pcl_base.hpp>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/point_traits.h>
#include <pcl/register_point_struct.h>

#include "slam_viewer/core/Common.h"

namespace slam_viewer{

typedef pcl::PointXYZ PointXYZ;
typedef pcl::PointXYZI PointXYZI;
typedef pcl::PointXYZRGB PointXYZRGB;

/// 带有线束信息的点类型
struct PointXYZR {
    PCL_ADD_POINT4D

    std::uint8_t ring = 0;

    PointXYZR() {}

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
} EIGEN_ALIGN16;

/// 带有时间戳偏置信息的点类型
struct PointXYZT {
    PCL_ADD_POINT4D

    double offset_time = 0;

    PointXYZT() {}

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
} EIGEN_ALIGN16;


/// 带有线束和时间戳偏置信息的点类型
struct PointXYZRT {
    PCL_ADD_POINT4D

    uint8_t ring = 0;

    double offset_time = 0;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
} EIGEN_ALIGN16;

}

// clang-format off
POINT_CLOUD_REGISTER_POINT_STRUCT(slam_viewer::PointXYZR,
                                  (float, x, x)
                                  (float, y, y)
                                  (float, z, z)
                                  (std::uint8_t, ring, ring))

POINT_CLOUD_REGISTER_POINT_STRUCT(slam_viewer::PointXYZT,
                                  (float, x, x)
                                  (float, y, y)
                                  (float, z, z)
                                  (double, offset_time, offset_time))

POINT_CLOUD_REGISTER_POINT_STRUCT(slam_viewer::PointXYZRT,
                                  (float, x, x)
                                  (float, y, y)
                                  (float, z, z)
                                  (std::uint8_t, ring, ring)
                                  (double, offset_time, offset_time))
// clang-format on