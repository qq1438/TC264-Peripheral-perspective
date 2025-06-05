#ifndef _Ring_h_
#define _Ring_h_

#include "zf_common_typedef.h"
#include "stdbool.h"
#include "PTS_Deal.h"

// 环岛状态定义
typedef enum
{
    Ring_state1,    // 检测环岛入口
    Ring_state2,    // 确认进入环岛
    Ring_state3,    // 环岛内部处理阶段1
    Ring_state4,    // 环岛内部处理阶段2
    Ring_state5,    // 环岛出口处理
    Ring_state6,    // 环岛结束处理阶段1
    Ring_state7,    // 环岛结束处理阶段2
    Ring_state8,    // 环岛完退出
    Ring_state9,    // 环岛完全退出
}ring_state;

// 环岛类型定义
typedef enum
{
    no_ring,        // 无环岛
    left_ring,      // 左环岛
    right_ring,     // 右环岛
}ring_kind;

// 环岛控制状态结构体
typedef struct {
    ring_state state;       // 当前环岛状态
    ring_kind kind;         // 环岛类型
    uint8 phase_counter;    // 阶段计数器
    float sum_zangle;       // 累计角度
    uint8 last_high;
    uint32 time_counter;  // 时间计数器
    uint8 flag;            // 标志位
} RingState;

// 新增初始化结构体
typedef struct {
    bool is_left;
    struct {
        BoundaryData *current;
        BoundaryData *opposite;
        FBoundaryData *Fcurrent;
        FBoundaryData *Fopposite;
    } bounds;
} RingContext;


#include "zf_common_headfile.h"

// 常量定义
#define MIN_POINTS 20
#define PHASE_COUNT_THRESHOLD 3
#define ANGLE_THRESHOLD_LARGE 300.0f
#define ANGLE_THRESHOLD_MEDIUM 45.0f
#define ANGLE_THRESHOLD_SMALL 30.0f
#define WIDTH_THRESHOLD (Step_Max >> 1)
#define WIDTH_THRESHOLD_SMALL (Step_Max / 3)

extern RingState ring_ctrl;
extern uint8 IDID;

// 函数声明
void Ring_Process(void);
void Ring_evolve(void);
void Ring_Out(void);
void Ring_end(void);
#endif
