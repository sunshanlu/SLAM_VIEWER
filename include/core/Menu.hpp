#pragma once

#include "Common.h"

NAMESPACE_BEGIN

/// 菜单类
class Menu : public View {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef std::shared_ptr<Menu> Ptr;
    typedef std::shared_ptr<const Menu> ConstPtr;

    /// 菜单元素CLASS
    // clang-format off
    template <typename T> 
    struct MenuItem {
        typedef typename std::shared_ptr<MenuItem> Ptr;
        typedef typename std::shared_ptr<const MenuItem> ConstPtr;
        typedef typename std::shared_ptr<pangolin::Var<T>> VarPtr;
        
        VarPtr item_var_;                    ///< 菜单元素变量
        std::function<void(void)> callback_; ///< 元素变量的回调函数
    };
    // clang-format on

    typedef std::vector<MenuItem<bool>::Ptr> BoolItems;
    typedef std::vector<MenuItem<int>::Ptr> IntItems;
    typedef std::vector<MenuItem<float>::Ptr> FloatItems;
    typedef std::vector<MenuItem<double>::Ptr> DoubleItems;
    typedef std::vector<MenuItem<std::string>::Ptr> StrItems;

    Menu(std::string name);

    /// 创建Menu的布局，
    void CreateDisplayLayout(pangolin::Layout layout = pangolin::LayoutEqualVertical) override;

    /// 添加一个按钮菜单元素，在渲染线程启动之前的初始化阶段使用
    void AddButtonItem(std::string name, std::function<void(bool)> callback);

    /// 添加一个checkbox菜单元素
    void AddCheckBoxItem(std::string name, std::function<void(bool)> callback);

    /// 添加一个整数菜单元素
    void AddIntItem(std::string name, std::function<void(int)> callback, int min = 0, int max = 10,
                    int default_val = 5);

    /// 添加一个float菜单元素
    void AddFloatItem(std::string name, std::function<void(float)> callback, float min = 0.f, float max = 10.f,
                      float default_val = 5.f, bool log_scale = false);

    /// 添加一个double菜单元素
    void AddDoubleItem(std::string name, std::function<void(double)> callback, double min = 0.f, double max = 10.f,
                       double default_val = 5.f, bool log_scale = false);

    /// 菜单更新元素
    void Update() {
        for (auto &item : bool_items_)
            item->callback_();

        for (auto &item : float_items_)
            item->callback_();

        for (auto &item : double_items_)
            item->callback_();

        for (auto &item : int_items_)
            item->callback_();
    }

    /// 菜单自动渲染，无需额外的代码
    void Render() override { Update(); }

private:
    /// 创建按钮菜单元素，按下后会自己回弹
    void CreateButtonItem(std::string name, std::function<void(bool)> callback);

    /// 按键按下后，不会自己回弹，callback一直有效
    void CreateCheckBoxItem(std::string name, std::function<void(bool)> callback);

    /// 创建整型菜单元素
    void CreateIntItem(std::string name, std::function<void(int)> callback, int min = 0, int max = 10,
                       int default_val = 5);

    /// 创建float浮点型菜单元素
    void CreateFloatItem(std::string name, std::function<void(float)> callback, float min = 0.f, float max = 10.f,
                         float default_val = 5.f, bool log_scale = false);

    /// 创建double浮点型菜单元素
    void CreateDoubleItem(std::string name, std::function<void(double)> callback, double min = 0.f, double max = 10.f,
                          double default_val = 5.f, bool log_scale = false);

    BoolItems bool_items_;     ///< button 类型菜单元素
    IntItems int_items_;       ///< int 类型菜单元素
    FloatItems float_items_;   ///< float 类型菜单元素
    DoubleItems double_items_; ///< double 类型菜单元素
};

NAMESPACE_END
