#ifndef _image_processing_h_
#define _image_processing_h_

//==================================================================================
// 类型定义和包含文件
//==================================================================================
#include "PTS_Deal.h" // 包含 PTS_Deal.h
#include "zf_common_headfile.h" // 包含基础头文件

typedef enum
{
    no_pts ,    // 无效点状态
    left_pts,   // 左边界点状态
    Right_pts,  // 右边界点状态
    two_pts,    // 双边界点状态
}Pts_state;


//==================================================================================
// 外部变量声明
//==================================================================================
extern uint8 Image[MT9V03X_H][MT9V03X_W]; // 原始图像数据
extern uint8 Seed_State;                 // 种子点状态
extern uint8 original_left[2];           // 原始左边界点
extern uint8 original_right[2];          // 原始右边界点
extern uint8 mid_position;               // 中线位置
extern uint8 Index;                      // 索引变量
extern uint8 width_base;                 // 基础宽度
extern uint32 left_threshold;            // 左边界阈值
extern uint32 right_threshold;           // 右边界阈值
extern uint8 low_threshold;              // 底部阈值

//==================================================================================
// 图像处理基础操作
//==================================================================================
/**
 * @brief 复制图像数据
 */
void Image_Copy(void);


//==================================================================================
// 逆透视变换相关函数 (Image Coordinate -> Bird's-Eye View)
//==================================================================================
/**
 * @brief 初始化图像逆透视变换矩阵
 * @param change_un_Mat 3x3 的逆透视变换矩阵
 */
void ImagePerspective_Init(float change_un_Mat[3][3]);

/**
 * @brief 对输入的单个点进行逆透视变换
 * @param y 输入点的 Y 坐标
 * @param x 输入点的 X 坐标
 * @param output 输出变换后的坐标数组 [x, y]
 */
void CalculateTransform_inverse(uint8 y, uint8 x, uint8 output[2]);

/**
 * @brief 对输入的单个点进行透视变换 (使用预计算矩阵，原地修改)
 * @param pts_in 输入点的图像鸟瞰图坐标 [x, y]，变换后的原图坐标 [x, y] 将覆盖此数组
 */
void ImagePerspective(uint8 pts_in[2]);

/**
 * @brief 对点集进行逆透视变换
 * @param pts_in 输入点集图像坐标数组
 * @param pts_out 输出变换后的点集鸟瞰图坐标数组
 * @param step 输入点集的有效点数
 * @param step_Max pts_in 和 pts_out 数组的最大容量
 * @return uint8 变换后的有效点数
 */
uint8 PtsPerspective(uint8 pts_in[][2], uint8 pts_out[][2], uint8 step, uint8 step_Max);

/**
 * @brief 对单个点进行逆透视变换 (通过调用 CalculateTransform)
 * @param pt_in 输入点的图像坐标数组 [x, y]
 * @param pt_out 输出变换后的点鸟瞰图坐标数组 [x, y]
 * @return uint8 CalculateTransform 的返回值 (状态码或标志)
 */
uint8 TransformSinglePoint(uint8 pt_in[2], uint8 pt_out[2]);

/**
 * @brief 计算坐标变换 (可能是逆透视变换的核心实现)
 * @param y 输入点的 Y 坐标
 * @param x 输入点的 X 坐标
 * @param output 输出变换后的坐标数组 [x, y]
 * @return uint8 状态码或标志
 */
uint8 CalculateTransform(uint8 y, uint8 x, uint8 output[2]);


//==================================================================================
// 图像阈值处理
//==================================================================================
/**
 * @brief 对图像指定区域进行大津法 (Otsu) 阈值处理
 * @param x_start 区域起始 X 坐标
 * @param x_end 区域结束 X 坐标
 * @param y_start 区域起始 Y 坐标
 * @param y_end 区域结束 Y 坐标
 * @return uint8 计算得到的阈值
 */
uint8 Region_Otsu(uint8 x_start, uint8 x_end, uint8 y_start, uint8 y_end);

/**
 * @brief 计算指定列的大津法 (Otsu) 阈值
 * @param column_x 需要计算阈值的图像列索引
 * @return uint8 计算得到的阈值
 */
uint8 otsu_threshold(uint8 column_x);

