#include "CloudUI.hpp"

NAMESPACE_BEGIN

std::vector<Vec4, Eigen::aligned_allocator<Vec4>> intensity_table_; ///< 颜色表
bool intensity_table_init_;                                         ///< 颜色表是否初始化

// clang-format off
std::vector<Vec4, Eigen::aligned_allocator<Vec4>> contrast_table_ = {
    {0   / 255.0,   18  / 255.0,    25  / 255.0,    0.5},
    {0   / 255.0,   95  / 255.0,    115 / 255.0,    0.5},
    {10  / 255.0,   147 / 255.0,    150 / 255.0,    0.5},
    {148 / 255.0,   210 / 255.0,    189 / 255.0,    0.5},
    {233 / 255.0,   216 / 255.0,    166 / 255.0,    0.5},
    {238 / 255.0,   155 / 255.0,    0   / 255.0,    0.5},
    {202 / 255.0,   103 / 255.0,    2   / 255.0,    0.5},
    {187 / 255.0,   62  / 255.0,    3   / 255.0,    0.5},
    {173 / 255.0,   32  / 255.0,    18  / 255.0,    0.5},
    {155 / 255.0,   34  / 255.0,    38  / 255.0,    0.5},
};
// clang-format on

/**
 * @brief 创建稠密颜色表
 *
 */
void BuildIntensityTable() {
    intensity_table_.reserve(256 * 6);
    auto make_color = [](int r, int g, int b) -> Vec4 { return Vec4(r / 255.0f, g / 255.0f, b / 255.0f, 0.5f); };
    for (int i = 0; i < 256; i++)
        intensity_table_.emplace_back(make_color(255, i, 0));

    for (int i = 0; i < 256; i++)
        intensity_table_.emplace_back(make_color(255 - i, 0, 255));

    for (int i = 0; i < 256; i++)
        intensity_table_.emplace_back(make_color(0, 255, i));

    for (int i = 0; i < 256; i++)
        intensity_table_.emplace_back(make_color(255, 255 - i, 0));

    for (int i = 0; i < 256; i++)
        intensity_table_.emplace_back(make_color(i, 0, 255));

    for (int i = 0; i < 256; i++)
        intensity_table_.emplace_back(make_color(0, 255, 255 - i));
}

/**
 * @brief float到Vec4的映射函数
 *
 * @param intensity 输入的float数字
 * @return Vec4     输出的Vec4的颜色
 */
Vec4 IntensityToRgbPCL(const float &intensity) {
    int index = int(intensity * 6);
    index = index % intensity_table_.size();
    return intensity_table_[index];
}

/**
 * @brief 点云ui渲染函数
 *
 */
void CloudUI::Render() {
    if (!IsValid())
        return;

    glPointSize(point_size_);
    pangolin::RenderVboCbo(vbo_, cbo_);
    glPointSize(1.0);
}

/**
 * @brief 清除函数
 *
 */
void CloudUI::Clear() {
    vbo_.Free();
    cbo_.Free();
    std::lock_guard<std::mutex> lock(mutex_);
    cloud_xyz_.clear();
    cloud_color_.clear();
}

/**
 * @brief 重置点云在世界坐标系下的位姿Twi
 *
 * @param Twi 输入的重置后的Twi数据
 */
void CloudUI::ResetTwi(const SE3 &Twi) {
    need_update_.store(true);

    std::vector<Vec3> cloud_xyz(cloud_xyz_.size());

    SE3 Tiw_ = Twi_.inverse();
    for (int i = 0; i < cloud_xyz_.size(); ++i) {
        const auto &pw = cloud_xyz_[i];
        cloud_xyz[i] = Twi * Tiw_ * pw;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    std::swap(cloud_xyz_, cloud_xyz);
    Twi_ = Twi;
}

NAMESPACE_END