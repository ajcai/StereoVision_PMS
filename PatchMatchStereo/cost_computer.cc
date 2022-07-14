#include "PatchMatchStereo/cost_computer.h"

float32 CostComputerPMS::Compute(const sint32& x, const sint32& y,
                                 const float32& d) const {
  const float32 xr = x - d;
  if (xr < 0.0f || xr >= width_) {
    return (1 - alpha_) * tau_clr_ + alpha_ * tau_grad_;
  }
  // 颜色空间距离
  const auto clr_p = GetColor(img_left_, x, y);
  const auto clr_q = GetColor(img_right_, xr, y);
  const auto dc = std::min(
      static_cast<float32>(abs(clr_p.b - clr_q.x) + abs(clr_p.g - clr_q.y) +
                           abs(clr_p.r - clr_q.z)),
      tau_clr_);
  // 梯度空间距离
  const auto grad_p = GetGradient(grad_left_, x, y);
  const auto grad_q = GetGradient(grad_right_, xr, y);
  const auto dg = std::min(
      static_cast<float32>(abs(grad_p.x - grad_q.x) + abs(grad_p.y - grad_q.y)),
      tau_grad_);
  //代价值
  return (1 - alpha_) * dc + alpha_ * dg;
}

float32 CostComputerPMS::ComputeA(const sint32& x, const sint32& y,
                                  const DisparityPlane& p) const {
  const auto pat = patch_size_ / 2;
  const auto& clr_p = GetColor(img_left_, x, y);
  float32 cost = 0.0f;
  sint32 patch_size = 2 * pat + 1;
  float32* cost_patch = new float32[patch_size * patch_size];
#pragma omp parallel for
  for (sint32 pr = -pat; pr <= pat; ++pr) {
    sint32 y_p = y + pr;
    for (sint32 pc = -pat; pc <= pat; ++pc) {
      sint32 x_p = x + pc;
      sint32 patch_id = (pr + pat) * patch_size + (pc + pat);
      cost_patch[patch_id] = 0;
      if (y_p < 0 || y_p > height_ - 1 || x_p < 0 || x_p > width_ - 1) {
        continue;
      }
      // 计算视差值
      float32 d = p.to_disparity(x_p, y_p);
      if (d < min_disp_ || d > max_disp_) {
        // cost += COST_PUNISH;
        cost_patch[patch_id] = COST_PUNISH;
        continue;
      }
      // 为了解决edge-fattening问题
      // 计算权值，颜色差异越大，为同一平面内的可能性越小
      const auto& clr_q = GetColor(img_left_, x_p, y_p);
      const auto dc = abs(clr_p.b - clr_q.b) + abs(clr_p.g - clr_q.g) +
                      abs(clr_p.r - clr_q.r);
      const auto w = exp(-dc / gamma_);
      // cost += w * Compute(x_p, y_p, d);
      cost_patch[patch_id] = w * Compute(x_p, y_p, d);
    }
  }
  for (sint32 k = 0; k < patch_size * patch_size; ++k) {
    cost += cost_patch[k];
  }
  delete[] cost_patch;
  return cost;
}

// PColor CostComputerPMS::GetColor(const uint8* img_data, const sint32& x,
//                                  const sint32& y) const {
//   auto* pixel = img_data + (y * width_ + x) * 3;
//   return {pixel[0], pixel[1], pixel[2]};
// }

// PVector3f CostComputerPMS::GetColor(const uint8* img_data, const float32& x,
//                                     const sint32& y) const {
//   float32 clr[3];
//   const auto x1 = static_cast<sint32>(x);
//   const sint32 x2 = (x1 < width_ - 1) ? (x1 + 1) : x1;
//   const float32 ofs = x - x1;

//   for (sint32 n = 0; n < 3; ++n) {
//     const auto& g1 = img_data[(y * width_ + x1) * 3 + n];
//     const auto& g2 = img_data[(y * width_ + x2) * 3 + n];
//     clr[n] = (1 - ofs) * g1 + ofs * g2;
//   }
//   return {clr[0], clr[1], clr[2]};
// }

// PGradient CostComputerPMS::GetGradient(const PGradient* grad_data,
//                                        const sint32& x, const sint32& y)
//                                        const {
//   return grad_data[y * width_ + x];
// }
