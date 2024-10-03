#include <opencv2/opencv.hpp>

int main(int argc, char **argv) {
    std::string path = "/media/rookie-lu/新加卷/Dataset/KITTI/sequences/00/image_0/000000.png";

    cv::Mat image = cv::imread(path, cv::IMREAD_COLOR);

    std::string content = path + "\n" + "haha next line!";
    cv::Size text_size = cv::getTextSize(content, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, nullptr);
    cv::putText(image, content, cv::Point(0, text_size.height), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                cv::Scalar(255, 255, 255), 1, 8);

    cv::imshow("image", image);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}