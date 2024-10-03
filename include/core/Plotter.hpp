#pragma once

#include "Common.h"

NAMESPACE_BEGIN

class Plotter : public View {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef std::shared_ptr<Plotter> Ptr;
    typedef std::shared_ptr<const Plotter> ConstPtr;

    /// @brief 绘图元素
    struct PlotterItem {
        typedef std::shared_ptr<pangolin::Plotter> SharedTable; ///< 渲染Log的板
        typedef std::shared_ptr<pangolin::DataLog> LogPtr;

        typedef std::shared_ptr<PlotterItem> Ptr;
        typedef std::shared_ptr<const PlotterItem> ConstPtr;

        std::string name_;  ///< Plotter名称
        LogPtr logger_;     ///< 数据记录器
        int label_nums_;    ///< 标签数量
        SharedTable table_; ///< 渲染板

        //更新logger中的数据
        void Update(const std::vector<float> &data);

        /**
         * @brief 更新的数据
         *
         * @tparam Derived Eigen矩阵的类型
         * @param data 输入的更新的数据
         */
        template <typename Derived> void Update(const Eigen::MatrixBase<Derived> &data) {
            if (!logger_)
                return;
            if (label_nums_ != data.rows() * data.cols())
                return;
            logger_->Log(data);
        }
    };

    Plotter(std::string name)
        : View(std::move(name)) {}

    /// 添加一个绘图元素，在渲染线程启动之前的初始化阶段使用
    void AddPlotterItem(std::string plot_name, std::vector<std::string> labels, float x_min = -10, float x_max = 600,
                        float y_min = -10, float y_max = 10, float x_ticks = 75, float y_ticks = 2);

    /// 其他线程调用，更新绘图数据
    void UpdatePlotterItem(const std::string &plot_name, const std::vector<float> &data);

    /// 其他线程调用，更新绘图数据
    template <typename Derived>
    void UpdatePlotterItem(const std::string &plot_name, const Eigen::MatrixBase<Derived> &data) {
        if (plotter_items_.find(plot_name) == plotter_items_.end())
            return;

        plotter_items_[plot_name]->Update(data);
    }

    /// pangolin::DataLog无需单独渲染
    void Render() override {}

private:
    /// 创建绘图元素
    void CreatePlotterItem(std::string plot_name, std::vector<std::string> labels, float x_min, float x_max,
                           float y_min, float y_max, float x_ticks, float y_ticks);

    std::unordered_map<std::string, PlotterItem::Ptr> plotter_items_; ///< 绘图元素
};

NAMESPACE_END