#pragma once

#include "Common.h"

namespace slam_viewer{

/// @brief 图像显示器
class ImageShower : public View {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef std::shared_ptr<ImageShower> Ptr;
    typedef std::shared_ptr<const ImageShower> ConstPtr;

    struct Image {
        typedef std::shared_ptr<Image> Ptr;
        typedef std::shared_ptr<const Image> ConstPtr;

        cv::Mat image_;                ///< 待渲染的图像
        std::string content_;          ///< 图像中涵盖的内容
        std::mutex mutex_;             ///< 图像互斥锁
        Vec2 pos_;                     ///< 图像要渲染的位置
        std::atomic_bool img_update_;  ///< image_是否产生了更新
        std::atomic_bool cont_update_; ///< content_是否产生了更新
    };

    ImageShower(std::string name, int row, int col, int row_offset = 20, int col_offset = 20);

    ~ImageShower() { cv::destroyAllWindows(); }

    /// 更新图像
    void UpdateImage(const std::string &name, const std::string &content, cv::Mat image);

    /// 图像渲染前的准备工作，即只能在初始时使用，渲染线程启动后不能使用
    void AddImage(const std::string &name, int row, int col);

    /// 创建布局
    void CreateDisplayLayout(pangolin::Layout layout = pangolin::LayoutEqualVertical);

    /// 渲染函数
    void Render();

private:
    std::unordered_map<std::string, Image::Ptr> images_; ///< 要渲染的图像
    int row_, col_;                                      ///< 图像分布的行数和列数
    int cur_row_, cur_col_;                              ///< 图形当前行和当前列
    int max_u_, max_v_;                                  ///< images_中的图像的最大u和最大v
    cv::Mat render_image_;                               ///< 将所有图像拼接成一个大图
    int row_offset_, col_offset_;                        ///< 行和列之间间隔
};

}