#include "PTS_Deal.h"
#include "clip.h"
#include "image_processing.h"

/**
 * @brief ��߽����ݽṹ�壬�洢��߽��ԭʼ�㡢��������Ϣ
 */
BoundaryData left_boundary = {0};

/**
 * @brief �ұ߽����ݽṹ�壬�洢�ұ߽��ԭʼ�㡢��������Ϣ
 */
BoundaryData right_boundary = {0};

/**
 * @brief ǰ����߽����ݽṹ�壬�洢ǰ����߽��ԭʼ�㡢��������Ϣ
 */
FBoundaryData Fleft_boundary = {0};

/**
 * @brief ǰ���ұ߽����ݽṹ�壬�洢ǰ���ұ߽��ԭʼ�㡢��������Ϣ
 */
FBoundaryData Fright_boundary = {0};

/**
 * @brief ������߽�ԭʼ�㣬��ȡ��߽�ԭʼ�㼯
 * @note �������left_boundary.original_pts����������left_boundary.original_step
 */
void LeftPts_Deal_Original(void)
{
    left_boundary.original_step = findline_lefthand_adaptive(5, 3, left_boundary.original_pts);
}

/**
 * @brief �����ұ߽�ԭʼ�㣬��ȡ�ұ߽�ԭʼ�㼯
 * @note �������right_boundary.original_pts����������right_boundary.original_step
 */
void RightPts_Deal_Original(void)
{
    right_boundary.original_step = findline_righthand_adaptive(5, 3, right_boundary.original_pts);
}

/**
 * @brief ����ǰ����߽�ԭʼ�㣬��ȡǰ����߽�ԭʼ�㼯
 * @note �������Fleft_boundary.original_pts����������Fleft_boundary.original_step
 */
void FLeftPts_Deal_Original(void)
{
    Fleft_boundary.original_step = findline_lefthand_adaptive(5, 3, Fleft_boundary.original_pts);
}

/**
 * @brief ����ǰ���ұ߽�ԭʼ�㣬��ȡǰ���ұ߽�ԭʼ�㼯
 * @note �������Fright_boundary.original_pts����������Fright_boundary.original_step
 */
void FRightPts_Deal_Original(void)
{
    Fright_boundary.original_step = findline_righthand_adaptive(5, 3, Fright_boundary.original_pts);
}
