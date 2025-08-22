#include "PTS_Deal.h"
#include "clip.h"
#include "image_processing.h"

/**
 * @brief 左边界数据结构体，存储左边界的原始点、处理点等信息
 */
BoundaryData left_boundary = {0};

/**
 * @brief 右边界数据结构体，存储右边界的原始点、处理点等信息
 */
BoundaryData right_boundary = {0};

/**
 * @brief 前视左边界数据结构体，存储前视左边界的原始点、处理点等信息
 */
FBoundaryData Fleft_boundary = {0};

/**
 * @brief 前视右边界数据结构体，存储前视右边界的原始点、处理点等信息
 */
FBoundaryData Fright_boundary = {0};

/**
 * @brief 处理左边界原始点，提取左边界原始点集
 * @note 结果存入left_boundary.original_pts，点数存入left_boundary.original_step
 */
void LeftPts_Deal_Original(void)
{
    left_boundary.original_step = findline_lefthand_adaptive(5, 3, left_boundary.original_pts);
}

/**
 * @brief 处理右边界原始点，提取右边界原始点集
 * @note 结果存入right_boundary.original_pts，点数存入right_boundary.original_step
 */
void RightPts_Deal_Original(void)
{
    right_boundary.original_step = findline_righthand_adaptive(5, 3, right_boundary.original_pts);
}

/**
 * @brief 处理前视左边界原始点，提取前视左边界原始点集
 * @note 结果存入Fleft_boundary.original_pts，点数存入Fleft_boundary.original_step
 */
void FLeftPts_Deal_Original(void)
{
    Fleft_boundary.original_step = findline_lefthand_adaptive(5, 3, Fleft_boundary.original_pts);
}

/**
 * @brief 处理前视右边界原始点，提取前视右边界原始点集
 * @note 结果存入Fright_boundary.original_pts，点数存入Fright_boundary.original_step
 */
void FRightPts_Deal_Original(void)
{
    Fright_boundary.original_step = findline_righthand_adaptive(5, 3, Fright_boundary.original_pts);
}
