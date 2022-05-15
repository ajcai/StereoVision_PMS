#include "patch_match_stereo.h"
PatchMatchStereo::PatchMatchStereo()
    : width_(0),
      height_(0),
      img_left_(nullptr),
      img_right_(nullptr),
      gray_left_(nullptr),
      gray_right_(nullptr),
      grad_left_(nullptr),
      grad_right_(nullptr),
      cost_left_(nullptr),
      cost_right_(nullptr),
      disp_left_(nullptr),
      disp_right_(nullptr),
      plane_left_(nullptr),
      plane_right_(nullptr),
      is_initialized_(false) {}
PatchMatchStereo::~PatchMatchStereo() {}

bool PatchMatchStereo::Initialize(const sint32& width, const sint32& height,
                                  const PMSOption& option) {
  // ··· 赋值
  // 影像尺寸
  width_ = width;
  height_ = height;
  // PMS参数
  option_ = option;
  if (width <= 0 || height <= 0) {
    return false;
  }
}