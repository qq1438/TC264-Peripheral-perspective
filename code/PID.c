#include "PID.h"
#include "clip.h"
#include <math.h>

/**
 * @brief 初始化PID控制器
 * @param PIDX PID控制器结构体指针
 * @param KP 比例系数
 * @param KI 积分系数
 * @param KD 微分系数
 * @param limit 输出限幅值
 */
void PID_Init(PID *PIDX, float e_ration,float de_ration)
{
    PIDX->KP = 0;
    PIDX->KI = 0;
    PIDX->KD = 0;
    PIDX->limit = 0;
    PIDX->last_error = 0.0f;    // 初始化上次误差为0
    PIDX->last_output = 0.0f;   // 初始化上次输出为0
    PIDX->target = 0.0f;        // 初始化目标值为0
    PIDX->llast_error = 0.0f;
    PIDX->Integral_error = 0.0f;
    PIDX->e_ration = e_ration;
    PIDX->de_ration = de_ration;
}

/**
 * @brief 位置式PID控制
 * @param PIDX PID控制器结构体指针
 * @param Value 当前测量值
 * @return 经过限幅后的控制输出
 */
float PID_Position(PID *PIDX, float Value)
{
    float error = PIDX->target - Value;         // 计算当前误差
    float derror = error - PIDX->last_error;    // 计算误差变化率
    PIDX->Integral_error += error;
    if(PIDX->Integral_error > 10000)PIDX->Integral_error = 10000;
    if(error < 100)PIDX->Integral_error = 0;
    // 计算PID输出：比例项 + 微分项
    float output = PIDX->KP * error + 
                  PIDX->KD * derror + PIDX->Integral_error * PIDX->KI;
    
    PIDX->last_error = error;  // 更新上次误差
    
    // 返回经过限幅后的输出
    return fclip(output, -PIDX->limit, PIDX->limit);
}

/**
 * @brief 增量式PID控制
 * @param PIDX PID控制器结构体指针
 * @param Value 当前测量值
 * @return 经过限幅后的控制输出
 */
float PID_Incream(PID *PIDX, float Value)
{
    float error = PIDX->target - Value;         // 计算当前误差
    float derror = error - PIDX->last_error;    // 计算误差变化率
    // 计算PID输出：比例项 + 积分项 + 上次输出
    float output = PIDX->KP * derror +  PIDX->KI * error + PIDX->KD * (error - 2 * PIDX->last_error + PIDX->llast_error) +PIDX->last_output;
    
    output = fclip(output, -PIDX->limit, PIDX->limit);

    PIDX->last_error = error;   // 更新上次误差
    PIDX->last_output = output; // 更新上次输出
    PIDX->llast_error = PIDX->last_error;
    // 返回经过限幅后的输出
    return output;
}

int PID_Incream_IMU(PID *PIDX, float Value)
{
    float accz = imu660ra_acc_z - 4092 > 0 ? imu660ra_acc_z - 4092 : 0;
    float error = PIDX->target - Value;         // 计算当前误差
    float derror = error - PIDX->last_error;    // 计算误差变化率
    float output = PIDX->KP * derror +  PIDX->KI * error - PIDX->KD * fabs(accz) +PIDX->last_output;
    output = fclip(output, -PIDX->limit, PIDX->limit);
    PIDX->last_error = error;   // 更新上次误差
    PIDX->last_output = output; // 更新上次输出
    PIDX->llast_error = PIDX->last_error;
    // 返回经过限幅后的输出
    return (int)output;
}

int angle_pid(PID *PIDX,float Value){
    float error = PIDX->target - Value;         // 计算当前误差
    float derror = error - PIDX->last_error;    // 计算误差变化率
    int out1 = error * PIDX->KP -  imu660ra_gyro_z * PIDX->KD;
    return clip(out1 , -PIDX->limit, PIDX->limit);
}

#define LIMIT          1
#define FUZZY_SEG_NUMS 7
#define seg_len        3
#define num_area       7

// 模糊控制常量定义
#define NB -3  // 负大
#define NM -2  // 负中
#define NS -1  // 负小
#define ZO 0   // 零
#define PS 1   // 正小
#define PM 2   // 正中
#define PB 3   // 正大

// 将二维规则表改为一维规则表，只根据误差进行模糊控制
float Kp_rule_list[FUZZY_SEG_NUMS] = {PB, PM, PS, ZO, NS, NM, NB};  // Kp的模糊规则表

