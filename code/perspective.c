#include "perspective.h"
#include <math.h>

/**
 * @brief 左前角点和右前角点的索引
 * FL_corner_index: 左前角点在点集中的索引
 * FR_corner_index: 右前角点在点集中的索引
 */
uint8 FL_corner_index, FR_corner_index;

/**
 * @brief 透视变换后顶边距离图像顶部的像素距离
 * 默认值为50，可根据实际标定动态调整
 */
uint8 up_distance = 50;

/**
 * @brief 逆透视变换矩阵（3x3）
 * 用于将透视图像转换为俯视图像
 */
double inverse_matrix[3][3];

#define EPSILON 1e-10  ///< 浮点数比较的极小值

/**
 * @brief 高斯消元法解线性方程组Ax=b
 * @param A 系数矩阵，按行优先展开为一维数组，大小8x8
 * @param b 常数向量，大小8
 * @param x 解向量，大小8
 */
void solveLinearSystem(double A[64], double b[8], double x[8]) {
    int n = 8;

    // 消元过程
    for (int k = 0; k < n; k++) {
        // 选主元
        double max_val = fabs(A[k * n + k]);
        int max_row = k;
        for (int i = k + 1; i < n; i++) {
            if (fabs(A[i * n + k]) > max_val) {
                max_val = fabs(A[i * n + k]);
                max_row = i;
            }
        }

        // 行交换
        if (max_row != k) {
            for (int j = k; j < n; j++) {
                double temp = A[k * n + j];
                A[k * n + j] = A[max_row * n + j];
                A[max_row * n + j] = temp;
            }
            double temp = b[k];
            b[k] = b[max_row];
            b[max_row] = temp;
        }

        // 消元
        for (int i = k + 1; i < n; i++) {
            double factor = A[i * n + k] / A[k * n + k];
            for (int j = k; j < n; j++) {
                A[i * n + j] -= factor * A[k * n + j];
            }
            b[i] -= factor * b[k];
        }
    }

    // 回代求解
    for (int i = n - 1; i >= 0; i--) {
        double sum = 0;
        for (int j = i + 1; j < n; j++) {
            sum += A[i * n + j] * x[j];
        }
        x[i] = (b[i] - sum) / A[i * n + i];
    }
}

/**
 * @brief 对4个点进行归一化处理，并计算归一化变换矩阵T
 * @param points 输入的4个点，每个点为2维（uint8_t类型）
 * @param normalized 输出的归一化后点坐标（double类型，8个元素）
 * @param T 输出的归一化变换矩阵（3x3）
 */
void NormalizePoints(const uint8_t points[][2], double normalized[8], double T[3][3]) {
    double meanX = 0, meanY = 0;
    for (int i = 0; i < 4; ++i) {
        meanX += points[i][0];
        meanY += points[i][1];
    }
    meanX /= 4;
    meanY /= 4;

    double sumDist = 0;
    for (int i = 0; i < 4; ++i) {
        double dx = points[i][0] - meanX;
        double dy = points[i][1] - meanY;
        sumDist += sqrt(dx * dx + dy * dy);
    }
    double avgDist = sumDist / 4;
    double scale = (avgDist > EPSILON) ? (sqrt(2.0) / avgDist) : 1.0;

    // 构造归一化矩阵T
    T[0][0] = scale;  T[0][1] = 0;      T[0][2] = -scale * meanX;
    T[1][0] = 0;      T[1][1] = scale;  T[1][2] = -scale * meanY;
    T[2][0] = 0;      T[2][1] = 0;      T[2][2] = 1;

    // 归一化点坐标
    for (int i = 0; i < 4; ++i) {
        normalized[2 * i]     = (points[i][0] - meanX) * scale;
        normalized[2 * i + 1] = (points[i][1] - meanY) * scale;
    }
}

/**
 * @brief 对单应性矩阵H进行去归一化处理
 * @param H 输入输出，归一化单应性矩阵，处理后为去归一化矩阵
 * @param T1 归一化变换矩阵（源点）
 * @param T2_inv 归一化变换矩阵（目标点）的逆
 */
void DenormalizeMatrix(double H[3][3], const double T1[3][3], const double T2_inv[3][3]) {
    double temp[3][3];
    // temp = T2_inv * H
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            temp[i][j] = 0;
            for (int k = 0; k < 3; ++k) {
                temp[i][j] += T2_inv[i][k] * H[k][j];
            }
        }
    }
    // H = temp * T1
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            H[i][j] = 0;
            for (int k = 0; k < 3; ++k) {
                H[i][j] += temp[i][k] * T1[k][j];
            }
        }
    }
}

