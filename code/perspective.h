#ifndef __PERSPECTIVE_H__
#define __PERSPECTIVE_H__

#include "zf_common_headfile.h"

/**
 * @brief Flash中存储逆透视矩阵的页索引
 */
#define MATRIX_INDEX 0

/**
 * @brief 赛道宽度
 */
#define INVERSE_MATRIX_SIZE 40

/**
 * @brief 计算并获取逆透视变换矩阵
 * 
 * 该函数会根据当前图像中的特征点，计算逆透视变换所需的3x3矩阵，并将结果存储到全局变量inverse_matrix中。
 * 通常用于标定或图像变换前的准备工作。
 */
void Get_inverse_matrix(void);

/**
 * @brief 从Flash中读取逆透视变换矩阵
 * 
 * @param matrix 输出参数，3x3的double类型矩阵，用于存放读取到的逆透视矩阵
 * 
 * 该函数会从Flash指定页读取9个浮点数，按行优先方式填充到matrix[3][3]中。
 */
void Read_Matrix(double matrix[3][3]);

/**
 * @brief 对原始图像进行逆透视变换，生成俯视图像
 * 
 * @param inverse_matrix 输入参数，3x3的逆透视变换矩阵
 * @param transformed_image 输出参数，变换后的图像数据，大小为MT9V03X_H x MT9V03X_W
 * 
 * 该函数会遍历目标图像的每个像素，通过逆透视矩阵计算其在原图像中的对应位置，并生成变换后的俯视图像。
 */
void Image_Transformed(double inverse_matrix[3][3], uint8 transformed_image[MT9V03X_H][MT9V03X_W]);

#endif
