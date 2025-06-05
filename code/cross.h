#ifndef CROSS_H
#define CROSS_H

#include "zf_common_typedef.h"
#include "stdbool.h"
#include "PTS_Deal.h"

typedef enum
{
    no_cross,        // 无交叉口
    mid_cross,       // 中间交叉口
}cross_kind;

typedef enum
{
    cross_state1,    
    cross_state2,      
}cross_state;


// 环岛控制状态结构体
typedef struct {
    cross_kind kind;         // 交叉口类型
    cross_state state;            // 交叉口状态
    uint8 left_x;
    uint8 left_y;
    uint8 right_x;
    uint8 right_y;
    uint8 phase_counter;    // 阶段计数器
    uint32 sum_distance;       // 累计距离
    uint8 flag;            // 标志位

} CrossState;


#include "zf_common_headfile.h"

#define CROSS_single_high 10
#define CROSS_single_width 10
#define MIN_Point 30
#define Cross_Count_Threshold 3
#define CROSS_OFFSET 10

extern CrossState cross_ctrl;

void cross_process(void);

#endif
