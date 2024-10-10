#include "slam_viewer/ui/BoxUI.h"
#include "slam_viewer/core/WindowImpl.h"

using namespace slam_viewer;

int main(int argc, char **argv) {
    /// 1. 创建一个Bounding Box
    SE3 Twi;
    BoxUI::Ptr box_ui = std::make_shared<BoxUI>(Twi, 5, 2, 1.5);

    /// 2. 创建一个可视化窗口
    auto viewer = std::make_shared<WindowImpl>();
    auto view3d = std::make_shared<View3D>("3D View");
    auto camera = std::make_shared<Camera>("Camera");
    viewer->AddView(view3d, 0, 1, 0, 1);
    view3d->AddUIItem(box_ui);
    view3d->SetCamera(camera);
    std::thread viewer_thread(&WindowImpl::Run, viewer);

    /// 3. 测试ResetTwi函数
    Eigen::AngleAxisf delta_r(0.01 * M_PI, Vec3(0.0, 0.0, 1.0));
    SO3 delta_R(delta_r.toRotationMatrix());
    Vec3 delta_t(0.01, 0, 0);
    for (int i = 0; i < 1000; ++i) {
        Twi.so3() = Twi.so3() * delta_R;
        Twi.translation() += delta_t;
        box_ui->ResetTwi(Twi);
        std::this_thread::sleep_for(10ms);
    }

    viewer_thread.join();

    return 0;
}