#pragma once
#include <opencv2/opencv.hpp>

#include "Utils/pms_types.h"

namespace Utils {
void VisualizeDisparityMap(const float32* disp_map, const sint32& width,
                           const sint32& height, cv::Mat& vis_gray,
                           cv::Mat& vis_color);
}