// 隶属度区间定义
int interval[FUZZY_SEG_NUMS] = {9, 6, 3, 0, -3, -6, -9};

// 用于调试的全局变量
float v_error = 0;
float v_derror = 0;

// 模糊化函数 - 计算输入值的隶属度
void fuzzification(float val, float ration, float percent[FUZZY_SEG_NUMS])
{
    val *= ration;
    if (val <= interval[0])
    {
        percent[0] = 1;
        return;
    }
    else if (val <= interval[1])
    {
        percent[1] = (val - interval[0]) / seg_len;
        percent[0] = 1 - percent[1];
        return;
    }
    else if (val <= interval[2])
    {
        percent[2] = (val - interval[1]) / seg_len;
        percent[1] = 1 - percent[2];
        return;
    }
    else if (val <= interval[3])
    {
        percent[3] = (val - interval[2]) / seg_len;
        percent[2] = 1 - percent[3];
        return;
    }
    else if (val <= interval[4])
    {
        percent[4] = (val - interval[3]) / seg_len;
        percent[3] = 1 - percent[4];
        return;
    }
    else if (val <= interval[5])
    {
        percent[5] = (val - interval[4]) / seg_len;
        percent[4] = 1 - percent[5];
        return;
    }
    else if (val <= interval[6])
    {
        percent[6] = (val - interval[5]) / seg_len;
        percent[5] = 1 - percent[6];
        return;
    }
    else
    {
        percent[6] = 1;
        return;
    }
}

// 简化后的一维模糊规则应用函数
void fuzzy_rule_1D(const float percent_e[FUZZY_SEG_NUMS], const float rule[FUZZY_SEG_NUMS], float percent_out[])
{
    for (int i = 0; i < FUZZY_SEG_NUMS; i++)
    {
        if (percent_e[i] == 0)
            continue;
        // 将范围从-3~3转换为0~6的索引
        int index = (int)(rule[i] + 3);
        if (index >= 0 && index < FUZZY_SEG_NUMS) { // 确保索引在有效范围内
            percent_out[index] += percent_e[i];
        }
    }
}

// 离心解模糊化函数
float de_fuzzification_centroid(float max, float percent[FUZZY_SEG_NUMS])
{
    float weighted_sum = 0;
    float total_membership = 0;
    float PID_interval[FUZZY_SEG_NUMS];
    float seg = 2 * max / (FUZZY_SEG_NUMS - 1);
    
    for (int i = 0; i < FUZZY_SEG_NUMS; i++) {
        PID_interval[i] = -max + i * seg;
        
        weighted_sum += percent[i] * PID_interval[i];
        total_membership += percent[i];
    }
    
    if (total_membership == 0)
        return 0;
    
    return weighted_sum / total_membership;
}

// 一维模糊PID控制函数
int vague_speed_PID_1D(PID *pid, int value)
{
    float kp = pid->KP;
    float kd = pid->KD; // 保存原始Kd值
    float error = (pid->target - value);
    
    float percent_e[FUZZY_SEG_NUMS] = {0};
    float percent_kp[FUZZY_SEG_NUMS] = {0};
    float percent_kd[FUZZY_SEG_NUMS] = {0};
    
    fuzzification(error, pid->e_ration, percent_e);
    
    // 应用一维模糊规则到Kp
    fuzzy_rule_1D(percent_e, Kp_rule_list, percent_kp);
    
    // 解模糊化，得到调整后的Kp和Kd
    pid->KP = fabsf(de_fuzzification_centroid(pid->KP, percent_kp));
    pid->KD = kd / (pid->KP + 1);
    
    // 使用调整后的Kp和Kd计算PID输出
    int output = angle_pid(pid, value);
    
    // 恢复原始参数值
    pid->KP = kp;
    pid->KD = kd;
    pid->last_error = error;
    
    return output;
}

// 模糊PID控制入口函数
int vague_speed_PID(PID *pid, int value)
{
    // 调用一维模糊PID
    return vague_speed_PID_1D(pid, value);
}

void PID_dtarget(PID *pid,float MAX_target,float target,float now_value,uint16 step)
{
    if((target - now_value) > MAX_target && (target - pid->target) > MAX_target)
    {
        pid->target += target / step;
    }
    else
    {
        pid->target = target;
    }
    
}