/**
 * @brief 计算自适应阈值
 * @param y 像素的 Y 坐标
 * @param x 像素的 X 坐标
 * @param block 计算局部阈值的邻域块大小 (边长)
 * @param clip_value 阈值调整的裁剪值
 * @return uint8 计算得到的自适应阈值
 */
uint8 adaptiveThreshold(uint8 y, uint8 x, uint8 block, int8 clip_value);

/**
 * @brief 对图像进行高斯模糊处理
 * @param x 像素的 X 坐标
 * @param y 像素的 Y 坐标
 */
void blur(uint8 x,uint8 y);


//==================================================================================
// 种子点和边界点查找
//==================================================================================
/**
 * @brief 在指定高度范围内寻找种子点
 * @param High 搜索区域的上限 Y 坐标
 * @param mini_high 搜索区域的下限 Y 坐标
 */
void Seek_Pts_Seed(uint8 High, uint8 mini_high);

/**
 * @brief 从种子点向上搜索边界
 * @param point 种子点的坐标数组 (point[0] = y, point[1] = x)
 * @param seed_kind 种子点类型 (例如，左边界、右边界)
 */
void seek_up(uint8 point[2], uint8 seed_kind);

/**
 * @brief 从种子点向上搜索边界
 * @param x 需要查找的列的X坐标
 * @return uint8 找到的点的y坐标
 */
uint8 Seek_TOP(uint8 x);

/**
 * @brief 使用自适应阈值查找左边界线点集
 * @param block_size 自适应阈值计算的邻域块大小
 * @param clip_value 自适应阈值计算的裁剪值
 * @param pts_arry 用于存储找到的左边界点的数组
 * @return uint8 找到的点的数量
 */
uint8 findline_lefthand_adaptive(uint8 block_size, uint8 clip_value, uint8 pts_arry[][2]);

/**
 * @brief 使用自适应阈值查找右边界线点集
 * @param block_size 自适应阈值计算的邻域块大小
 * @param clip_value 自适应阈值计算的裁剪值
 * @param pts_arry 用于存储找到的右边界点的数组
 * @return uint8 找到的点的数量
 */
uint8 findline_righthand_adaptive(uint8 block_size, uint8 clip_value, uint8 pts_arry[][2]);


//==================================================================================
// 边界线跟踪
//==================================================================================
/**
 * @brief 跟踪左边界线
 * @param pts_in 输入的左边界候选点集
 * @param num 输入点集的有效点数
 * @param pts_out 输出跟踪到的左边界线点集
 * @param approx_num 近似计算时使用的点数
 * @param dist 搜索或匹配距离阈值
 * @return uint8 跟踪到的左边界线上的点数
 */
uint8 track_leftline(uint8 pts_in[][2], uint8 num, uint8 pts_out[][2], int8 approx_num, uint8 dist);

/**
 * @brief 跟踪右边界线
 * @param pts_in 输入的右边界候选点集
 * @param num 输入点集的有效点数
 * @param pts_out 输出跟踪到的右边界线点集
 * @param approx_num 近似计算时使用的点数
 * @param dist 搜索或匹配距离阈值
 * @return uint8 跟踪到的右边界线上的点数
 */
uint8 track_rightline(uint8 pts_in[][2], uint8 num, uint8 pts_out[][2], int8 approx_num, uint8 dist);

/**
 * @brief 跟踪左边界线上的特定点
 * @param pts_in 输入的左边界线点集
 * @param num 输入点集的有效点数
 * @param ID 要跟踪的点的索引或标识
 * @param pts_out 输出跟踪到的点的坐标
 * @param approx_num 近似计算时使用的点数
 * @param dist 搜索或匹配距离阈值
 */
void track_leftpoint(uint8 pts_in[][2], uint8 num, uint8 ID, uint8 pts_out[2], int8 approx_num, uint8 dist);

/**
 * @brief 跟踪右边界线上的特定点
 * @param pts_in 输入的右边界线点集
 * @param num 输入点集的有效点数
 * @param ID 要跟踪的点的索引或标识
 * @param pts_out 输出跟踪到的点的坐标
 * @param approx_num 近似计算时使用的点数
 * @param dist 搜索或匹配距离阈值
 */
void track_rightpoint(uint8 pts_in[][2], uint8 num, uint8 ID, uint8 pts_out[2], int8 approx_num, uint8 dist);


