#ifndef _PTS_Deal_h_
#define _PTS_Deal_h_

#include "zf_common_typedef.h"
#include "zf_device_mt9v03x.h"
#include "stdbool.h"

/**
 * @brief 拐点坐标映射结构体
 * @details 用于存储特征点在原始点集、变换后点集以及当前点集中的索引信息
 * @var CornerMapping::original 原始点集中的坐标 (x, y)
 * @var CornerMapping::transformed 变换后点集中的坐标 (x, y)
 * @var CornerMapping::now_index 变换后点在当前点集中的索引
 * @var CornerMapping::original_index 该点在原始点集中的索引
 */
typedef struct {
    uint8 original[2];      ///< 原始坐标
    uint8 transformed[2];   ///< 变换后坐标
    uint8 now_index;        ///< 在Now数组中的索引
    uint8 original_index;   ///< 在Original数组中的索引
} CornerMapping;

/**
 * @brief 边界数据结构体
 * @details 存储边界的原始点、处理点、特征点映射及相关状态信息
 * @var BoundaryData::original_step 原始点集长度
 * @var BoundaryData::now_step 当前点集长度
 * @var BoundaryData::now_step_original 保留原始步长
 * @var BoundaryData::original_pts 原始点集 (每行为一个点的x, y)
 * @var BoundaryData::now_pts 当前点集 (每行为一个点的x, y)
 * @var BoundaryData::map 特征点映射数组
 * @var BoundaryData::Lp_id 角点ID
 * @var BoundaryData::angle 当前边界角度
 * @var BoundaryData::is_straight 是否为直线段
 * @var BoundaryData::Lp_state 角点状态
 * @var BoundaryData::is_lost 丢线标志
 */
typedef struct {
    uint8 original_step;
    uint8 now_step;
    uint8 now_step_original;
    uint8 original_pts[MT9V03X_W][2];
    uint8 now_pts[MT9V03X_H][2];
    CornerMapping map[3];
    uint8 Lp_id;
    float angle;
    bool is_straight;
    bool Lp_state;
    bool is_lost;
} BoundaryData;

/**
 * @brief 前视边界数据结构体
 * @details 存储前视边界的原始点和处理点
 * @var FBoundaryData::original_step 原始点集长度
 * @var FBoundaryData::original_pts 原始点集 (每行为一个点的x, y)
 * @var FBoundaryData::now_step 当前点集长度
 * @var FBoundaryData::now_pts 当前点集 (每行为一个点的x, y)
 */
typedef struct {
    uint8 original_step;
    uint8 original_pts[MT9V03X_W][2];
    uint8 now_step;
    uint8 now_pts[MT9V03X_H][2];
} FBoundaryData;

#include "zf_common_headfile.h"

#define WIDTH 40         ///< 处理点集最大宽度
#define Step_Max 40      ///< 点集最大步长
#define Start_high 115   ///< 起始高度
#define Mini_high 90     ///< 最小高度

/**
 * @brief 左边界数据结构体，存储左边界的原始点、处理点等信息
 */
extern BoundaryData left_boundary;

/**
 * @brief 右边界数据结构体，存储右边界的原始点、处理点等信息
 */
extern BoundaryData right_boundary;

/**
 * @brief 前视左边界数据结构体，存储前视左边界的原始点、处理点等信息
 */
extern FBoundaryData Fleft_boundary;

/**
 * @brief 前视右边界数据结构体，存储前视右边界的原始点、处理点等信息
 */
extern FBoundaryData Fright_boundary;

/**
 * @brief 处理左边界原始点，提取左边界原始点集
 * @note 结果存入left_boundary.original_pts，点数存入left_boundary.original_step
 */
void LeftPts_Deal_Original(void);

/**
 * @brief 处理右边界原始点，提取右边界原始点集
 * @note 结果存入right_boundary.original_pts，点数存入right_boundary.original_step
 */
void RightPts_Deal_Original(void);

/**
 * @brief 处理前视左边界原始点，提取前视左边界原始点集
 * @note 结果存入Fleft_boundary.original_pts，点数存入Fleft_boundary.original_step
 */
void FLeftPts_Deal_Original(void);

/**
 * @brief 处理前视右边界原始点，提取前视右边界原始点集
 * @note 结果存入Fright_boundary.original_pts，点数存入Fright_boundary.original_step
 */
void FRightPts_Deal_Original(void);

#endif
