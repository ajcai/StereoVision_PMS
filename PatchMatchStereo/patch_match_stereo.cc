#include "PatchMatchStereo/patch_match_stereo.h"

#include <iostream>
#include <random>

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
PatchMatchStereo::~PatchMatchStereo() { Release(); }

bool PatchMatchStereo::Match(const uint8* img_left, const uint8* img_right,
                             float32* disp_left) {
  if (!is_initialized_) {
    return false;
  }
  if (img_left == nullptr || img_right == nullptr) {
    return false;
  }
  img_left_ = img_left;
  img_right_ = img_right;
  //随机初始化
  RandomInitialization();
  for (sint32 y = 0; y < height_; ++y) {
    for (sint32 x = 0; x < width_; ++x) {
      const sint32 p = y * width_ + x;
      disp_left[p] = disp_left_[p];
    }
  }
  return true;
}

void PatchMatchStereo::Release() {
  SAFE_DELETE(grad_left_)
  SAFE_DELETE(grad_left_)
  SAFE_DELETE(cost_left_)
  SAFE_DELETE(cost_right_)
  SAFE_DELETE(disp_left_)
  SAFE_DELETE(disp_right_)
  SAFE_DELETE(plane_left_)
  SAFE_DELETE(plane_right_)
}

void PatchMatchStereo::RandomInitialization() const {
  const sint32 width = width_;
  const sint32 height = height_;
  if (width <= 0 || height <= 0 || disp_left_ == nullptr ||
      disp_right_ == nullptr || plane_left_ == nullptr ||
      plane_right_ == nullptr) {
    return;
  }
  const auto& option = option_;
  const sint32 min_disparity = option.min_disparity;
  const sint32 max_disparity = option.max_disparity;

  // 随机数生成器
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float32> rand_d(
      static_cast<float32>(min_disparity), static_cast<float32>(max_disparity));
  std::uniform_real_distribution<float32> rand_n(-1.0f, 1.0f);

  for (int k = 0; k < 2; ++k) {
    auto* disp_ptr = k == 0 ? disp_left_ : disp_right_;
    auto* plane_ptr = k == 0 ? plane_left_ : plane_right_;
    sint32 sign = (k == 0) ? 1 : -1;
    for (sint32 y = 0; y < height; ++y) {
      for (sint32 x = 0; x < width; ++x) {
        const sint32 p = y * width + x;
        // 随机视差值
        float32 disp = sign * rand_d(gen);
        if (option.is_integer_disp) {
          disp = static_cast<float32>(round(disp));
        }
        disp_ptr[p] = disp;

        // 随机法向量
        PVector3f norm;
        if (!option.is_force_fpw) {
          norm.x = rand_n(gen);
          norm.y = rand_n(gen);
          float32 z = rand_n(gen);
          while (z == 0.0f) {
            z = rand_n(gen);
          }
          norm.z = z;
          norm.normalize();
        } else {
          norm.x = 0.0f;
          norm.y = 0.0f;
          norm.z = 1.0f;
        }
        // 计算视差平面
        plane_ptr[p] = DisparityPlane(x, y, norm, disp);
      }
    }
  }
}

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
  // ··· 开辟内存空间
  const sint32 img_size = width * height;
  const sint32 disp_range = option.max_disparity - option.min_disparity;
  // 灰度数据
  gray_left_ = new uint8[img_size];
  gray_right_ = new uint8[img_size];
  // 梯度数据
  grad_left_ = new PGradient[img_size]();
  grad_right_ = new PGradient[img_size]();
  // 代价数据
  cost_left_ = new float32[img_size];
  cost_right_ = new float32[img_size];
  // 视差图
  disp_left_ = new float32[img_size];
  disp_right_ = new float32[img_size];
  // 平面集
  plane_left_ = new DisparityPlane[img_size];
  plane_right_ = new DisparityPlane[img_size];

  is_initialized_ = grad_left_ && grad_right_ && disp_left_ && disp_right_ &&
                    plane_left_ && plane_right_;

  return is_initialized_;
}