#include "image_processing.h"

/**
 * @brief 图像灰度数据，存储当前帧的图像。
 * @var Image[y][x]
 * y: 行坐标，范围0~MT9V03X_H-1
 * x: 列坐标，范围0~MT9V03X_W-1
 */
uint8 Image[MT9V03X_H][MT9V03X_W];

/**
 * @brief 左种子点坐标，Seed_left[0]为x，Seed_left[1]为y
 */
uint8 Seed_left[2];

/**
 * @brief 右种子点坐标，Seed_right[0]为x，Seed_right[1]为y
 */
uint8 Seed_right[2];

/**
 * @brief 左原始种子点坐标，original_left[0]为x，original_left[1]为y
 */
uint8 original_left[2];

/**
 * @brief 右原始种子点坐标，original_right[0]为x，original_right[1]为y
 */
uint8 original_right[2];

/**
 * @brief 当前左右种子点的中点x坐标
 */
uint8 mid_position = 94;

/**
 * @brief 当前种子点状态，取值见Pts_state枚举
 */
uint8 Seed_State = 0;

/**
 * @brief 左线自适应阈值累计值
 */
uint32 left_threshold = 0;

/**
 * @brief 右线自适应阈值累计值
 */
uint32 right_threshold = 0;

/**
 * @brief 拷贝mt9v03x_image到Image
 */
void Image_Copy(void){
    for (uint16_t i = 0; i < MT9V03X_H; i++) {
        for (uint16_t j = 0; j < MT9V03X_W; j++) {
            Image[i][j] = mt9v03x_image[i][j];
        }
    }
}

/**
 * @brief 透视变换矩阵，3x3
 * @var perspective_Mat[i][j]
 * i: 行，j: 列
 */
float perspective_Mat[3][3];

/**
 * @brief 对输入点(x, y)进行透视变换，输出output[2]为变换后坐标
 * @param y 输入点y坐标
 * @param x 输入点x坐标
 * @param output 输出点output[0]=x', output[1]=y'
 * @retval 1: 成功，0: 失败
 */
uint8 CalculateTransform(uint8 y, uint8 x, uint8 output[2]) {
    if(y >= MT9V03X_H || x >= MT9V03X_W) return 0;

    float fx = (float)x;
    float fy = (float)y;
    float denominator = (perspective_Mat[2][0] * fx + perspective_Mat[2][1] * fy + perspective_Mat[2][2]);

    if (fabsf(denominator) == 0) {
        return 0;
    }

    float transformed_x = (perspective_Mat[0][0] * fx + perspective_Mat[0][1] * fy + perspective_Mat[0][2]) / denominator;
    float transformed_y = (perspective_Mat[1][0] * fx + perspective_Mat[1][1] * fy + perspective_Mat[1][2]) / denominator;

    if (transformed_x >= 0.0f && transformed_x < (float)MT9V03X_W &&
        transformed_y >= 0.0f && transformed_y < (float)MT9V03X_H) {
        output[0] = (uint8)roundf(transformed_x);
        output[1] = (uint8)roundf(transformed_y);
        if (output[0] < MT9V03X_W && output[1] < MT9V03X_H) {
           return 1;
        }
    }

    return 0;
}

/**
 * @brief 对输入点(x, y)进行逆透视变换，输出output[2]为变换后坐标
 * @param y 输入点y坐标
 * @param x 输入点x坐标
 * @param output 输出点output[0]=x', output[1]=y'
 */
void CalculateTransform_inverse(uint8 y, uint8 x, uint8 output[2]) {
    float fx = (float)x;
    float fy = (float)y;
    float denominator = (perspective_Mat[2][0] * fx + perspective_Mat[2][1] * fy + perspective_Mat[2][2]);
    
    float transformed_x = (perspective_Mat[0][0] * fx + perspective_Mat[0][1] * fy + perspective_Mat[0][2]) / denominator;  
    float transformed_y = (perspective_Mat[1][0] * fx + perspective_Mat[1][1] * fy + perspective_Mat[1][2]) / denominator;
            
    output[0] = (uint8)roundf(transformed_x);
    output[1] = (uint8)roundf(MT9V03X_H - transformed_y);
}

