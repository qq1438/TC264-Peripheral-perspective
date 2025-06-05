#include "buzzer.h"

void buzzer_init(void)
{
    // ��ʼ��PWM�����ռ�ձ�Ϊ0��������Ĭ�Ϲر�
    pwm_init(ATOM1_CH3_P10_3, 2700, 0);
}

void buzzer_start(void)
{
    // ����ռ�ձ�Ϊ50%
    pwm_set_duty(ATOM1_CH3_P10_3, PWM_DUTY_MAX / 2);
}

void buzzer_stop(void)
{
    // ռ�ձ���0��ֹͣ������
    pwm_set_duty(ATOM1_CH3_P10_3, 0);
} 