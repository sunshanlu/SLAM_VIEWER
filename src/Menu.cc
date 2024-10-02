#include "Menu.hpp"

NAMESPACE_BEGIN

Menu::Menu(std::string name, float window_ratio)
    : name_(std::move(name))
    , window_ratio_(std::move(window_ratio)) {}

/// 创建Menu的布局，
void Menu::CreateDisplayLayout() {
    pangolin::CreatePanel(name_);

    while (!create_queue_.empty()) {
        auto create_func = create_queue_.front();
        create_queue_.pop();
        create_func();
    }
}

/// 添加一个按钮菜单元素
void Menu::AddButtonItem(std::string name, std::function<void(bool)> callback) {
    auto create_func = [=]() { CreateButtonItem(name, callback); };
    create_queue_.push(create_func);
}

/// 添加一个checkbox菜单元素
void Menu::AddCheckBoxItem(std::string name, std::function<void(bool)> callback) {
    auto create_func = [=]() { CreateCheckBoxItem(name, callback); };
    create_queue_.push(create_func);
}

/// 添加一个整数菜单元素
void Menu::AddIntItem(std::string name, std::function<void(int)> callback, int min, int max, int default_val) {
    auto create_func = [=]() { CreateIntItem(name, callback, min, max, default_val); };
    create_queue_.push(create_func);
}

/// 添加一个float菜单元素
void Menu::AddFloatItem(std::string name, std::function<void(float)> callback, float min, float max, float default_val,
                        bool log_scale) {
    auto create_func = [=]() { CreateFloatItem(name, callback, min, max, default_val, log_scale); };
    create_queue_.push(create_func);
}

/// 添加一个double菜单元素
void Menu::AddDoubleItem(std::string name, std::function<void(double)> callback, double min, double max,
                         double default_val, bool log_scale) {
    auto create_func = [=]() { CreateDoubleItem(name, callback, min, max, default_val, log_scale); };
    create_queue_.push(create_func);
}

/// 创建按钮菜单元素，按下后会自己回弹
void Menu::CreateButtonItem(std::string name, std::function<void(bool)> callback) {
    MenuItem<bool>::Ptr button_item = std::make_shared<MenuItem<bool>>();
    button_item->item_var_ = std::make_shared<pangolin::Var<bool>>(name_ + "." + name, false, false);
    button_item->callback_ = [=]() { callback(pangolin::Pushed(*button_item->item_var_)); };
    bool_items_.push_back(button_item);
}

/// 按键按下后，不会自己回弹，callback一直有效
void Menu::CreateCheckBoxItem(std::string name, std::function<void(bool)> callback) {
    MenuItem<bool>::Ptr check_box_item = std::make_shared<MenuItem<bool>>();
    check_box_item->item_var_ = std::make_shared<pangolin::Var<bool>>(name_ + "." + name, false, true);
    check_box_item->callback_ = [=]() { callback(*check_box_item->item_var_); };
    bool_items_.push_back(check_box_item);
}

/// 创建整型菜单元素
void Menu::CreateIntItem(std::string name, std::function<void(int)> callback, int min, int max, int default_val) {
    MenuItem<int>::Ptr int_item = std::make_shared<MenuItem<int>>();
    int_item->item_var_ = std::make_shared<pangolin::Var<int>>(name_ + "." + name, default_val, min, max);
    int_item->callback_ = [=]() { callback(*int_item->item_var_); };
    int_items_.push_back(int_item);
}

/// 创建float浮点型菜单元素
void Menu::CreateFloatItem(std::string name, std::function<void(float)> callback, float min, float max,
                           float default_val, bool log_scale) {
    MenuItem<float>::Ptr float_item = std::make_shared<MenuItem<float>>();
    float_item->item_var_ =
        std::make_shared<pangolin::Var<float>>(name_ + "." + name, default_val, min, max, log_scale);
    float_item->callback_ = [=]() { callback(*float_item->item_var_); };
    float_items_.push_back(float_item);
}

/// 创建double浮点型菜单元素
void Menu::CreateDoubleItem(std::string name, std::function<void(double)> callback, double min, double max,
                            double default_val, bool log_scale) {
    MenuItem<double>::Ptr double_item = std::make_shared<MenuItem<double>>();
    double_item->item_var_ =
        std::make_shared<pangolin::Var<double>>(name_ + "." + name, default_val, min, max, log_scale);
    double_item->callback_ = [=]() { callback(*double_item->item_var_); };
    double_items_.push_back(double_item);
}

NAMESPACE_END