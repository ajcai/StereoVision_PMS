#pragma once
#include <algorithm>

#include "Utils/pms_types.h"
#define COST_PUNISH 120.f
// #define USE_FAST_EXP
/* 快速exp */
inline double fast_exp(double x) {
  x = 1.0 + x / 1024;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  return x;
}

class CostComputer {
 public:
  CostComputer()
      : img_left_(nullptr),
        img_right_(nullptr),
        width_(0),
        height_(0),
        patch_size_(0),
        min_disp_(0),
        max_disp_(0) {}
  CostComputer(const uint8* img_left, const uint8* img_right,
               const sint32& width, const sint32& height,
               const sint32& patch_size, const sint32& min_disp,
               const sint32& max_disp) {
    img_left_ = img_left;
    img_right_ = img_right;
    width_ = width;
    height_ = height;
    patch_size_ = patch_size;
    min_disp_ = min_disp;
    max_disp_ = max_disp;
  }
  virtual ~CostComputer() = default;

 public:
  virtual float32 Compute(const sint32& i, const sint32& j,
                          const float32& d) const = 0;

 public:
  const uint8* img_left_;
  const uint8* img_right_;
  sint32 width_;
  sint32 height_;
  sint32 patch_size_;
  sint32 min_disp_;
  sint32 max_disp_;
};

class CostComputerPMS : public CostComputer {
 public:
  CostComputerPMS()
      : grad_left_(nullptr),
        grad_right_(nullptr),
        gamma_(0),
        alpha_(0),
        tau_clr_(0),
        tau_grad_(0){};
  CostComputerPMS(const uint8* img_left, const uint8* img_right,
                  const PGradient* grad_left, const PGradient* grad_right,
                  const sint32& width, const sint32& height,
                  const sint32& patch_size, const sint32& min_disp,
                  const sint32& max_disp, const float32& gamma,
                  const float32& alpha, const float32& t_clr,
                  const float32 t_grad)
      : CostComputer(img_left, img_right, width, height, patch_size, min_disp,
                     max_disp) {
    grad_left_ = grad_left;
    grad_right_ = grad_right;
    gamma_ = gamma;
    alpha_ = alpha;
    tau_clr_ = t_clr;
    tau_grad_ = t_grad;
  };
  // 计算左图p点视差为d的代价值
  float32 Compute(const sint32& x, const sint32& y,
                  const float32& d) const override;

  inline PColor GetColor(const uint8* img_data, const sint32& x,
                         const sint32& y) const {
    auto* pixel = img_data + (y * width_ + x) * 3;
    return {pixel[0], pixel[1], pixel[2]};
  }

  inline PVector3f GetColor(const uint8* img_data, const float32& x,
                            const sint32& y) const {
    float32 clr[3];
    const auto x1 = static_cast<sint32>(x);
    const sint32 x2 = (x1 < width_ - 1) ? (x1 + 1) : x1;
    const float32 ofs = x - x1;

    for (sint32 n = 0; n < 3; ++n) {
      const auto& g1 = img_data[(y * width_ + x1) * 3 + n];
      const auto& g2 = img_data[(y * width_ + x2) * 3 + n];
      clr[n] = (1 - ofs) * g1 + ofs * g2;
    }
    return {clr[0], clr[1], clr[2]};
  }

  inline PGradient GetGradient(const PGradient* grad_data, const sint32& x,
                               const sint32& y) const {
    return grad_data[y * width_ + x];
  }

  // 计算左图p点视差平面为p时的聚合代价值
  float32 ComputeA(const sint32& x, const sint32& y,
                   const DisparityPlane& p) const;

 private:
  const PGradient* grad_left_;
  const PGradient* grad_right_;
  float32 gamma_;
  float32 alpha_;
  float32 tau_clr_;
  float32 tau_grad_;
};
