#ifndef _BUZZER_H_
#define _BUZZER_H_

#include "zf_common_typedef.h"
#include "zf_driver_pwm.h"

/**
 * @brief 蜂鸣器驱动初始化，配置无源蜂鸣器的PWM引脚和频率，初始占空比为0
 */
void buzzer_init(void);

/**
 * @brief 以50%占空比启动蜂鸣器
 */
void buzzer_start(void);

/**
 * @brief 停止蜂鸣器输出
 */
void buzzer_stop(void);

#endif // _BUZZER_H_ 