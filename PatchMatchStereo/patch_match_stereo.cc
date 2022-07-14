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
  // 随机初始化
  RandomInitialization();
  // 计算灰度图
  ComputeGray();
  // 计算梯度图
  ComputeGradient();
  // 迭代传播
  Propagation();
  // 平面转视差
  PlaneToDisparity();

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

void PatchMatchStereo::ComputeGray() const {
  const sint32 width = width_;
  const sint32 height = height_;
  if (width <= 0 || height <= 0 || img_left_ == nullptr ||
      img_right_ == nullptr || gray_left_ == nullptr ||
      gray_right_ == nullptr) {
    return;
  }
  // 彩色转灰度
  for (sint32 n = 0; n < 2; ++n) {
    auto* color = (n == 0) ? img_left_ : img_right_;
    auto* gray = (n == 0) ? gray_left_ : gray_right_;
    for (sint32 i = 0; i < height; ++i) {
      for (sint32 j = 0; j < width; ++j) {
        const auto b = color[i * width * 3 + 3 * j];
        const auto g = color[i * width * 3 + 3 * j + 1];
        const auto r = color[i * width * 3 + 3 * j + 2];
        gray[i * width + j] = uint8((r * 38 + g * 75 + b * 15) >> 7);
        // gray[i * width + j] = uint8((r * 19595 + g * 38469 + b * 7472) >>
        // 16);
      }
    }
  }
}

void PatchMatchStereo::ComputeGradient() const {
  const sint32 width = width_;
  const sint32 height = height_;
  if (width <= 0 || height <= 0 || img_left_ == nullptr ||
      img_right_ == nullptr || gray_left_ == nullptr ||
      gray_right_ == nullptr) {
    return;
  }
  // Sobel梯度算子
  for (sint32 n = 0; n < 2; ++n) {
    auto* gray = (n == 0) ? gray_left_ : gray_right_;
    auto* grad = (n == 0) ? grad_left_ : grad_right_;
    int max_grad = 0, min_grad = 999;
    for (int y = 0; y < height - 1; ++y) {
      for (int x = 1; x < width - 1; ++x) {
        const auto grad_x =
            (-gray[(y - 1) * width + x - 1] + gray[(y - 1) * width + x + 1]) +
            (-2 * gray[y * width + x - 1] + 2 * gray[y * width + x + 1]) +
            (-gray[(y + 1) * width + x - 1] + gray[(y + 1) * width + x + 1]);
        const auto grad_y =
            (-gray[(y - 1) * width + x - 1] - 2 * gray[(y - 1) * width + x] -
             gray[(y - 1) * width + x + 1]) +
            (gray[(y + 1) * width + x - 1] + 2 * gray[(y + 1) * width + x] -
             gray[(y + 1) * width + x + 1]);
        // 这里除以8是为了让梯度的最大值不超过255，这样计算代价时梯度差和颜色差位于同一个尺度
        grad[y * width + x].x = grad_x / 4;
        grad[y * width + x].y = grad_y / 4;
        max_grad = (grad_x > max_grad) ? grad_x : max_grad;
        min_grad = (grad_x < min_grad) ? grad_x : min_grad;
      }
    }
    std::cout << "max&min grad: " << max_grad << " " << min_grad << std::endl;
  }
}

void PatchMatchStereo::Propagation() const {
  const sint32 width = width_;
  const sint32 height = height_;
  if (width <= 0 || height <= 0 || img_left_ == nullptr ||
      img_right_ == nullptr || grad_left_ == nullptr ||
      grad_right_ == nullptr || disp_left_ == nullptr ||
      disp_right_ == nullptr || plane_left_ == nullptr ||
      plane_right_ == nullptr || cost_left_ == nullptr ||
      cost_right_ == nullptr) {
    return;
  }
  // 左右视图匹配参数
  const auto option_left = option_;
  auto option_right = option_;
  option_right.min_disparity = -option_left.max_disparity;
  option_right.max_disparity = -option_left.min_disparity;
  // 左右视图传播实例
  std::cout << "Propagation instance" << std::endl;
  PMSPropagation propa_right(width, height, img_right_, img_left_, grad_right_,
                             grad_left_, plane_right_, plane_left_,
                             option_right, cost_right_, cost_left_,
                             disp_right_);
  PMSPropagation propa_left(width, height, img_left_, img_right_, grad_left_,
                            grad_right_, plane_left_, plane_right_, option_left,
                            cost_left_, cost_right_, disp_left_);

  for (sint16 k = 0; k < option_.num_iters; ++k) {
    std::cout << "Propagation " << k << std::endl;
    propa_left.DoPropagation();
    propa_right.DoPropagation();
  }
}

void PatchMatchStereo::PlaneToDisparity() const {
  const sint32 width = width_;
  const sint32 height = height_;
  if (width <= 0 || height <= 0 || img_left_ == nullptr ||
      img_right_ == nullptr || grad_left_ == nullptr ||
      grad_right_ == nullptr || disp_left_ == nullptr ||
      disp_right_ == nullptr || plane_left_ == nullptr ||
      plane_right_ == nullptr) {
    return;
  }
  for (sint32 k = 0; k < 2; ++k) {
    auto* plane_ptr = (k == 0) ? plane_left_ : plane_right_;
    auto* disp_ptr = (k == 0) ? disp_left_ : disp_right_;
    for (sint32 y = 0; y < height; ++y) {
      for (sint32 x = 0; x < width_; ++x) {
        const sint32 p = y * width + x;
        const auto& plane = plane_ptr[p];
        disp_ptr[p] = plane.to_disparity(x, y);
      }
    }
  }
}