/**
 * @brief 初始化透视变换矩阵
 * @param change_un_Mat 输入的3x3变换矩阵
 */
void ImagePerspective_Init(float change_un_Mat[3][3]) {
    for (int i = 0; i < 3; i++) {
         for (int j = 0; j < 3; j++) {
             perspective_Mat[i][j] = change_un_Mat[i][j];
         }
     }
}

/**
 * @brief 对输入点进行逆透视变换，结果写回pts_in
 * @param pts_in 输入输出点，pts_in[0]=x, pts_in[1]=y
 */
void ImagePerspective(uint8 pts_in[2]){
    uint8 x,y;
    x = pts_in[0];
    y = pts_in[1];
    float denominator = (perspective_Mat[0][0] * perspective_Mat[1][1] - perspective_Mat[0][0] * perspective_Mat[2][1] * y -
                        perspective_Mat[0][1] * perspective_Mat[1][0] + perspective_Mat[0][1] * perspective_Mat[2][0] * y +
                        perspective_Mat[1][0] * perspective_Mat[2][1] * x - perspective_Mat[1][1] * perspective_Mat[2][0] * x);

    float inv_transformed_x = (perspective_Mat[0][1] * perspective_Mat[1][2] - perspective_Mat[0][1] * perspective_Mat[2][2] * y -
                            perspective_Mat[0][2] * perspective_Mat[1][1] + perspective_Mat[0][2] * perspective_Mat[2][1] * y +
                                perspective_Mat[1][1] * perspective_Mat[2][2] * x - perspective_Mat[1][2] * perspective_Mat[2][1] * x) / denominator;

    float inv_transformed_y = (-perspective_Mat[0][0] * perspective_Mat[1][2] + perspective_Mat[0][0] * perspective_Mat[2][2] * y +
                            perspective_Mat[0][2] * perspective_Mat[1][0] - perspective_Mat[0][2] * perspective_Mat[2][0] * y -
                            perspective_Mat[1][0] * perspective_Mat[2][2] * x + perspective_Mat[1][2] * perspective_Mat[2][0] * x) / denominator;

    int rounded_x = (int)roundf(inv_transformed_x);
    int rounded_y = (int)roundf(inv_transformed_y);

    pts_in[0] = (uint8)fmaxf(0.0f, fminf((float)(MT9V03X_W - 1), (float)rounded_x));
    pts_in[1] = (uint8)fmaxf(0.0f, fminf((float)(MT9V03X_H - 1), (float)rounded_y));
}

/**
 * @brief 自适应阈值计算
 * @param y 行坐标
 * @param x 列坐标
 * @param block 阈值计算窗口大小
 * @param clip_value 阈值修正值
 * @retval 阈值
 */
uint8 adaptiveThreshold(uint8 y,uint8 x,uint8 block,int8 clip_value)
{
    int8 half_block = block / 2;
    if(y < half_block || y > MT9V03X_H - half_block || x < half_block || x > MT9V03X_W - half_block)half_block = 1;
    int16 thres = 0;
    for(int8 dy = -half_block; dy <= half_block; dy++)
    {
        for(int8 dx=-half_block; dx<=half_block; dx++)
        {
            thres += Image[y+dy][x+dx];
        }
    }
    thres = thres / (block*block) - clip_value > 0 ? thres / (block*block) - clip_value : 0;
    return (uint8)thres;
}

/**
 * @brief 沿列向上寻找边缘点
 * @param point 输入点point[0]=x, point[1]=y，输出找到的点写回point
 * @param seed_kind 指定种子类型，left_pts/Right_pts/其它
 */
void seek_up(uint8 point[2], uint8 seed_kind)
{
    uint8 column_x = point[0];
    uint8 start_y = point[1];

    if (column_x < 1 || column_x >= MT9V03X_W - 1 || start_y < 1 || start_y >= MT9V03X_H - 1)
    {
        return;
    }

    uint8 found_y = find_edge_canny_col(column_x, start_y, 1);

    if (found_y > 0)
    {
        if (seed_kind == left_pts)
        {
            Seed_left[0] = column_x;
            Seed_left[1] = found_y;
        }
        else if (seed_kind == Right_pts)
        {
            Seed_right[0] = column_x;
            Seed_right[1] = found_y;
        }
        else
        {
            point[0] = column_x;
            point[1] = found_y;
        }
    }
}

