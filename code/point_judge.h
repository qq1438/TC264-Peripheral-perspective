#ifndef _point_judge_h_
#define _point_judge_h_

#include "zf_common_headfile.h"

// 增加调试宏和常量
#define CURVATURE_THRESHOLD 2.0f    // 曲率阈值
#define ANGLE_THRESHOLD 30.0f    // 直道角度阈值
#define ANGLE_LOW 70.0f // 角度阈值下限
#define ANGLE_HIGH 140.0f // 角度阈值上限
#define WINDOW_SIZE 10               // 滑动窗口大小

/**
 * @brief 综合调用各处理函数，判断左右边界的拐点、直线段和丢线状态。
 * @param left  左边界数据结构体指针
 * @param right 右边界数据结构体指针
 */
void find_corner(BoundaryData *left, BoundaryData *right);

/**
 * @brief 在点集内根据不同策略寻找角点（特征点）索引。
 * @param pts_in        输入点集，二维数组，每行为一个点的(x, y)
 * @param step          点集长度
 * @param reverse_order 是否反向查找，1为反向，0为正向
 * @return              返回找到的角点索引
 */
uint8 seek_corner(uint8 pts_in[][2], uint8 step, uint8 reverse_order);

#endif
