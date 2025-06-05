#include "ladrc.h"

/**
 * @brief ��ʼ��LADRC������
 * @param ladrc LADRC�������ṹ��ָ��
 * @param Ts ��������
 * @param limit ����޷�ֵ
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
 * @brief ����LADRC������Ŀ��ֵ
 * @param ladrc LADRC�������ṹ��ָ��
 * @param target Ŀ��ֵ
 */
void LADRC_SetTarget(LADRC *ladrc, float target)
{
    ladrc->target = target;
}

/**
 * @brief LADRC���������º���
 * @param ladrc LADRC�������ṹ��ָ��
 * @param y ϵͳ�������ֵ
 * @return �������u
 */
float LADRC_Update(LADRC *ladrc, float y)
{
    float wc = ladrc->wc;
    float w0 = ladrc->w0;
    float b = ladrc->b;
    float Ts = ladrc->Ts;
    
    // ����״̬�۲�����ESO��
    // �۲������
    float e = y - ladrc->z1;
    
    // ESO���·��̣���ɢ����
    // z1(k+1) = z1(k) + Ts * (z2(k) + 2*w0*e + b*u(k))
    // z2(k+1) = z2(k) + Ts * (w0^2 * e)
    float z1_new = ladrc->z1 + Ts * (ladrc->z2 + 2.0f * w0 * e + b * ladrc->last_u);
    float z2_new = ladrc->z2 + Ts * (w0 * w0 * e);
    
    // ����ESO״̬
    ladrc->z1 = z1_new;
    ladrc->z2 = z2_new;
    
    // ����״̬����������
    // u0 = wc * (r - z1)
    float u0 = wc * (ladrc->target - ladrc->z1);
    
    // �Ŷ�����
    // u = (u0 - z2) / b
    float u = (u0 - ladrc->z2) / b;
    
    // ����޷�
    if (u > ladrc->limit)
    {
        u = ladrc->limit;
    }
    else if (u < -ladrc->limit)
    {
        u = -ladrc->limit;
    }
    
    // �������������´�ESO����ʹ��
    ladrc->last_u = u;
    
    return u;
}

/**
 * @brief ����LADRC������״̬
 * @param ladrc LADRC�������ṹ��ָ��
 */
void LADRC_Reset(LADRC *ladrc)
{
    ladrc->z1 = 0.0f;
    ladrc->z2 = 0.0f;
    ladrc->last_u = 0.0f;
} 