/**
 * @brief 寻找左右种子点
 * @param High 起始行
 * @param mini_high 最小行
 */
void Seek_Pts_Seed(uint8 High, uint8 mini_high)
{
    if(High >= MT9V03X_H || High < 1) {
        Seed_State = no_pts;
        return;
    }

    uint8 result = no_pts;
    Seed_left[0] = 0;
    Seed_left[1] = High;
    Seed_right[0] = MT9V03X_W;
    Seed_right[1] = High;
    uint8 found_left_x = find_edge_canny_row(High, mid_position, 0);
    if (found_left_x >= 3) {
        Seed_left[0] = found_left_x;
        result += left_pts;
    }

    uint8 found_right_x = find_edge_canny_row(High, mid_position, 1);
    if (found_right_x <= MT9V03X_W - 3) {
        Seed_right[0] = found_right_x;
        result += Right_pts;
    }

    if (result != two_pts) {
        if (result == left_pts) { 
            for (uint8 i = High - 1; i >= mini_high; i--) {
                found_right_x = find_edge_canny_row(i, mid_position, 1);
                if (found_right_x <= MT9V03X_W - 3) {
                    Seed_right[0] = found_right_x;
                    Seed_right[1] = i;
                    result += Right_pts;
                    break;
                }
            }
        } else if (result == Right_pts) { 
             for (uint8 i = High - 1; i >= mini_high; i--) {
                found_left_x = find_edge_canny_row(i, mid_position, 0);
                if (found_left_x >= 3) {
                    Seed_left[0] = found_left_x;
                    Seed_left[1] = i;
                    result += left_pts;
                    break;
                }
            }
        }
    }

    Seed_State = result;
    mid_position = (Seed_left[0] + Seed_right[0]) / 2;
    memcpy(original_left, Seed_left, sizeof(Seed_left));
    memcpy(original_right, Seed_right, sizeof(Seed_right));
}

/**
 * @brief 前进方向向量
 * dir_front[0]: 上，dir_front[1]: 右，dir_front[2]: 下，dir_front[3]: 左
 */
int dir_front[4][2]={
        {0,-1},
        {1,0},
        {0,1},
        {-1,0}
};

/**
 * @brief 左前方向向量
 */
int dir_frontleft[4][2]={
        {-1,-1},
        {1,-1},
        {1,1},
        {-1,1}
};

/**
 * @brief 右前方向向量
 */
int dir_frontright[4][2]={
        {1,-1},
        {1,1},
        {-1,1},
        {-1,-1}
};

/**
 * @brief 左手法自适应阈值寻线
 * @param block_size 阈值窗口大小
 * @param clip_value 阈值修正
 * @param pts_arry 输出点数组
 * @retval 实际采集到的点数
 */
uint8 findline_lefthand_adaptive(uint8 block_size,uint8 clip_value,uint8 pts_arry[][2])
{
    uint8 step = 0, dir = 0, turn = 0;
    uint8 thres = 0,time = 0;
    left_threshold = 0;
    while(step < MT9V03X_W - 2 && Seed_left[0] > 2 && Seed_left[0] < (MT9V03X_W - 2) && Seed_left[1] > 2 && Seed_left[1] < (MT9V03X_H - 2) && turn < 4 )
    {
        if(Seed_left[0] == original_right[0] && Seed_left[1] == original_right[1])
        {
            time++;
            if(time > 3)
            {
                step = 0;
                break;
            }
        }
        if(step == 1)
        {
            original_right[0] = Seed_left[0];
            original_right[1] = Seed_left[1];
        }
        if(turn==0){
            thres = adaptiveThreshold(Seed_left[1], Seed_left[0], block_size, clip_value);
            left_threshold += thres;
        }
        int front_value = Image[(Seed_left[1] + dir_front[dir][1])][(Seed_left[0] + dir_front[dir][0])];
        int frontleft_value = Image[(Seed_left[1] + dir_frontleft[dir][1])][(Seed_left[0] + dir_frontleft[dir][0])];

        if(front_value < thres)
        {
            dir = (dir+1) % 4;
            turn++;
        }
        else if(frontleft_value < thres)
        {
            Seed_left[0] += dir_front[dir][0];
            Seed_left[1] += dir_front[dir][1];
            pts_arry[step][0] = Seed_left[0];
            pts_arry[step][1] = Seed_left[1];
            step++;
            turn=0;
        }
        else{
            Seed_left[0] += dir_frontleft[dir][0];
            Seed_left[1] += dir_frontleft[dir][1];
            pts_arry[step][0] = Seed_left[0];
            pts_arry[step][1] = Seed_left[1];
            step++;
            turn=0;
            dir = (dir + 3) % 4;
        }
    }
    if(step > 0)left_threshold /= step;
    else left_threshold = 0;
    return step;
}

