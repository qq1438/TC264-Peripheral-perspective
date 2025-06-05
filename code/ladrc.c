#include "ladrc.h"

/**
 * @brief 初始化LADRC控制器
 * @param ladrc LADRC控制器结构体指针
 * @param Ts 采样周期
 * @param limit 输出限幅值
 */
void LADRC_Init(LADRC *ladrc, float Ts)
{
    ladrc->wc = 0;
    ladrc->w0 = 0;
    ladrc->b = 0;
    ladrc->Ts = Ts;
    ladrc->limit = 0;
    
    ladrc->z1 = 0.0f;
    ladrc->z2 = 0.0f;
    ladrc->target = 0.0f;
    ladrc->last_u = 0.0f;
}

/**
 * @brief 设置LADRC控制器目标值
 * @param ladrc LADRC控制器结构体指针
 * @param target 目标值
 */
void LADRC_SetTarget(LADRC *ladrc, float target)
{
    ladrc->target = target;
}

/**
 * @brief LADRC控制器更新函数
 * @param ladrc LADRC控制器结构体指针
 * @param y 系统输出测量值
 * @return 控制输出u
 */
float LADRC_Update(LADRC *ladrc, float y)
{
    float wc = ladrc->wc;
    float w0 = ladrc->w0;
    float b = ladrc->b;
    float Ts = ladrc->Ts;
    
    // 扩张状态观测器（ESO）
    // 观测器误差
    float e = y - ladrc->z1;
    
    // ESO更新方程（离散化）
    // z1(k+1) = z1(k) + Ts * (z2(k) + 2*w0*e + b*u(k))
    // z2(k+1) = z2(k) + Ts * (w0^2 * e)
    float z1_new = ladrc->z1 + Ts * (ladrc->z2 + 2.0f * w0 * e + b * ladrc->last_u);
    float z2_new = ladrc->z2 + Ts * (w0 * w0 * e);
    
    // 更新ESO状态
    ladrc->z1 = z1_new;
    ladrc->z2 = z2_new;
    
    // 线性状态反馈控制律
    // u0 = wc * (r - z1)
    float u0 = wc * (ladrc->target - ladrc->z1);
    
    // 扰动补偿
    // u = (u0 - z2) / b
    float u = (u0 - ladrc->z2) / b;
    
    // 输出限幅
    if (u > ladrc->limit)
    {
        u = ladrc->limit;
    }
    else if (u < -ladrc->limit)
    {
        u = -ladrc->limit;
    }
    
    // 保存控制输出供下次ESO更新使用
    ladrc->last_u = u;
    
    return u;
}

/**
 * @brief 重置LADRC控制器状态
 * @param ladrc LADRC控制器结构体指针
 */
void LADRC_Reset(LADRC *ladrc)
{
    ladrc->z1 = 0.0f;
    ladrc->z2 = 0.0f;
    ladrc->last_u = 0.0f;
} 