//==================================================================================
// 点集处理与插值
//==================================================================================
/**
 * @brief 对点集进行模糊处理 (例如均值滤波)
 * @param step 输入点集的有效点数
 * @param kernel 模糊核的大小
 * @param pts_in 输入点集数组 (原地修改)
 * @return uint8 处理后的有效点数 (可能不变)
 */
uint8 blur_points(uint8 step, uint8 kernel, uint8 pts_in[][2]);

/**
 * @brief 对点集进行重采样，使其点间距近似均匀
 * @param pts_in 输入点集数组
 * @param step1 输入点集的有效点数
 * @param step2 输出点集数组的最大容量
 * @param dist 期望的点间距
 * @return uint8 重采样后的有效点数
 */
uint8 resample_points(uint8 pts_in[][2], uint8 step1, uint8 step2, uint8 dist);

/**
 * @brief 在两点之间添加直线上的点 (线性插值)
 * @param location1 直线起点坐标
 * @param location2 直线终点坐标
 * @param pts 用于存储添加点的数组
 * @param add_location 开始添加点的数组索引
 * @param dist 添加点之间的距离
 * @return uint8 添加点后，数组中新的有效点索引 (add_location + 添加的点数)
 */
uint8 addline(uint8 location1[2], uint8 location2[2], uint8 pts[][2], uint8 add_location, uint8 dist);

/**
 * @brief 在三点定义的二次贝塞尔曲线上添加点 (插值)
 * @param location1 贝塞尔曲线起点
 * @param location2 贝塞尔曲线控制点
 * @param location3 贝塞尔曲线终点
 * @param pts 用于存储添加点的数组
 * @param add_location 开始添加点的数组索引
 * @param dist 添加点之间的近似距离 (沿曲线)
 * @return uint8 添加点后，数组中新的有效点索引 (add_location + 添加的点数)
 */
uint8 add_three_point_bezier(uint8 location1[2], uint8 location2[2], uint8 location3[2], uint8 pts[][2], uint8 add_location, uint8 dist);


//==================================================================================
// 几何计算与特征提取
//==================================================================================
/**
 * @brief 计算点集中指定点处的局部角度
 * @param pts_in 输入的点集数组
 * @param num 输入点集的有效点数
 * @param ID 计算角度的目标点的索引
 * @param dist 计算角度时考虑的邻近点距离
 * @return float 计算得到的局部角度 (弧度或度，需要根据实现确认)
 */
float local_angle_points(uint8 pts_in[][2], uint8 num, uint8 ID, uint8 dist);

/**
 * @brief 使用三个点计算曲率 (基于 Menger 曲率)
 * @param P0 第一个点的坐标
 * @param P1 第二个点的坐标 (中心点)
 * @param P2 第三个点的坐标
 * @return float 计算得到的曲率
 */
float compute_curvature(uint8 P0[2], uint8 P1[2], uint8 P2[2]);

/**
 * @brief 获取点集中的最大 Y 坐标 (通常代表图像中的最低点或最远点)
 * @param pts_in 输入的点集数组
 * @param num 输入点集的有效点数
 * @return uint8 点集中的最大 Y 坐标
 */
uint8 Get_high(uint8 pts_in[][2], uint8 num);

/**
 * @brief 计算两点之间的欧氏距离
 * @param pt1 第一个点的坐标
 * @param pt2 第二个点的坐标
 * @return float 两点间的距离
 */
float Get_Distance(uint8 pt1[2], uint8 pt2[2]);

/**
 * @brief 对角度数组进行非极大值抑制 (NMS)
 * @param angle_in 输入的角度数组
 * @param num 输入数组的元素个数
 * @param angle_out 输出经过 NMS 处理的角度数组
 * @param kernel NMS 操作的窗口大小
 */
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);


//==================================================================================
// 控制与决策 (可能需要移至更上层模块)
//==================================================================================
/**
 * @brief 根据中线数据和速度计算控制误差
 * @param midline 中线数据结构指针
 * @param speed 当前速度
 * @param speed_decision 速度相关的决策参数
 * @return float 计算得到的控制误差值
 */
float Get_error(MidlineData *midline, int speed, float speed_decision);


#endif // _image_processing_h_
