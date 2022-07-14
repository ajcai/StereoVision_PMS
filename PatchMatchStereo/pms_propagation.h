#pragma once
#include <iostream>
#include "PatchMatchStereo/cost_computer.h"
#include "Utils/pms_types.h"
#include "Utils/timer.h"

class PMSPropagation final {
 public:
  PMSPropagation(const sint32 width, const sint32 height, const uint8* img_left,
                 const uint8* img_right, const PGradient* grad_left,
                 const PGradient* grad_right, DisparityPlane* plane_left,
                 DisparityPlane* plane_right, const PMSOption& option,
                 float32* cost_left, float32* cost_right,
                 float32* disparity_map);
  ~PMSPropagation();

 public:
  void DoPropagation();

 private:
  void ComputeCostData() const;
  void SpatialPropagation(const sint32& x, const sint32& y,
                          const sint32& direction) const;
  void ViewPropagation(const sint32& x, const sint32& y) const;
  void PlaneRefine(const sint32& x, const sint32& y) const;

 private:
  PMSOption option_;
  sint32 num_iter_;
  /** @brief 影像宽高 */
  sint32 width_;
  sint32 height_;
  /** @brief 影像数据 */
  const uint8* img_left_;
  const uint8* img_right_;
  /** @brief 代价计算器 */
  CostComputer* cost_cpt_left_;
  CostComputer* cost_cpt_right_;
  /** @brief 梯度数据 */
  const PGradient* grad_left_;
  const PGradient* grad_right_;
  /** @brief 平面数据 */
  DisparityPlane* plane_left_;
  DisparityPlane* plane_right_;
  /** @brief 代价数据 */
  float32* cost_left_;
  float32* cost_right_;
  /** @brief 视差数据 */
  float32* disparity_map_;
};
