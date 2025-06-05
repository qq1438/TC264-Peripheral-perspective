#include "PID.h"
#include "clip.h"
#include <math.h>

/**
 * @brief ��ʼ��PID������
 * @param PIDX PID�������ṹ��ָ��
 * @param KP ����ϵ��
 * @param KI ����ϵ��
 * @param KD ΢��ϵ��
 * @param limit ����޷�ֵ
 */
void PID_Init(PID *PIDX, float e_ration,float de_ration)
{
    PIDX->KP = 0;
    PIDX->KI = 0;
    PIDX->KD = 0;
    PIDX->limit = 0;
    PIDX->last_error = 0.0f;    // ��ʼ���ϴ����Ϊ0
    PIDX->last_output = 0.0f;   // ��ʼ���ϴ����Ϊ0
    PIDX->target = 0.0f;        // ��ʼ��Ŀ��ֵΪ0
    PIDX->llast_error = 0.0f;
    PIDX->Integral_error = 0.0f;
    PIDX->e_ration = e_ration;
    PIDX->de_ration = de_ration;
}

/**
 * @brief λ��ʽPID����
 * @param PIDX PID�������ṹ��ָ��
 * @param Value ��ǰ����ֵ
 * @return �����޷���Ŀ������
 */
float PID_Position(PID *PIDX, float Value)
{
    float error = PIDX->target - Value;         // ���㵱ǰ���
    float derror = error - PIDX->last_error;    // �������仯��
    PIDX->Integral_error += error;
    if(PIDX->Integral_error > 10000)PIDX->Integral_error = 10000;
    if(error < 100)PIDX->Integral_error = 0;
    // ����PID����������� + ΢����
    float output = PIDX->KP * error + 
                  PIDX->KD * derror + PIDX->Integral_error * PIDX->KI;
    
    PIDX->last_error = error;  // �����ϴ����
    
    // ���ؾ����޷�������
    return fclip(output, -PIDX->limit, PIDX->limit);
}

/**
 * @brief ����ʽPID����
 * @param PIDX PID�������ṹ��ָ��
 * @param Value ��ǰ����ֵ
 * @return �����޷���Ŀ������
 */
float PID_Incream(PID *PIDX, float Value)
{
    float error = PIDX->target - Value;         // ���㵱ǰ���
    float derror = error - PIDX->last_error;    // �������仯��
    // ����PID����������� + ������ + �ϴ����
    float output = PIDX->KP * derror +  PIDX->KI * error + PIDX->KD * (error - 2 * PIDX->last_error + PIDX->llast_error) +PIDX->last_output;
    
    output = fclip(output, -PIDX->limit, PIDX->limit);

    PIDX->last_error = error;   // �����ϴ����
    PIDX->last_output = output; // �����ϴ����
    PIDX->llast_error = PIDX->last_error;
    // ���ؾ����޷�������
    return output;
}

int PID_Incream_IMU(PID *PIDX, float Value)
{
    float accz = imu660ra_acc_z - 4092 > 0 ? imu660ra_acc_z - 4092 : 0;
    float error = PIDX->target - Value;         // ���㵱ǰ���
    float derror = error - PIDX->last_error;    // �������仯��
    float output = PIDX->KP * derror +  PIDX->KI * error - PIDX->KD * fabs(accz) +PIDX->last_output;
    output = fclip(output, -PIDX->limit, PIDX->limit);
    PIDX->last_error = error;   // �����ϴ����
    PIDX->last_output = output; // �����ϴ����
    PIDX->llast_error = PIDX->last_error;
    // ���ؾ����޷�������
    return (int)output;
}

int angle_pid(PID *PIDX,float Value){
    float error = PIDX->target - Value;         // ���㵱ǰ���
    float derror = error - PIDX->last_error;    // �������仯��
    int out1 = error * PIDX->KP -  imu660ra_gyro_z * PIDX->KD;
    return clip(out1 , -PIDX->limit, PIDX->limit);
}

#define LIMIT          1
#define FUZZY_SEG_NUMS 7
#define seg_len        3
#define num_area       7

// ģ�����Ƴ�������
#define NB -3  // ����
#define NM -2  // ����
#define NS -1  // ��С
#define ZO 0   // ��
#define PS 1   // ��С
#define PM 2   // ����
#define PB 3   // ����

// ����ά������Ϊһά�����ֻ����������ģ������
float Kp_rule_list[FUZZY_SEG_NUMS] = {PB, PM, PS, ZO, NS, NM, NB};  // Kp��ģ�������

// ���������䶨��
int interval[FUZZY_SEG_NUMS] = {9, 6, 3, 0, -3, -6, -9};

// ���ڵ��Ե�ȫ�ֱ���
float v_error = 0;
float v_derror = 0;

// ģ�������� - ��������ֵ��������
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

// �򻯺��һάģ������Ӧ�ú���
void fuzzy_rule_1D(const float percent_e[FUZZY_SEG_NUMS], const float rule[FUZZY_SEG_NUMS], float percent_out[])
{
    for (int i = 0; i < FUZZY_SEG_NUMS; i++)
    {
        if (percent_e[i] == 0)
            continue;
        // ����Χ��-3~3ת��Ϊ0~6������
        int index = (int)(rule[i] + 3);
        if (index >= 0 && index < FUZZY_SEG_NUMS) { // ȷ����������Ч��Χ��
            percent_out[index] += percent_e[i];
        }
    }
}

// ���Ľ�ģ��������
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

// һάģ��PID���ƺ���
int vague_speed_PID_1D(PID *pid, int value)
{
    float kp = pid->KP;
    float kd = pid->KD; // ����ԭʼKdֵ
    float error = (pid->target - value);
    
    float percent_e[FUZZY_SEG_NUMS] = {0};
    float percent_kp[FUZZY_SEG_NUMS] = {0};
    float percent_kd[FUZZY_SEG_NUMS] = {0};
    
    fuzzification(error, pid->e_ration, percent_e);
    
    // Ӧ��һάģ������Kp
    fuzzy_rule_1D(percent_e, Kp_rule_list, percent_kp);
    
    // ��ģ�������õ��������Kp��Kd
    pid->KP = fabsf(de_fuzzification_centroid(pid->KP, percent_kp));
    pid->KD = kd / (pid->KP + 1);
    
    // ʹ�õ������Kp��Kd����PID���
    int output = angle_pid(pid, value);
    
    // �ָ�ԭʼ����ֵ
    pid->KP = kp;
    pid->KD = kd;
    pid->last_error = error;
    
    return output;
}

// ģ��PID������ں���
int vague_speed_PID(PID *pid, int value)
{
    // ����һάģ��PID
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





