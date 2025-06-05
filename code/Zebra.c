#include "PTS_Deal.h"

bool zebra = false;

static bool judge_time(int position, int target);

/**
 * @brief 斑马线检测函数，根据速度和阈值判定是否检测到斑马线
 * @param target 边缘变化次数阈值，当变化次数大于该值时，认为检测到斑马线
 */
void zebra_judge(int target)
{
    uint8 point[2] = {94,Start_high};
    TransformSinglePoint(point,point);
    point[1] = point[1] - 10;
    ImagePerspective(point);
    int base_high = point[1];
    int offsets[3] = {0, -5, 5};
    for(int i = 0; i < 3; i++) {
        if(judge_time(base_high + offsets[i], target)) {
            zebra = true;
            break;
        }
    }
}

/**
 * @brief 判断指定位置是否满足斑马线判定条件
 * @param position 图像垂直位置（对应边界点的索引）
 * @param target 边缘变化次数阈值
 * @return 如果满足条件返回 true，否则返回 false
 */
bool judge_time(int position, int target)
{
    uint8 left_x = 0;
    uint8 right_x = 0;
    uint8 count = 0;
    bool now_state = false, last_state = false;
    
    // 找到该位置对应的左边界点 - 找最接近且小于等于position的点
    for(uint8 i = 0; i < left_boundary.original_step; i++) {
        if(left_boundary.original_pts[i][1] <= position) {
            left_x = left_boundary.original_pts[i][0];
            break;
        }
    }
    
    // 找到该位置对应的右边界点 - 找最接近且小于等于position的点
    for(uint8 i = 0; i < right_boundary.original_step; i++) {
        if(right_boundary.original_pts[i][1] <= position) {
            right_x = right_boundary.original_pts[i][0];
            break;
        }
    }
    
    // 边界安全检查
    if(left_x >= right_x || left_x >= MT9V03X_W || right_x >= MT9V03X_W || position < 0 || position >= MT9V03X_H || left_x == 0 || right_x == 0) {
        return false;
    }
    
    // 设置初始状态为第一个像素的状态
    if(Image[position][left_x] > low_threshold) {
        last_state = true;
    } else {
        last_state = false;
    }
    
    // 从第二个像素开始计算边缘变化次数
    for(uint8 i = left_x + 1; i <= right_x; i++) {
        if(Image[position][i] > low_threshold) {
            now_state = true;
        } else {
            now_state = false;
        }
        
        // 检测到状态变化时计数加1
        if(now_state != last_state) {
            count++;
        }
        last_state = now_state;
    }
    return (count > target);
}