/**
 * @brief 计算3x3矩阵的行列式
 * @param matrix 输入矩阵
 * @return 行列式值
 */
double determinant3x3(const double matrix[3][3]) {
    return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
           matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
           matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
}

/**
 * @brief 计算3x3矩阵的逆矩阵
 * @param matrix 输入矩阵
 * @param inverse 输出逆矩阵
 */
void inverseMatrix3x3(const double matrix[3][3], double inverse[3][3]) {
    double det = determinant3x3(matrix);
    if (fabs(det) < EPSILON) {
        memset(inverse, 0, sizeof(double) * 9);
        return;
    }

    double invDet = 1.0 / det;

    inverse[0][0] = (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) * invDet;
    inverse[0][1] = (matrix[0][2] * matrix[2][1] - matrix[0][1] * matrix[2][2]) * invDet;
    inverse[0][2] = (matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1]) * invDet;
    inverse[1][0] = (matrix[1][2] * matrix[2][0] - matrix[1][0] * matrix[2][2]) * invDet;
    inverse[1][1] = (matrix[0][0] * matrix[2][2] - matrix[0][2] * matrix[2][0]) * invDet;
    inverse[1][2] = (matrix[0][2] * matrix[1][0] - matrix[0][0] * matrix[1][2]) * invDet;
    inverse[2][0] = (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]) * invDet;
    inverse[2][1] = (matrix[0][1] * matrix[2][0] - matrix[0][0] * matrix[2][1]) * invDet;
    inverse[2][2] = (matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]) * invDet;
}

/**
 * @brief 计算逆透视变换矩阵
 * @param perspective_points 输入的透视图像上的4个点
 * @param topdown_points 目标俯视图像上的4个点
 * @param matrix 输出的逆透视变换矩阵（3x3）
 * @return 计算成功返回true
 */
bool CalculateInversePerspectiveMatrix(const uint8_t perspective_points[][2], const uint8_t topdown_points[][2], double matrix[3][3]) {
    double norm_perspective[8], norm_topdown[8];
    double T_perspective[3][3], T_topdown[3][3];
    double T_topdown_inv[3][3];

    // 步骤1：对输入点和目标点归一化
    NormalizePoints(perspective_points, norm_perspective, T_perspective);
    NormalizePoints(topdown_points, norm_topdown, T_topdown);

    // 步骤2：计算目标点归一化矩阵的逆
    inverseMatrix3x3(T_topdown, T_topdown_inv);

    // 步骤3：构造线性方程组A*h=b，求解单应性矩阵参数h
    double A[64] = { 0 };
    double b[8] = { 0 };

    for (int i = 0; i < 4; ++i) {
        double x = norm_perspective[2 * i];
        double y = norm_perspective[2 * i + 1];
        double u = norm_topdown[2 * i];
        double v = norm_topdown[2 * i + 1];

        int row = 2 * i;
        A[row * 8 + 0] = x;
        A[row * 8 + 1] = y;
        A[row * 8 + 2] = 1;
        A[row * 8 + 6] = -u * x;
        A[row * 8 + 7] = -u * y;
        b[row] = u;

        row = 2 * i + 1;
        A[row * 8 + 3] = x;
        A[row * 8 + 4] = y;
        A[row * 8 + 5] = 1;
        A[row * 8 + 6] = -v * x;
        A[row * 8 + 7] = -v * y;
        b[row] = v;
    }

    double h[8];
    solveLinearSystem(A, b, h);

    // 步骤4：组装归一化单应性矩阵
    double H_norm[3][3] = {
        {h[0], h[1], h[2]},
        {h[3], h[4], h[5]},
        {h[6], h[7], 1.0}
    };

    // 步骤5：去归一化
    DenormalizeMatrix(H_norm, T_perspective, T_topdown_inv);
    
    // 步骤6：归一化矩阵，使H[2][2]为-1
    double scale = -1.0 / H_norm[2][2];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            H_norm[i][j] *= scale;
        }
    }

    // 步骤7：输出最终逆透视矩阵
    memcpy(matrix, H_norm, sizeof(double) * 9);
    return true;
}

