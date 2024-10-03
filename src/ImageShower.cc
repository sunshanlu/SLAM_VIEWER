#include "ImageShower.h"

NAMESPACE_BEGIN

/**
 * @brief 图像可视化器的构造函数
 *
 * @param name  输入的名字
 * @param row   输入的图像行数
 * @param col   输入的图像列数
 */
ImageShower::ImageShower(std::string name, int row, int col, int row_offset, int col_offset)
    : View(std::move(name))
    , row_(std::move(row))
    , col_(std::move(col))
    , cur_row_(0)
    , cur_col_(0)
    , max_u_(-1)
    , max_v_(-1)
    , row_offset_(std::move(row_offset))
    , col_offset_(std::move(col_offset)) {
    cv::namedWindow(name_, cv::WINDOW_KEEPRATIO | cv::WINDOW_NORMAL);
    cv::resizeWindow(name_, 640, 480);
}

/**
 * @brief 指定图像名称，更新图像和内容
 *
 * @param name      输入的图像名称
 * @param content   输入的内容
 * @param image     输入的图像
 */
void ImageShower::UpdateImage(const std::string &name, const std::string &content, cv::Mat image) {
    if (images_.find(name) == images_.end())
        return;

    if (image.channels() != 3)
        throw std::runtime_error("Image must be 3 channels");

    Image::Ptr image_ptr = images_[name];
    std::lock_guard<std::mutex> lock(image_ptr->mutex_);
    if (!image_ptr->image_.empty() && (image_ptr->image_.cols != image.cols || image_ptr->image_.rows != image.rows))
        throw std::runtime_error("Image size changed");

    image_ptr->image_ = image;
    image_ptr->img_update_ = true;

    if (image_ptr->content_ != content) {
        image_ptr->content_ = content;
        image_ptr->cont_update_ = true;
    }
}

/**
 * @brief 图像渲染前的准备工作，不涉及线程安全问题
 *
 * @details
 *      1. 统计图像的最大行和最大列
 *      2. 分配图像的显示区域
 *
 * @param name  输入的图像名称
 * @param row   输入的图像具有的行像素值
 * @param col   输入的图像具有的列像素值
 */
void ImageShower::AddImage(const std::string &name, int row, int col) {
    if (images_.find(name) != images_.end())
        return;

    if (cur_col_ >= col_ || cur_row_ >= row_)
        return;

    auto image = std::make_shared<Image>();
    image->pos_ = Vec2(cur_col_++, cur_row_);
    image->img_update_ = false;
    images_.insert({name, image});

    if (row > max_v_)
        max_v_ = row;
    if (col > max_u_)
        max_u_ = col;

    if (cur_col_ >= col_) {
        cur_col_ = 0;
        cur_row_++;
    }
}

/**
 * @brief 创建需渲染的组合图像
 *
 * @param layout 针对ImageShower没用
 */
void ImageShower::CreateDisplayLayout(pangolin::Layout layout) {
    auto all_row = (max_v_ + row_offset_) * row_ + row_offset_;
    auto all_col = (max_u_ + col_offset_) * col_ + col_offset_;

    render_image_ = cv::Mat(all_row, all_col, CV_8UC3, cv::Scalar(127, 127, 127));
}

/**
 * @brief 渲染函数
 * @details
 *      1. 判断image是否update，进行更新
 *      2. 判断content是否update，进行更新
 */
void ImageShower::Render() {
    if (max_u_ < 0 || max_v_ < 0)
        return;

    for (const auto &item : images_) {
        auto image = item.second;
        if (image->img_update_) {
            image->img_update_ = false;
            std::lock_guard<std::mutex> lock(image->mutex_);
            int start_x = col_offset_ + image->pos_[0] * (max_u_ + col_offset_);
            int start_y = row_offset_ + image->pos_[1] * (max_v_ + row_offset_);

            int half_diff_u = (max_u_ - image->image_.cols) / 2.0;
            int half_diff_v = (max_v_ - image->image_.rows) / 2.0;

            start_x += half_diff_u;
            start_y += half_diff_v;
            int end_x = start_x + image->image_.cols;
            int end_y = start_y + image->image_.rows;

            image->image_.copyTo(render_image_.rowRange(start_y, end_y).colRange(start_x, end_x));
        }

        if (image->cont_update_) {
            image->img_update_ = false;
            std::vector<std::string> line_content;
            {
                std::lock_guard<std::mutex> lock(image->mutex_);

                auto fist_iter = image->content_.begin();

                /// 断行
                while (fist_iter != image->content_.end()) {
                    auto find_iter = std::find(fist_iter, image->content_.end(), '\n');
                    
                    if (find_iter == image->content_.end()){
                        line_content.emplace_back(fist_iter, image->content_.end());;
                        break;
                    }

                    line_content.emplace_back(fist_iter, find_iter);
                    fist_iter = find_iter + 1;
                }
            }

            int start_x = image->pos_[0] * (max_u_ + col_offset_);
            int start_y = image->pos_[1] * (max_v_ + row_offset_);
            int end_x = start_x + max_u_;
            int end_y = start_y + row_offset_;
            render_image_.rowRange(start_y, end_y).colRange(start_x, end_x) = cv::Scalar(127, 127, 127);

            for (int i = 0; i < line_content.size(); ++i) {
                cv::Size text_size = cv::getTextSize(line_content[i], cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, nullptr);
                cv::putText(render_image_, line_content[i], cv::Point(start_x, start_y + 1.25 * text_size.height),
                            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
                start_y += 1.5 * text_size.height;
            }
        }
    }

    cv::imshow(name_, render_image_);
}

NAMESPACE_END