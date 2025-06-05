#include "PIT_Demo.h"
#include "Zebra.h"

#define speed_over(x) x * 28

PID MID_PID;
LADRC speed_ladrc;
LADRC gyroz_ladrc;  

float speed = 0;
float add_speed = 0;
float Zangle_acc;
uint8 times = 0;

float Get_gyro_z(uint8 time)
{
    float sum_gyro_z = 0;
    for(uint8 i = 0; i < time; i++)
    {
        imu660ra_get_gyro();
        float data = imu660ra_gyro_transition(imu660ra_gyro_z);
        sum_gyro_z += data;
    }   
    return sum_gyro_z / (time * 1.0);
}

int mid_pwm = 0;
int mid_speed = 0;
float last_prop = 0;
uint32 stop_distance = 0;
bool start_signl = 1;
bool stop_state = 0;
IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0);
    pit_clear_flag(CCU60_CH1);
    float target_gyroz = (float)vague_speed_PID(&MID_PID,MID_Error);
    Zangle_acc = Get_gyro_z(1);
    LADRC_SetTarget(&gyroz_ladrc,target_gyroz);
    int Speed_out = (int)LADRC_Update(&gyroz_ladrc, Zangle_acc);
    float prop = Zangle_acc / 100;
    prop = fclip(prop*prop,0,0.5);
    if(MID_Error < 0)prop = -prop;
    prop = prop * 0.3 + last_prop * 0.7;
    last_prop = prop;
    float speed_target = speed_over(speed);
    if(long_state){
        speed_target += speed_over(add_speed);
        pwm_set_duty(ATOM0_CH0_P21_2,700);
        pwm_set_duty(ATOM0_CH1_P21_3,700);
    }else
    {
        pwm_set_duty(ATOM0_CH0_P21_2,FOC_PWM);
        pwm_set_duty(ATOM0_CH1_P21_3,FOC_PWM);
    }
    if(zebra==1)
    {
        prop = 0;
        stop_distance += mid_speed;
        if(stop_distance > 5000)
        {
            speed_target = 0;
            if(abs(mid_speed) == 0)times++;
            if(times >= 10)nec_stop_flag = 1;
            stop_distance = 1000000;
        }
        pwm_set_duty(ATOM2_CH3_P11_6,0);
        pwm_set_duty(ATOM0_CH0_P21_2,0);
        pwm_set_duty(ATOM0_CH1_P21_3,0);
    }
    else{
        pwm_set_duty(ATOM2_CH3_P11_6,10000);
    }
    
    int left_speed = encoder_get_count(TIM4_ENCODER);
    int right_speed = encoder_get_count(TIM2_ENCODER);
    mid_speed = (left_speed * (0.5 + prop) + right_speed * (0.5 - prop));
    encoder_clear_count(TIM4_ENCODER);
    encoder_clear_count(TIM2_ENCODER);
    LADRC_SetTarget(&speed_ladrc,speed_target);
    mid_pwm = LADRC_Update(&speed_ladrc,mid_speed);
    
    int left_pwm = 0,right_pwm = 0;
    if(Speed_out > 0)
    {
        left_pwm = mid_pwm -  abs(Speed_out);
        right_pwm = mid_pwm + abs(Speed_out);
    }else
    {
        left_pwm = mid_pwm + abs(Speed_out);
        right_pwm = mid_pwm - abs(Speed_out);
    }
    gpio_set_level(P02_5,left_pwm>0?1:0);
    gpio_set_level(P02_7,right_pwm>0?0:1);

    pwm_set_duty(ATOM0_CH4_P02_4,clip(abs(left_pwm),0,speed_ladrc.limit));
    pwm_set_duty(ATOM0_CH6_P02_6,clip(abs(right_pwm),0,speed_ladrc.limit));

}