/**
 * @brief 对原始图像进行逆透视变换，生成俯视图像
 * @param inverse_matrix 逆透视变换矩阵（3x3）
 * @param transformed_image 输出的变换后图像（二维数组）
 */
void Image_Transformed(double inverse_matrix[3][3], uint8 transformed_image[MT9V03X_H][MT9V03X_W]) {
    // 遍历目标图像的每个像素，计算其在原图像中的对应位置
    for (int v = 0; v < MT9V03X_H; v++) {
        for (int u = 0; u < MT9V03X_W; u++) {
            float x = (float)u;
            float y = (float)v;

            // 计算齐次坐标变换的分母
            float denominator = (inverse_matrix[0][0] * inverse_matrix[1][1] - inverse_matrix[0][0] * inverse_matrix[2][1] * y -
                                  inverse_matrix[0][1] * inverse_matrix[1][0] + inverse_matrix[0][1] * inverse_matrix[2][0] * y +
                                  inverse_matrix[1][0] * inverse_matrix[2][1] * x - inverse_matrix[1][1] * inverse_matrix[2][0] * x);

            // 计算原图像中的x、y坐标
            float transformed_x = (inverse_matrix[0][1] * inverse_matrix[1][2] - inverse_matrix[0][1] * inverse_matrix[2][2] * y -
                                inverse_matrix[0][2] * inverse_matrix[1][1] + inverse_matrix[0][2] * inverse_matrix[2][1] * y +
                                inverse_matrix[1][1] * inverse_matrix[2][2] * x - inverse_matrix[1][2] * inverse_matrix[2][1] * x) / denominator;

            float transformed_y = (-inverse_matrix[0][0] * inverse_matrix[1][2] + inverse_matrix[0][0] * inverse_matrix[2][2] * y +
                            inverse_matrix[0][2] * inverse_matrix[1][0] - inverse_matrix[0][2] * inverse_matrix[2][0] * y -
                            inverse_matrix[1][0] * inverse_matrix[2][2] * x + inverse_matrix[1][2] * inverse_matrix[2][0] * x) / denominator;

            int x1 = (int)(transformed_x + 0.5f);
            int y1 = (int)(transformed_y + 0.5f);

            // 边界判断，超出原图像范围则填充255
            if (x1 >= 0 && x1 < MT9V03X_W && y1 >= 0 && y1 < MT9V03X_H) {
                transformed_image[v][u] = mt9v03x_image[y1][x1];
            } else {
                transformed_image[v][u] = 255;
            }
        }
    }
}

/**
 * @brief 获取透视变换所需的四个关键点
 * 主要流程：拷贝图像、寻找种子点、处理左右边界点、寻找角点等
 */
void Get_Four_Points(void)
{
    Image_Copy(); // 拷贝原始图像
    Seek_Pts_Seed(Start_high, Mini_high); // 寻找种子点
    LeftPts_Deal_Original();  // 处理左边界点
    RightPts_Deal_Original(); // 处理右边界点
    left_boundary.now_step_original = left_boundary.original_step;
    right_boundary.now_step_original = right_boundary.original_step;
    find_corner(&left_boundary, &right_boundary); // 寻找左右边界角点
    left_boundary.Lp_id = left_boundary.map[0].original_index;
    right_boundary.Lp_id = right_boundary.map[0].original_index;
    // 计算左右起始点
    uint8 left_statr_point[2] = { left_boundary.original_pts[left_boundary.Lp_id][0] - 5, left_boundary.original_pts[left_boundary.Lp_id][1] - 5 };
    uint8 right_statr_point[2] = { right_boundary.original_pts[right_boundary.Lp_id][0] + 5, right_boundary.original_pts[right_boundary.Lp_id][1] - 5 };
    seek_up(left_statr_point, left_pts);   // 向上追踪左边界
    seek_up(right_statr_point, Right_pts); // 向上追踪右边界
    FLeftPts_Deal_Original();  // 处理追踪后的左边界点
    FRightPts_Deal_Original(); // 处理追踪后的右边界点
    Fleft_boundary.original_step = Arry_Filter_2(Fleft_boundary.original_pts, Fleft_boundary.original_step);   // 滤波
    Fright_boundary.original_step = Arry_Filter_2(Fright_boundary.original_pts, Fright_boundary.original_step); // 滤波
    FL_corner_index = seek_corner(Fleft_boundary.original_pts, Fleft_boundary.original_step, 4);  // 左前角点
    FR_corner_index = seek_corner(Fright_boundary.original_pts, Fright_boundary.original_step, 5); // 右前角点
    ips200_show_gray_image(0, 0, Image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0); // 显示图像
}

