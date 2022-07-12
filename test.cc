#include <iostream>
#include <opencv2/opencv.hpp>

#include "PatchMatchStereo/patch_match_stereo.h"
#include "Utils/visualization.h"

#define BOOST_TEST_MODULE example_test_suite
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(pms_match) {
  std::string path_left = "../Data/Cone/im2.png";
  std::string path_right = "../Data/Cone/im6.png";

  cv::Mat img_left = cv::imread(path_left, cv::IMREAD_COLOR);
  cv::Mat img_right = cv::imread(path_right, cv::IMREAD_COLOR);
  BOOST_REQUIRE(img_left.data != nullptr && img_right.data != nullptr);
  BOOST_REQUIRE(img_left.rows == img_right.rows &&
                img_left.cols == img_right.cols);
  // -------------------------
  const sint32 width = static_cast<uint32>(img_left.cols);
  const sint32 height = static_cast<uint32>(img_right.rows);
  // -------------------------
  PMSOption pms_option;
  PatchMatchStereo pms;
  BOOST_REQUIRE(pms.Initialize(width, height, pms_option));
  // -------------------------
  auto disparity = new float32[uint32(width * height)]();  // 初始化为0
  BOOST_REQUIRE(pms.Match(img_left.data, img_right.data, disparity));

  cv::Mat vis_gray, vis_color;
  Utils::VisualizeDisparityMap(disparity, width, height, vis_gray, vis_color);
  cv::imwrite("./disp_gray.jpg", vis_gray);
  cv::imwrite("./disp_color.jpg", vis_color);
}