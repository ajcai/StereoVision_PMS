#pragma once
#include <vector>

#include "pms_types.h"

class PatchMatchStereo {
 public:
  PatchMatchStereo();
  ~PatchMatchStereo();
  bool Initialize(const sint32& width, const sint32& height,
                  const PMSOption& option);

 public:
  int width;
  int height;

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
