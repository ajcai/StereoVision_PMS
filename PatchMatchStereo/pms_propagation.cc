#include "PatchMatchStereo/pms_propagation.h"

PMSPropagation::PMSPropagation(const sint32 width, const sint32 height,
                               const uint8* img_left, const uint8* img_right,
                               const PGradient* grad_left,
                               const PGradient* grad_right,
                               DisparityPlane* plane_left,
                               DisparityPlane* plane_right,
                               const PMSOption& option, float32* cost_left,
                               float32* cost_right, float32* disparity_map)
    : cost_cpt_left_(nullptr),
      cost_cpt_right_(nullptr),
      width_(width),
      height_(height),
      num_iter_(0),
      img_left_(img_left),
      img_right_(img_right),
      grad_left_(grad_left),
      grad_right_(grad_right),
      plane_left_(plane_left),
      plane_right_(plane_right),
      cost_left_(cost_left),
      cost_right_(cost_right),
      disparity_map_(disparity_map) {
  cost_cpt_left_ = new CostComputerPMS(
      img_left, img_right, grad_left, grad_right, width, height,
      option.patch_size, option.min_disparity, option.max_disparity,
      option.gamma, option.alpha, option.tau_col, option.tau_grad);
  cost_cpt_right_ = new CostComputerPMS(
      img_right, img_left, grad_right, grad_left, width, height,
      option.patch_size, -option.max_disparity, -option.min_disparity,
      option.gamma, option.alpha, option.tau_col, option.tau_grad);
  option_ = option;
  std::cout << "init cost" << std::endl;
  ComputeCostData();
  std::cout << "init cost done" << std::endl;
}

PMSPropagation::~PMSPropagation() {
  if (cost_cpt_left_) {
    delete cost_cpt_left_;
    cost_cpt_left_ = nullptr;
  }
  if (cost_cpt_right_) {
    delete cost_cpt_right_;
    cost_cpt_right_ = nullptr;
  }
}

void PMSPropagation::ComputeCostData() const {
  if (!cost_cpt_left_ || !cost_cpt_right_ || !img_left_ || !img_right_ ||
      !grad_left_ || !grad_right_ || !cost_left_ || !plane_left_ ||
      !plane_right_ || !disparity_map_) {
    std::cout << "empty ptr" << std::endl;
    return;
  }
  auto* cost_cpt = dynamic_cast<CostComputerPMS*>(cost_cpt_left_);
  for (sint32 y = 0; y < height_; ++y) {
    for (sint32 x = 0; x < width_; ++x) {
      const auto& plane_p = plane_left_[y * width_ + x];
      cost_left_[y * width_ + x] = cost_cpt->ComputeA(x, y, plane_p);
      // std::cout << x << " " << y << " " << cost_left_[y * width_ + x]
      //           << std::endl;
    }
  }
}

void PMSPropagation::DoPropagation() {
  const sint32 dir = (num_iter_ % 2 == 0) ? 1 : -1;
  sint32 y = (dir == 1) ? 0 : height_ - 1;
  for (sint32 i = 0; i < height_; ++i) {
    sint32 x = (dir == 1) ? 0 : width_ - 1;
    for (sint32 j = 0; j < width_; ++j) {
      SpatialPropagation(x, y, dir);
      ViewPropagation(x, y);
      x += dir;
    }
    y += dir;
  }
  ++num_iter_;
}
void PMSPropagation::SpatialPropagation(const sint32& x, const sint32& y,
                                        const sint32& direction) const {
  const sint32 dir = direction;
  // 获取p当前的视差平面并计算代价
  auto& plane_p = plane_left_[y * width_ + x];
  auto& cost_p = cost_left_[y * width_ + x];
  auto* cost_cpt = dynamic_cast<CostComputerPMS*>(cost_cpt_left_);
  // 获取p左(右)像素的视差平面，计算将平面分配给p时的代价，取较小值
  const sint32 xd = x - dir;
  if (xd >= 0 && xd < width_) {
    auto& plane = plane_left_[y * width_ + xd];
    if (plane != plane_p) {
      const auto cost = cost_cpt->ComputeA(x, y, plane);
      if (cost < cost_p) {
        plane_p = plane;
        cost_p = cost;
      }
    }
  }
  // 获取p上(下)像素的视差平面，计算将平面分配给p时的代价，取较小值
  const sint32 yd = y - dir;
  if (yd >= 0 && yd < height_) {
    auto& plane = plane_left_[yd * width_ + x];
    if (plane != plane_p) {
      const auto cost = cost_cpt->ComputeA(x, y, plane);
      if (cost < cost_p) {
        plane_p = plane;
        cost_p = cost;
      }
    }
  }
}
void PMSPropagation::ViewPropagation(const sint32& x, const sint32& y) const {}
void PMSPropagation::PlaneRefine(const sint32& x, const sint32& y) const { ; }