#ifndef _point_judge_h_
#define _point_judge_h_

#include "zf_common_headfile.h"

// 增加调试宏和常量
#define CURVATURE_THRESHOLD 2.0f    // 曲率阈值
#define ANGLE_LOW 70.0f // 角度阈值下限
#define ANGLE_HIGH 140.0f // 角度阈值上限
#define WINDOW_SIZE 10               // 滑动窗口大小


void find_corner(BoundaryData *left, BoundaryData *right);
uint8 seek_corner(uint8 pts_in[][2], uint8 step, uint8 reverse_order);

#endif
