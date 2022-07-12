#include "Utils/visualization.h"
void Utils::VisualizeDisparityMap(const float32* disp_map, const sint32& width,
                                  const sint32& height, cv::Mat& vis_gray,
                                  cv::Mat& vis_color) {
  vis_gray = cv::Mat(height, width, CV_8UC1);
  float32 min_disp = float32(width), max_disp = float32(-width);
  for (sint32 i = 0; i < height; ++i) {
    for (sint32 j = 0; j < width; ++j) {
      float32 disp = std::abs(disp_map[i * width + j]);
      if (disp != Invalid_Float) {
        min_disp = std::min(min_disp, disp);
        max_disp = std::max(max_disp, disp);
      }
    }
  }
  std::cout << "max & min disp: " << max_disp << " " << min_disp << std::endl;
  float32 disp_range = max_disp - min_disp;
  for (sint32 i = 0; i < height; ++i) {
    for (sint32 j = 0; j < width; ++j) {
      float32 disp = std::abs(disp_map[i * width + j]);
      if (disp != Invalid_Float) {
        vis_gray.data[i * width + j] =
            static_cast<uchar>((disp - min_disp) / disp_range * 255);
      } else {
        vis_gray.data[i * width + j] = 0;
      }
    }
  }
  cv::applyColorMap(vis_gray, vis_color, cv::COLORMAP_JET);
}
