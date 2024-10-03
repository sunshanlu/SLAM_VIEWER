#include "Plotter.hpp"

NAMESPACE_BEGIN

/// 添加一个绘图元素
void Plotter::AddPlotterItem(std::string plot_name, std::vector<std::string> labels, float x_min, float x_max,
                             float y_min, float y_max, float x_ticks, float y_ticks) {
    auto create_task = [=]() { CreatePlotterItem(plot_name, labels, x_min, x_max, y_min, y_max, x_ticks, y_ticks); };
    tasks_queue_.push(create_task);
}

/// 其他线程调用，更新绘图数据
void Plotter::UpdatePlotterItem(const std::string &plot_name, const std::vector<float> &data) {
    if (plotter_items_.find(plot_name) == plotter_items_.end())
        return;

    plotter_items_[plot_name]->Update(data);
}

/// 创建绘图元素
void Plotter::CreatePlotterItem(std::string plot_name, std::vector<std::string> labels, float x_min, float x_max,
                                float y_min, float y_max, float x_ticks, float y_ticks) {
    auto &plotter_display = pangolin::Display(name_);

    auto log = std::make_shared<pangolin::DataLog>();
    log->SetLabels(labels);

    auto table = std::make_shared<pangolin::Plotter>(log.get(), x_min, x_max, y_min, y_max, x_ticks, y_ticks);
    table->SetBounds(0.02, 0.98, 0.0, 1.0);
    table->Track("$i");
    table->SetBackgroundColour(pangolin::Colour(248. / 255, 248. / 255, 255. / 255));

    auto plotter_item = std::make_shared<PlotterItem>();
    plotter_item->name_ = plot_name;
    plotter_item->logger_ = log;
    plotter_item->label_nums_ = labels.size();
    plotter_item->table_ = table;
    plotter_items_.insert({plot_name, plotter_item});

    plotter_display.AddDisplay(*plotter_item->table_);
}

/**
 * @brief 更新logger中的数据
 *
 * @param data 输入更新的数据
 */
void Plotter::PlotterItem::Update(const std::vector<float> &data) {
    if (!logger_)
        return;
    if (label_nums_ != data.size())
        return;
    logger_->Log(data);
}

NAMESPACE_END