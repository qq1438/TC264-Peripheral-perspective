#ifndef _image_processing_h_
#define _image_processing_h_

#include "PTS_Deal.h"
#include "zf_common_headfile.h"

/**
 * @brief 种子点状态枚举
 * @details
 *   - no_pts: 未找到种子点
 *   - left_pts: 仅找到左种子点
 *   - Right_pts: 仅找到右种子点
 *   - two_pts: 同时找到左右种子点
 */
typedef enum
{
    no_pts,      ///< 未找到种子点
    left_pts,    ///< 仅找到左种子点
    Right_pts,   ///< 仅找到右种子点
    two_pts,     ///< 同时找到左右种子点
} Pts_state;

/**
 * @brief 图像原始数据，灰度图像，分辨率为MT9V03X_H x MT9V03X_W
 */
extern uint8 Image[MT9V03X_H][MT9V03X_W];

/**
 * @brief 当前种子点状态，参见Pts_state
 */
extern uint8 Seed_State;

/**
 * @brief 左侧种子点原始坐标，original_left[0]=x, original_left[1]=y
 */
extern uint8 original_left[2];

/**
 * @brief 右侧种子点原始坐标，original_right[0]=x, original_right[1]=y
 */
extern uint8 original_right[2];

/**
 * @brief 图像中线位置（列坐标）
 */
extern uint8 mid_position;

/**
 * @brief 左侧边界自适应阈值
 */
extern uint32 left_threshold;

/**
 * @brief 右侧边界自适应阈值
 */
extern uint32 right_threshold;

/**
 * @brief 拷贝原始图像到处理缓冲区
 */
void Image_Copy(void);

/**
 * @brief 初始化逆透视变换矩阵
 * @param change_un_Mat 输入的3x3变换矩阵
 */
void ImagePerspective_Init(float change_un_Mat[3][3]);

/**
 * @brief 计算逆变换后的点坐标
 * @param y 输入点的行坐标
 * @param x 输入点的列坐标
 * @param output 输出点坐标，output[0]=x, output[1]=y
 */
void CalculateTransform_inverse(uint8 y, uint8 x, uint8 output[2]);

/**
 * @brief 对输入点进行逆透视变换，结果写回pts_in
 * @param pts_in 输入输出点，pts_in[0]=x, pts_in[1]=y
 */
void ImagePerspective(uint8 pts_in[2]);

/**
 * @brief 对一组点进行逆透视变换
 * @param pts_in 输入点集，二维数组，每行为[x, y]
 * @param pts_out 输出点集，二维数组，每行为[x, y]
 * @param step 输入点的数量
 * @param step_Max 输出点的最大数量
 * @retval 实际输出点数
 */
uint8 PtsPerspective(uint8 pts_in[][2], uint8 pts_out[][2], uint8 step, uint8 step_Max);

/**
 * @brief 对单个点进行透视变换
 * @param pt_in 输入点 [x, y]
 * @param pt_out 输出点 [x, y]
 * @retval 0-成功，其他-失败
 */
uint8 TransformSinglePoint(uint8 pt_in[2], uint8 pt_out[2]);

/**
 * @brief 计算正变换后的点坐标
 * @param y 输入点的行坐标
 * @param x 输入点的列坐标
 * @param output 输出点坐标，output[0]=x, output[1]=y
 * @retval 0-成功，其他-失败
 */
uint8 CalculateTransform(uint8 y, uint8 x, uint8 output[2]);

/**
 * @brief 自适应阈值计算
 * @param y 行坐标
 * @param x 列坐标
 * @param block 阈值计算窗口大小
 * @param clip_value 阈值修正值
 * @retval 阈值
 */
uint8 adaptiveThreshold(uint8 y, uint8 x, uint8 block, int8 clip_value);

/**
 * @brief 对指定点进行模糊处理
 * @param x 列坐标
 * @param y 行坐标
 */
void blur(uint8 x, uint8 y);

/**
 * @brief 寻找左右种子点
 * @param High 起始行
 * @param mini_high 最小行
 */
void Seek_Pts_Seed(uint8 High, uint8 mini_high);

/**
 * @brief 沿指定方向向上寻找种子点
 * @param point 起始点 [x, y]
 * @param seed_kind 种子点类型（left_pts或Right_pts）
 */
void seek_up(uint8 point[2], uint8 seed_kind);

/**
 * @brief 自适应阈值左边界寻线
 * @param block_size 阈值窗口大小
 * @param clip_value 阈值修正值
 * @param pts_arry 输出点集
 * @retval 实际找到的点数
 */
uint8 findline_lefthand_adaptive(uint8 block_size, uint8 clip_value, uint8 pts_arry[][2]);

/**
 * @brief 自适应阈值右边界寻线
 * @param block_size 阈值窗口大小
 * @param clip_value 阈值修正值
 * @param pts_arry 输出点集
 * @retval 实际找到的点数
 */
uint8 findline_righthand_adaptive(uint8 block_size, uint8 clip_value, uint8 pts_arry[][2]);

/**
 * @brief 对点集进行模糊滤波
 * @param step 点集数量
 * @param kernel 滤波核大小
 * @param pts_in 输入点集
 * @retval 实际输出点数
 */
uint8 blur_points(uint8 step, uint8 kernel, uint8 pts_in[][2]);

/**
 * @brief 计算点集某点的局部角度
 * @param pts_in 输入点集
 * @param num 点集数量
 * @param ID 当前点索引
 * @param dist 前后距离
 * @retval 局部角度（弧度）
 */
float local_angle_points(uint8 pts_in[][2], uint8 num, uint8 ID, uint8 dist);

/**
 * @brief 计算三点曲率
 * @param P0 第一个点 [x, y]
 * @param P1 第二个点 [x, y]
 * @param P2 第三个点 [x, y]
 * @retval 曲率值
 */
float compute_curvature(uint8 P0[2], uint8 P1[2], uint8 P2[2]);

/**
 * @brief 获取点集中的最大行坐标
 * @param pts_in 输入点集
 * @param num 点集数量
 * @retval 最大行坐标
 */
uint8 Get_high(uint8 pts_in[][2], uint8 num);

/**
 * @brief 计算两点间欧氏距离
 * @param pt1 第一个点 [x, y]
 * @param pt2 第二个点 [x, y]
 * @retval 距离
 */
float Get_Distance(uint8 pt1[2], uint8 pt2[2]);

/**
 * @brief 对角度序列进行非极大值抑制
 * @param angle_in 输入角度数组
 * @param num 数组长度
 * @param angle_out 输出角度数组
 * @param kernel 抑制核大小
 */
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);

/**
 * @brief 在指定行用Canny边缘检测寻找边界
 * @param row 行号
 * @param start_x 起始列
 * @param direction 方向（0-左，1-右）
 * @retval 边界点列坐标
 */
uint8 find_edge_canny_row(uint8 row, uint8 start_x, uint8 direction);

/**
 * @brief 在指定列用Canny边缘检测寻找边界
 * @param col 列号
 * @param start_y 起始行
 * @param direction 方向（0-上，1-下）
 * @retval 边界点行坐标
 */
uint8 find_edge_canny_col(uint8 col, uint8 start_y, uint8 direction);

#endif // _image_processing_h_
