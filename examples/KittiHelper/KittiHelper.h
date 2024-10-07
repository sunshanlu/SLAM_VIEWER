#pragma once

#include <string>

#include "DataHelper.h"

/// @brief kitti数据读取器
class KittiHelper : public DataHelper {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    typedef std::shared_ptr<KittiHelper> Ptr;

    /// kitti的配置项
    struct Options {
        std::string kitti_path; ///< kitti数据路径
        std::string sequence;   ///< 数据序列
        slam_viewer::SE3 Tlc;   ///< 相机在雷达系下的位姿
    };

    /// 读取下一帧数据
    DataBag LoadNext() override;

    KittiHelper(Options options)
        : options_(std::move(options))
        , frame_id_(0) {
        velo_path_ = options_.kitti_path + "/velodyne/sequences/" + options_.sequence + "/velodyne/";
        std::string temp_path = options_.kitti_path + "/sequences/" + options_.sequence + "/";
        left_img_path_ = temp_path + "image_0/";
        right_img_path_ = temp_path + "image_1/";
        stamp_path_ = temp_path + "times.txt";
        gt_path_ = options_.kitti_path + "/results/" + options_.sequence + ".txt";
    }

private:
    DataBag::PointClouds LoadPointCloud();                     ///< 加载点云数据
    void LoadImage(cv::Mat &left_image, cv::Mat &right_image); ///< 加载图像数据

    ///< 加载ground truth
    void LoadGTruth(const std::string &pose_str, slam_viewer::SE3 &Tll, slam_viewer::SE3 &Tcc);

    Options options_;    ///< kitti的配置项
    int frame_id_;       ///< 当前要读取的id信息
    std::string str_id_; ///< 字符串的id信息（带有零填充的）

    std::string velo_path_;      ///< velodyne数据路径
    std::string left_img_path_;  ///< kitti的图像数据路径
    std::string right_img_path_; ///< kitti的图像数据路径
    std::string stamp_path_;     ///< kitti的时间戳路径
    std::string gt_path_;        ///< kitti的ground truth路径
};
