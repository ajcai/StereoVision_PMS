#include <bits/stdc++.h>

#include <opencv2/opencv.hpp>

#include "PatchMatchStereo/patch_match_stereo.h"

int main(int argv, char** argc) {
  std::string path_left = argc[1];
  std::string path_right = argc[2];

  cv::Mat img_left = cv::imread(path_left, cv::IMREAD_COLOR);
  cv::Mat img_right = cv::imread(path_right, cv::IMREAD_COLOR);
  if (img_left.data == nullptr || img_right.data == nullptr) {
    std::cout << "读取影像失败！" << std::endl;
    return -1;
  }
  if (img_left.rows != img_right.rows || img_left.cols != img_right.cols) {
    std::cout << "左右影像尺寸不一致！" << std::endl;
    return -1;
  }
  // -------------------------
  const sint32 width = static_cast<uint32>(img_left.cols);
  const sint32 height = static_cast<uint32>(img_right.rows);
  // -------------------------
  PMSOption pms_option;
  PatchMatchStereo pms;
  if (!pms.Initialize(width, height, pms_option)) {
    std::cout << "初始化失败" << std::endl;
    return -2;
  }
  // -------------------------
  auto disparity = new float32[uint32(width * height)]();  // 初始化为0

  if (!pms.Match(img_left.data, img_right.data, disparity)) {
    std::cout << "匹配失败" << std::endl;
    return -2;
  }
}