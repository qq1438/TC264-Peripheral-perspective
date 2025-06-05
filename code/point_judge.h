#ifndef _point_judge_h_
#define _point_judge_h_

#include "zf_common_headfile.h"

// ���ӵ��Ժ�ͳ���
#define CURVATURE_THRESHOLD 2.0f    // ������ֵ
#define ANGLE_LOW 70.0f // �Ƕ���ֵ����
#define ANGLE_HIGH 140.0f // �Ƕ���ֵ����
#define WINDOW_SIZE 10               // �������ڴ�С


void find_corner(BoundaryData *left, BoundaryData *right);
uint8 seek_corner(uint8 pts_in[][2], uint8 step, uint8 reverse_order);

#endif
