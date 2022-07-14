#pragma once
#include <vector>

#include "PatchMatchStereo/pms_propagation.h"
#include "Utils/pms_types.h"

class PatchMatchStereo {
 public:
  PatchMatchStereo();
  ~PatchMatchStereo();

 public:
  /**
   * @brief 类的初始化，完成内存预分配、参数预设
   *
   * @param width 输入，图像宽
   * @param height 输入，图像高
   * @param option 输入，算法参数
   * @return true
   * @return false
   */
  bool Initialize(const sint32& width, const sint32& height,
                  const PMSOption& option);

  bool Match(const uint8* img_left, const uint8* img_right, float32* disp_left);

 private:
  void RandomInitialization() const;
  /** @brief 内存释放 */
  void Release();
  /** @brief 计算灰度图 */
  void ComputeGray() const;
  /** @brief 计算梯度图 */
  void ComputeGradient() const;
  /** @brief 迭代传播 */
  void Propagation() const;
  /** @brief 平面转视差 */
  void PlaneToDisparity() const;

 private:
  /** \brief PMS参数	 */
  PMSOption option_;

  /** \brief 影像宽	 */
  sint32 width_;

  /** \brief 影像高	 */
  sint32 height_;

  /** \brief 左影像数据	 */
  const uint8* img_left_;
  /** \brief 右影像数据	 */
  const uint8* img_right_;

  /** \brief 左影像灰度数据	 */
  uint8* gray_left_;
  /** \brief 右影像灰度数据	 */
  uint8* gray_right_;

  /** \brief 左影像梯度数据	 */
  PGradient* grad_left_;
  /** \brief 右影像梯度数据	 */
  PGradient* grad_right_;

  /** \brief 左影像聚合代价数据	 */
  float32* cost_left_;
  /** \brief 右影像聚合代价数据	 */
  float32* cost_right_;

  /** \brief 左影像视差图	*/
  float32* disp_left_;
  /** \brief 右影像视差图	*/
  float32* disp_right_;

  /** \brief 左影像平面集	*/
  DisparityPlane* plane_left_;
  /** \brief 右影像平面集	*/
  DisparityPlane* plane_right_;

  /** \brief 是否初始化标志	*/
  bool is_initialized_;
};