/**
 * @brief 右手法自适应阈值寻线
 * @param block_size 阈值窗口大小
 * @param clip_value 阈值修正
 * @param pts_arry 输出点数组
 * @retval 实际采集到的点数
 */
uint8 findline_righthand_adaptive(uint8 block_size,uint8 clip_value,uint8 pts_arry[][2])
{
    uint8 step = 0, dir = 0, turn = 0;
    uint8 thres = 0,time = 0;
    right_threshold = 0;
    while(step < MT9V03X_W-2 && Seed_right[0] > 2 && Seed_right[0] < (MT9V03X_W - 2) && Seed_right[1] > 2 && Seed_right[1] < (MT9V03X_H - 2) && turn <4)
    {
        if(Seed_right[0] == original_left[0] && Seed_right[1] == original_left[1])
        {
            time++;
            if(time > 3)
            {
                step = 0;
                break;
            }
        }
        if(step == 1)
        {
            original_left[0] = Seed_right[0];
            original_left[1] = Seed_right[1];
        }
        if(turn==0){
            thres = adaptiveThreshold(Seed_right[1], Seed_right[0], block_size, clip_value);
            right_threshold += thres;
        }
        int front_value = Image[(Seed_right[1] + dir_front[dir][1])][(Seed_right[0] + dir_front[dir][0])];
        int frontright_value = Image[(Seed_right[1] + dir_frontright[dir][1])][(Seed_right[0] + dir_frontright[dir][0])];

        if(front_value < thres)
        {
            dir = (dir+3) % 4;
            turn++;
        }
        else if(frontright_value < thres)
        {
            Seed_right[0] += dir_front[dir][0];
            Seed_right[1] += dir_front[dir][1];
            pts_arry[step][0] = Seed_right[0];
            pts_arry[step][1] = Seed_right[1];
            step++;
            turn=0;
        }
        else{
            Seed_right[0] += dir_frontright[dir][0];
            Seed_right[1] += dir_frontright[dir][1];
            pts_arry[step][0] = Seed_right[0];
            pts_arry[step][1] = Seed_right[1];
            step++;
            turn=0;
            dir = (dir + 1) % 4;
        }
    }
    if(step > 0)right_threshold /= step;
    else right_threshold = 0;
    return step;
}

/**
 * @brief 对一组点进行透视变换
 * @param pts_in 输入点数组
 * @param pts_out 输出点数组
 * @param step 输入点数
 * @param step_Max 最大输出点数
 * @retval 实际输出点数
 */
uint8 PtsPerspective(uint8 pts_in[][2], uint8 pts_out[][2], uint8 step, uint8 step_Max) {
    uint8 new_step = 0;
    for(uint8 i = 0; i < step; i++) {
        if(CalculateTransform(pts_in[i][1], pts_in[i][0], pts_out[i])) {
            new_step++;
        } else {
            break;
        }
        if(new_step >= step_Max) break;
    }
    return new_step;
}

/**
 * @brief 单点透视变换
 * @param pt_in 输入点pt_in[0]=x, pt_in[1]=y
 * @param pt_out 输出点pt_out[0]=x', pt_out[1]=y'
 * @retval 1: 成功，0: 失败
 */
uint8 TransformSinglePoint(uint8 pt_in[2], uint8 pt_out[2]) {
    return CalculateTransform(pt_in[1], pt_in[0], pt_out);
}

