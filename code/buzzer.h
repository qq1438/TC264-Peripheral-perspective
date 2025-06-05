#ifndef _BUZZER_H_
#define _BUZZER_H_

#include "zf_common_typedef.h"
#include "zf_driver_pwm.h"

/**
 * @brief ������������ʼ����������Դ��������PWM���ź�Ƶ�ʣ���ʼռ�ձ�Ϊ0
 */
void buzzer_init(void);

/**
 * @brief ��50%ռ�ձ�����������
 */
void buzzer_start(void);

/**
 * @brief ֹͣ���������
 */
void buzzer_stop(void);

#endif // _BUZZER_H_ 