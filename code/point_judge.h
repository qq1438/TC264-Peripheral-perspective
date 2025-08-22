#ifndef _point_judge_h_
#define _point_judge_h_

#include "zf_common_headfile.h"

// ���ӵ��Ժ�ͳ���
#define CURVATURE_THRESHOLD 2.0f    // ������ֵ
#define ANGLE_THRESHOLD 30.0f    // ֱ���Ƕ���ֵ
#define ANGLE_LOW 70.0f // �Ƕ���ֵ����
#define ANGLE_HIGH 140.0f // �Ƕ���ֵ����
#define WINDOW_SIZE 10               // �������ڴ�С

/**
 * @brief �ۺϵ��ø����������ж����ұ߽�Ĺյ㡢ֱ�߶κͶ���״̬��
 * @param left  ��߽����ݽṹ��ָ��
 * @param right �ұ߽����ݽṹ��ָ��
 */
void find_corner(BoundaryData *left, BoundaryData *right);

/**
 * @brief �ڵ㼯�ڸ��ݲ�ͬ����Ѱ�ҽǵ㣨�����㣩������
 * @param pts_in        ����㼯����ά���飬ÿ��Ϊһ�����(x, y)
 * @param step          �㼯����
 * @param reverse_order �Ƿ�����ң�1Ϊ����0Ϊ����
 * @return              �����ҵ��Ľǵ�����
 */
uint8 seek_corner(uint8 pts_in[][2], uint8 step, uint8 reverse_order);

#endif