/**
 * @brief 模糊处理后的点数组
 */
uint8 blur_pts_out[255][2];

/**
 * @brief 对点集进行模糊平滑处理
 * @param step 点数
 * @param kernel 卷积核大小
 * @param pts_in 输入点数组
 * @retval 处理后点数
 */
uint8 blur_points(uint8 step, uint8 kernel, uint8 pts_in[][2]){
    int8 half = kernel / 2;
    memset(blur_pts_out, 0, sizeof(blur_pts_out));
    blur_pts_out[0][0] = pts_in[0][0];
    blur_pts_out[0][1] = pts_in[0][1];

    for (int i = 1; i < step - 1; i++) {
        int weight_sum = 0;
        int temp_x = 0;
        int temp_y = 0;

        for (int8 j = -half; j <= half; j++) {
            int valid_index = i + j;

            if (valid_index < 0) valid_index = 0;
            if (valid_index >= step) valid_index = step - 1;

            int weight = half + 1 - abs(j);
            temp_x += pts_in[valid_index][0] * weight;
            temp_y += pts_in[valid_index][1] * weight;
            weight_sum += weight;
        }

        blur_pts_out[i][0] = temp_x / weight_sum;
        blur_pts_out[i][1] = temp_y / weight_sum;
    }

    blur_pts_out[step - 1][0] = pts_in[step - 1][0];
    blur_pts_out[step - 1][1] = pts_in[step - 1][1];
    return Arry_Copy(blur_pts_out,pts_in,step);
}

/**
 * @brief 计算点集某点的局部夹角
 * @param pts_in 点集
 * @param num 点数
 * @param ID 当前点索引
 * @param dist 前后距离
 * @retval 夹角弧度
 */
float local_angle_points(uint8 pts_in[][2], uint8 num, uint8 ID,uint8 dist){
    float dx1 = pts_in[ID][0] - pts_in[clip(ID - dist, 0, num - 1)][0];
    float dy1 = pts_in[ID][1] - pts_in[clip(ID - dist, 0, num - 1)][1];
    float dn1 = SquareRootFloat(dx1 * dx1 + dy1 * dy1);
    float dx2 = pts_in[clip(ID + dist, 0, num - 1)][0] - pts_in[ID][0];
    float dy2 = pts_in[clip(ID + dist, 0, num - 1)][1] - pts_in[ID][1];
    float dn2 = SquareRootFloat(dx2 * dx2 + dy2 * dy2);
    float c1 = dx1 / dn1;
    float s1 = dy1 / dn1;
    float c2 = dx2 / dn2;
    float s2 = dy2 / dn2;
    return atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);
}

/**
 * @brief 非极大值抑制处理角度数组
 * @param angle_in 输入角度数组
 * @param num 数组长度
 * @param angle_out 输出数组
 * @param kernel 抑制窗口
 */
void nms_angle(float angle_in[], int num, float angle_out[], int kernel){
    int half = kernel / 2;
    for (int i = 0; i < num; i++) {
        angle_out[i] = angle_in[i];
        for (int j = -half; j <= half; j++) {
            if (fabs(angle_in[clip(i + j, 0, num - 1)]) > fabs(angle_out[i])) {
                angle_out[i] = 0;
                break;
            }
        }
    }
}

/**
 * @brief 计算三点曲率
 * @param P0 第一个点
 * @param P1 第二个点
 * @param P2 第三个点
 * @retval 曲率值
 */
float compute_curvature(uint8 P0[2], uint8 P1[2], uint8 P2[2]) {
    float cross =
        (float)(P1[0] - P0[0]) * (float)(P2[1] - P0[1]) -
        (float)(P2[0] - P0[0]) * (float)(P1[1] - P0[1]);

    float area = 0.5 * fabs(cross);

    if (area < 1e-8) {
        return 0.0;
    }

    float dx, dy;

    dx = (float)(P2[0] - P1[0]);
    dy = (float)(P2[1] - P1[1]);
    float a = sqrt(dx * dx + dy * dy);

    dx = (float)(P2[0] - P0[0]);
    dy = (float)(P2[1] - P0[1]);
    float b = sqrt(dx * dx + dy * dy);

    dx = (float)(P1[0] - P0[0]);
    dy = (float)(P1[1] - P0[1]);
    float c = sqrt(dx * dx + dy * dy);

    float radius = (a * b * c) / (4 * area);

    return (radius != 0) ? 1.0 / radius : 0.0;
}

