#ifndef _LADRC_H_
#define _LADRC_H_

typedef struct 
{
    // 控制器参数
    float wc;         // 控制器带宽
    float w0;         // 观测器带宽
    float b;          // 控制增益
    
    // 扩张状态观测器(ESO)状态变量
    float z1;         // 系统输出估计值
    float z2;         // 系统状态及扰动估计值
    
    // 采样时间
    float Ts;         // 采样周期
    
    // 目标值和输出限幅
    float target;     // 目标值 (r)
    float limit;      // 输出限幅
    
    // 上一次的控制输出
    float last_u;     // 上一次的控制输出
} LADRC;

#include "zf_common_headfile.h"

// 函数声明
void LADRC_Init(LADRC *ladrc,float Ts);
void LADRC_SetTarget(LADRC *ladrc, float target);
float LADRC_Update(LADRC *ladrc, float y);
void LADRC_Reset(LADRC *ladrc);

#endif // _LADRC_H_ 
