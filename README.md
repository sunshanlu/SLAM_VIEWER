# slam_viewer

![C++](https://img.shields.io/badge/c++-17std-blue)
![Ubuntu](https://img.shields.io/badge/platform-ubuntu20.04-orange)
![build](https://img.shields.io/badge/build-pass-green)

# 1.介绍

<div align="center">
	<a href="https://www.bilibili.com/video/BV1WN2EYLEJW/" title="bilibili:SLAM_VIEWER">
		<img src="https://raw.githubusercontent.com/sunshanlu/slam_viewer/refs/heads/devel/SLAM_Viewer.jpg" alt="SLAM_Viewer" width=600>
	</a>
</div>

基于pangolin，提供常规的slam可视化api，具体的使用示例可参考examples文件夹下的相关内容。使用slam_viewer非常方便，代码涉及到的流程主要有：

1. `WindowImpl`创建；
2. `View`创建（`Menu`菜单、`Plotter`数据绘图、3d可视化`View3D`和`ImageShower`四种）；
3. `Camera`创建并绑定`View3D`；
4. `WindowImpl`绑定`View`（涉及到各部分`View`的窗口占比）；
5. 动态创建`UIItem`并绑定到`View3D`中。
6. 注意，针对`View`创建部分，在需要在可视化线程运行之前完成，针对`View3D`中渲染的`UIItem`可以动态的向`View3D`中添加

在使用样例中有针对`KITTI-Odometry`数据集的可视化操作，涉及到的渲染`UI`主要有 `Trajectory`（轨迹）、`Frame`（视觉`SLAM`帧）、`Coordinate`（坐标系）、`Cloud`（点云）、`Image`（图像）。除此之外，还有`Menu`菜单的配置，主要针对相机的操作，提供了跟踪模式，上帝视角和前置模式三部分的示例。

除此之外，`slam_viewer`为拓展预留了一些接口，如：
1. `CloudUI`部分的颜色工厂，用户可以方便的自定义一些可视化的颜色；
2. `UIItem`的3d渲染基类，用户可以自定义一些3d渲染的UIItem；
3. `Menu`菜单部分的配置，可以通过`Menu`内置`api`和`lambda`表达式的方式快速配置。

# 2.依赖
1. C++17
2. [Pangolin 0.9.1](https://github.com/stevenlovegrove/Pangolin/releases/tag/v0.9.1)
3. [Sophus 1.22.10](https://github.com/strasdat/Sophus/releases/tag/1.22.10)
4. Eigen3
5. PCL
6. TBB
7. OpenCV

针对`Pangolin`和`Sophus`需要去指定的`github`仓库下载源码进行源码编译和安装。而`Eigen3`、`PCL`和`TBB`则直接通过`apt`进行安装。
```shell
sudo apt install libeigen3-dev libpcl-dev libtbb-dev libopencv-dev
```

# 3. 编译安装

```shell
git clone https://github.com/sunshanlu/slam_viewer.git
cd slam_viewer
```

## 3.1 编译示例程序
```shell
cmake .. -DBUILD_EXAMPLES=ON
make -j8
```

## 3.2 不编译示例程序
```shell
cmake .. -DBUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=Release
make -j8
```

## 3.3 `slam_viewer`安装
```shell
sudo make install
```

# 4. 使用示例
具体的使用示例请看examples文件夹，若要运行`kitti_dataset_example`示例，需要下载`kitti-odometry`数据集，并且需要将数据集按照如下目录存放
```shell
.
├── results
│   ├── 00.txt
│   ├── 01.txt
│   ├── xx.txt
├── sequences
│   ├── 00
│   │   ├── image_0
│   │   │   │   ├── 000000.png
│   │   │   │   ├── 000001.png  
│   │   │   │   ├── xxx.png  
│   │   └── image_1
│   │   │   │   ├── 000000.png
│   │   │   │   ├── 000001.png  
│   │   │   │   ├── xxx.png  
│   │   └── times.txt
│   ├── ...
└── velodyne
    └── sequences
        ├── 00
        │   └── velodyne
        │       ├── 000000.bin
        │       ├── 000001.bin
        │       ├── xxx.bin
        ├── ...
```

运行`kitti_dataset_example`的命令如下：
```shell
./bin/kitti_dataset_example <dataset_path> <sequence>
```
1. `data_path`: 数据集路径，也就是上述目录中的.对应的绝对路径
2. `seqcuece`: 00/01/...，代表kitti数据集序列

# 5.`cmake`链接`slam_viewer`示例
```cmake
find_package(slam_viewer REQUIRED)

target_link_libraries(<your-target> slam_viewer::slam_viewer)
```