/**
 * @brief 获取点集的最小y值
 * @param pts_in 点集
 * @param num 点数
 * @retval 最小y值
 */
uint8 Get_high(uint8 pts_in[][2],uint8 num)
{
    uint8 high = Start_high;
    for(uint8 i = 0; i < num; i++)
    {
        if(pts_in[i][1] < high)high = pts_in[i][1];
    }
    return high;
}

/**
 * @brief 计算两点欧氏距离
 * @param pt1 第一个点
 * @param pt2 第二个点
 * @retval 距离
 */
float Get_Distance(uint8 pt1[2], uint8 pt2[2]) {
    float dx = (int)pt1[0] - pt2[0];
    float dy = (int)pt1[1] - pt2[1];
    return SquareRootFloat(dx*dx + dy*dy);
}

/**
 * @brief 对图像某点进行3x3高斯模糊
 * @param x 列坐标
 * @param y 行坐标
 */
void blur(uint8 x,uint8 y)
{
    if(x > 1 && x < 187)
    {
        Image[y][x]=(1 * Image[y - 1][x-1] + 2 * Image[y - 1][x] + 1 * Image[y - 1][x+1] +
                                2 * Image[y][x-1] + 4 * Image[y][x] + 2 * Image[y][x+1] +
                                1 * Image[y+1][x-1] + 2 * Image[y+1][x] + 1 * Image[y+1][x+1]) / 16;
    }
}

/**
 * @brief 一维Otsu阈值法
 * @param data 输入数据
 * @param len 数据长度
 * @retval 阈值
 */
static uint8 Otsu_Threshold_1D(uint8* data, uint16_t len)
{
    if (data == NULL || len == 0) {
        return 0;
    }

    #define GrayScale 256
    uint32 HistGram[GrayScale] = {0};
    for (uint16_t i = 0; i < len; i++) {
        HistGram[data[i]]++;
    }

    uint8 MinValue = 0, MaxValue = 255;
    while (MinValue < 255 && HistGram[MinValue] == 0) MinValue++;
    while (MaxValue > MinValue && HistGram[MaxValue] == 0) MaxValue--;

    if (MaxValue == MinValue) {
        return MaxValue;
    }
    if (MinValue + 1 >= MaxValue) {
        return MinValue;
    }

    uint32 Amount = 0;
    for (uint16_t i = MinValue; i <= MaxValue; i++) {
        Amount += HistGram[i];
    }
    if (Amount == 0) {
        return 0;
    }

    uint32 PixelIntegral = 0;
    for (uint16_t i = MinValue; i <= MaxValue; i++) {
        PixelIntegral += HistGram[i] * i;
    }

    double SigmaB = -1;
    uint8 Threshold = 0;
    uint32 PixelBack = 0;
    uint32 PixelIntegralBack = 0;

    for (uint16_t i = MinValue; i < MaxValue; i++) {
        PixelBack += HistGram[i];
        if (PixelBack == 0) continue;

        uint32 PixelFore = Amount - PixelBack;
        if (PixelFore == 0) break;

        PixelIntegralBack += HistGram[i] * i;
        uint32 PixelIntegralFore = PixelIntegral - PixelIntegralBack;

        double OmegaBack = (double)PixelBack / Amount;
        double OmegaFore = (double)PixelFore / Amount;
        double MicroBack = (double)PixelIntegralBack / PixelBack;
        double MicroFore = (double)PixelIntegralFore / PixelFore;
        double Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);

        if (Sigma > SigmaB) {
            SigmaB = Sigma;
            Threshold = (uint8)i;
        }
    }

    return Threshold;
}

/**
 * @brief 沿行方向Canny边缘检测，寻找边缘点
 * @param row 行号
 * @param start_x 起始列
 * @param direction 方向，1为向右，0为向左
 * @retval 找到的边缘点x坐标，未找到返回0
 */
