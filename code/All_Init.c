#include "All_Init.h"
#include "menu.h"  // 添加菜单头文件
#include "parameter_flash.h"

float FOC_PWM = 0;
bool Send_state = 0;
seekfree_assistant_oscilloscope_struct oscilloscope_data;

void send_signel(void)
{
    wireless_uart_init();
    oscilloscope_data.data[0] = 0;
    oscilloscope_data.data[1] = 0;
    oscilloscope_data.data[2] = 0;
    oscilloscope_data.data[3] = 0;
    oscilloscope_data.data[4] = 0;
    oscilloscope_data.data[5] = 0;
    oscilloscope_data.data[6] = 0;
    oscilloscope_data.data[7] = 0;
    oscilloscope_data.channel_num = 8;
}

void send_value(void){
    seekfree_assistant_oscilloscope_send(&oscilloscope_data);
    oscilloscope_data.data[0] = 0;
    oscilloscope_data.data[1] = 0;
    oscilloscope_data.data[2] = 0;
    oscilloscope_data.data[3] = 0;
    oscilloscope_data.data[4] = 0;
    oscilloscope_data.data[5] = 0;
    oscilloscope_data.data[6] = 0;
    oscilloscope_data.data[7] = 0;
}

// 设备初始化函数
void Device_Init(void)
{
    // 初始化PID参数
    uint32_t i = 1000000;
    while(i--);
    buzzer_init();
    PID_Init(&MID_PID,0.5,0);
    LADRC_Init(&speed_ladrc,0.005);
    LADRC_Init(&gyroz_ladrc,0.005);

    // 初始化显示屏
    ips200_set_dir(IPS200_PORTAIT_180);
    ips200_init(IPS200_TYPE_PARALLEL8);
    gpio_init(P11_6, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    pwm_init(ATOM2_CH3_P11_6,14500,10000);
    w25n04_init();

    // 初始化摄像头
    mt9v03x_init();
    uint16 boot_exp;
    ExposureTime_flash_load(&boot_exp);
    mt9v03x_set_exposure_time(boot_exp);
    // 初始化定时器中断
    my_key_init();

    float change_un_Mat[3][3];
    Read_Matrix(change_un_Mat);
    ImagePerspective_Init(change_un_Mat);
    Parameter_flash_load();
    menu_open();
    Parameter_flash_load();

    while(imu660ra_init());

    encoder_quad_init(TIM4_ENCODER,TIM4_ENCODER_CH2_P00_9,TIM4_ENCODER_CH1_P02_8);
    encoder_quad_init(TIM2_ENCODER,TIM2_ENCODER_CH1_P33_7,TIM2_ENCODER_CH2_P33_6);

    gpio_init(P02_5,GPO,1,GPO_PUSH_PULL);   //左轮,P02_5控制方向，，P02_4控制转速
    pwm_init(ATOM0_CH6_P02_6,17*1000,0);
    gpio_init(P02_7,GPO,0,GPO_PUSH_PULL);   //右轮，P02_7控制方向，，P02_6控制转速
    pwm_init(ATOM0_CH4_P02_4,17*1000,0);


    //send_signel();
    encoder_clear_count(TIM4_ENCODER);
    encoder_clear_count(TIM2_ENCODER);
    pwm_init(ATOM0_CH0_P21_2,50,FOC_PWM);
    pwm_init(ATOM0_CH1_P21_3,50,FOC_PWM);
    pit_ms_init(CCU60_CH1,5);
}



