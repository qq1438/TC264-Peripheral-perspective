#ifndef _LADRC_H_
#define _LADRC_H_

typedef struct 
{
    // ����������
    float wc;         // ����������
    float w0;         // �۲�������
    float b;          // ��������
    
    // ����״̬�۲���(ESO)״̬����
    float z1;         // ϵͳ�������ֵ
    float z2;         // ϵͳ״̬���Ŷ�����ֵ
    
    // ����ʱ��
    float Ts;         // ��������
    
    // Ŀ��ֵ������޷�
    float target;     // Ŀ��ֵ (r)
    float limit;      // ����޷�
    
    // ��һ�εĿ������
    float last_u;     // ��һ�εĿ������
} LADRC;

#include "zf_common_headfile.h"

// ��������
void LADRC_Init(LADRC *ladrc,float Ts);
void LADRC_SetTarget(LADRC *ladrc, float target);
float LADRC_Update(LADRC *ladrc, float y);
void LADRC_Reset(LADRC *ladrc);

#endif // _LADRC_H_ 