uint8 find_edge_canny_row(uint8 row, uint8 start_x, uint8 direction)
{
    if (row < 1 || row >= MT9V03X_H - 1 || start_x >= MT9V03X_W)
    {
        return 0;
    }

    uint8 grad_mag[MT9V03X_W] = {0};
    for (uint16_t x = 1; x < MT9V03X_W - 1; x++)
    {
        int32_t gx = (Image[row - 1][x + 1] + 2 * Image[row][x + 1] + Image[row + 1][x + 1]) -
                     (Image[row - 1][x - 1] + 2 * Image[row][x - 1] + Image[row + 1][x - 1]);

        int32_t gy = (Image[row + 1][x - 1] + 2 * Image[row + 1][x] + Image[row + 1][x + 1]) -
                     (Image[row - 1][x - 1] + 2 * Image[row - 1][x] + Image[row - 1][x + 1]);

        uint32_t magnitude = abs(gx) + abs(gy);
        grad_mag[x] = (uint8)clip(magnitude, 0, 255);
    }

    uint8 TH = Otsu_Threshold_1D(grad_mag, MT9V03X_W);

    if (direction == 1)
    {
        uint16_t start_pos = (start_x > 1) ? start_x : 1;
        for (uint16_t x = start_pos; x < MT9V03X_W - 1; x++)
        {
            if (grad_mag[x] > TH &&
                grad_mag[x] > grad_mag[x - 1])
            {
                return (uint8)x;
            }
        }
    }
    else
    { 
        int16_t start_pos = (start_x < MT9V03X_W - 2) ? start_x : (MT9V03X_W - 2);
        for (int16_t x = start_pos; x >= 1; x--)
        {
            if (grad_mag[x] > TH &&
                grad_mag[x] > grad_mag[x + 1])
            {
                return (uint8)x;
            }
        }
    }

    return 0;
}

/**
 * @brief 沿列方向Canny边缘检测，寻找边缘点
 * @param col 列号
 * @param start_y 起始行
 * @param direction 方向，0为向下，1为向上
 * @retval 找到的边缘点y坐标，未找到返回0
 */
uint8 find_edge_canny_col(uint8 col, uint8 start_y, uint8 direction)
{
    if (col < 1 || col >= MT9V03X_W - 1 || start_y >= MT9V03X_H)
    {
        return 0;
    }

    uint8 grad_mag[MT9V03X_H] = {0};
    
    for (uint16_t y = 1; y < MT9V03X_H - 2; y++)
    {
        int32_t gx = (Image[y - 1][col + 1] + 2 * Image[y][col + 1] + Image[y + 1][col + 1]) -
                     (Image[y - 1][col - 1] + 2 * Image[y][col - 1] + Image[y + 1][col - 1]);

        int32_t gy = (Image[y + 1][col - 1] + 2 * Image[y + 1][col] + Image[y + 1][col + 1]) -
                     (Image[y - 1][col - 1] + 2 * Image[y - 1][col] + Image[y - 1][col + 1]);

        uint32_t magnitude = abs(gx) + abs(gy);
        grad_mag[y] = (uint8)clip(magnitude, 0, 255);
    }

    uint8 TH = Otsu_Threshold_1D(grad_mag, MT9V03X_H) / 2;

    if (direction == 0)
    {
        uint16_t start_pos = (start_y > 1) ? start_y : 1;
        start_pos = (start_pos < MT9V03X_H - 2) ? start_pos : (MT9V03X_H - 2);
        
        for (uint16_t y = start_pos; y < MT9V03X_H - 1; y++)
        {
            if (grad_mag[y] > TH &&
                grad_mag[y] > grad_mag[y + 1])
            {
                return (uint8)y;
            }
        }
    }
    else
    { 

        int16_t start_pos = (start_y < MT9V03X_H - 2) ? start_y : (MT9V03X_H - 2);
        start_pos = (start_pos > 1) ? start_pos : 1;
        
        for (int16_t y = start_pos; y >= 1; y--)
        {
            if (grad_mag[y] > TH &&
                grad_mag[y] > grad_mag[y - 1])
            {
                return (uint8)y;
            }
        }
    }

    return 0;
}