/**
 * @brief 执行一次透视变换并显示结果
 * 1. 组装四个透视点和目标点
 * 2. 计算逆透视矩阵
 * 3. 变换图像并显示
 */
void Perspective_Transform(void)
{
    // 透视图像上的四个点（左前、右前、左后、右后）
    uint8_t perspective_points[4][2] = {
        {Fleft_boundary.original_pts[FL_corner_index][0], Fleft_boundary.original_pts[FL_corner_index][1]},
        {Fright_boundary.original_pts[FR_corner_index][0], Fright_boundary.original_pts[FR_corner_index][1]},
        {left_boundary.original_pts[left_boundary.Lp_id][0], left_boundary.original_pts[left_boundary.Lp_id][1]},
        {right_boundary.original_pts[right_boundary.Lp_id][0], right_boundary.original_pts[right_boundary.Lp_id][1]}
    };
    // 俯视图像上的四个点（目标点，固定位置）
    uint8_t topdown_points[4][2] = {
        {74, up_distance - 20},
        {114, up_distance - 20},
        {74, up_distance + 20},
        {114, up_distance + 20}
    };

    // 计算逆透视矩阵并变换图像
    if (CalculateInversePerspectiveMatrix(perspective_points, topdown_points, inverse_matrix)) {
        memset(Image, 0, sizeof(Image));
        Image_Transformed(inverse_matrix, Image);
        ips200_show_gray_image(0, 130, Image[0], 
                              MT9V03X_W, MT9V03X_H,
                              MT9V03X_W, MT9V03X_H, 0);
    }
}

/**
 * @brief 将逆透视矩阵写入Flash
 * 用于保存标定结果
 */
void Write_Matrix(void)
{
    flash_erase_page(0, MATRIX_INDEX);
    for (uint8 i = 0; i < 9; i++)
    {
        flash_union_buffer[i].float_type = inverse_matrix[i / 3][i % 3];
    }
    flash_write_page_from_buffer(0, MATRIX_INDEX);
}

/**
 * @brief 从Flash读取逆透视矩阵
 * @param matrix 输出的3x3矩阵
 */
void Read_Matrix(double matrix[3][3])
{
    flash_read_page_to_buffer(0, MATRIX_INDEX);
    for (uint8 i = 0; i < 9; i++)
    {
        matrix[i / 3][i % 3] = flash_union_buffer[i].float_type;
    }
}

/**
 * @brief 逆透视标定主流程
 * 按键1：采集四点并计算逆透视矩阵，显示变换结果
 * 按键2：自动调整up_distance，重新计算逆透视矩阵并保存
 */
void Get_inverse_matrix(void)
{
    if (key_flag == Button_1)
    {
        key_flag = 0;
        mt9v03x_finish_flag = 0;
        while (mt9v03x_finish_flag == 0); // 等待图像采集完成
        Get_Four_Points();                // 获取四个关键点
        Perspective_Transform();          // 计算逆透视并显示
    }
    if (key_flag == Button_2)
    {
        key_flag = 0;
        uint8 time = 0;
        uint8 flag = 0;
        // 从下往上扫描，清除底部非255区域
        for (int v = MT9V03X_H - 1; v > 0; v--)
        {
            for (int u = 0; u < MT9V03X_W; u++)
            {
                if (Image[v][u] != 255)
                {
                    time++;
                }
            }
            if (time != 0)
            {
                for (int u = 0; u < MT9V03X_W; u++)
                {
                    Image[v][u] = 255;
                }
                time = 0;
            }
            else
            {
                break;
            }
        }
        // 再次从下往上扫描，找到有效区域的上边界
        for (int v = MT9V03X_H - 1; v > 0; v--) {
            for (int u = 0; u < MT9V03X_W; u++) {
                if (Image[v][u] != 255)
                {
                    time++;
                }
            }
            if (time > INVERSE_MATRIX_SIZE)
            {
                flag = v + 1;
                break;
            }
        }
        // 根据有效区域调整up_distance
        up_distance += (MT9V03X_H - flag);
        Perspective_Transform();      // 重新计算逆透视并显示
        Write_Matrix();               // 保存逆透视矩阵
        ImagePerspective_Init(inverse_matrix); // 初始化逆透视模块
        up_distance = 50;             // 恢复默认值
    }
    
}
