#include "buzzer.h"

void buzzer_init(void)
{
    // 初始化PWM输出，占空比为0，蜂鸣器默认关闭
    pwm_init(ATOM1_CH3_P10_3, 2700, 0);
}

void buzzer_start(void)
{
    // 设置占空比为50%
    pwm_set_duty(ATOM1_CH3_P10_3, PWM_DUTY_MAX / 2);
}

void buzzer_stop(void)
{
    // 占空比置0，停止蜂鸣器
    pwm_set_duty(ATOM1_CH3_P10_3, 0);
} 