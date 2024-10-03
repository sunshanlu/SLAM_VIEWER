#include "CoordinateUI.h"
#include "WindowImpl.h"

using namespace slam_viewer;

int main(int argc, char **argv) {
    /// @note 左乘模型是以世界坐标系的某个轴进行旋转，右乘模型是以自身坐标系的某个轴旋转。旋转顺序很重要

    /// 1. 创建一个坐标系UI
    Eigen::AngleAxisf start_r(0.2 * M_PI, Vec3(1.0, 0.0, 0));
    SE3 Twi(start_r.toRotationMatrix(), Vec3(0.0, 0.0, 0.0));
    CoordinateUI::Ptr coordinate_ui = std::make_shared<CoordinateUI>(10, Twi);

    /// 2. 创建一个可视化窗口
    auto viewer = std::make_shared<WindowImpl>();
    auto camera = std::make_shared<Camera>("camera");
    auto view3d = std::make_shared<View3D>("view3d");
    viewer->AddView(view3d, 0, 1, 0, 1);

    view3d->SetCamera(camera);
    view3d->AddUIItem(coordinate_ui);
    
    std::thread viewer_thread(&WindowImpl::Run, viewer);

    /// 3. 测试ResetTwi函数
    Eigen::AngleAxisf delta_r(0.01 * M_PI, Vec3(0.0, 1.0, 0));
    SO3 delta_R(delta_r.toRotationMatrix());
    Vec3 delta_t(0.01, 0, 0);
    for (int i = 0; i < 1000; ++i) {
        Twi.so3() = Twi.so3() * delta_R;
        Twi.translation() += delta_t;
        coordinate_ui->ResetTwi(Twi);
        std::this_thread::sleep_for(10ms);
    }

    viewer_thread.join();

    return 0;
}