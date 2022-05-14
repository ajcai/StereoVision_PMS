#include <bits/stdc++.h>

#include <opencv2/opencv.hpp>

int main(int argv, char** argc) {
  std::string path_left = argc[1];

  cv::Mat img_left = cv::imread(path_left, cv::IMREAD_COLOR);
  std::cout << img_left.rows << std::endl;
  std::cout << img_left.cols << std::endl;
}