#include "key.h"

volatile uint8 key_flag = 0;
volatile bool nec_stop_flag = false;

void my_key_init(void)
{
    key_init(5);
    pit_ms_init(CCU60_CH0,5);
}

void key_scan(void)
{
    if(key_get_state(KEY_1) == KEY_SHORT_PRESS || key_get_state(KEY_1) == KEY_LONG_PRESS)
    {
        key_flag = Button_1;
    }
    else if(key_get_state(KEY_2) == KEY_SHORT_PRESS || key_get_state(KEY_2) == KEY_LONG_PRESS)
    {
        key_flag = Button_2;
    }   
    else if(key_get_state(KEY_3) == KEY_SHORT_PRESS || key_get_state(KEY_3) == KEY_LONG_PRESS)
    {
        key_flag = Button_3;
    }
    else if(key_get_state(KEY_4) == KEY_SHORT_PRESS || key_get_state(KEY_4) == KEY_LONG_PRESS)
    {
        key_flag = Button_4;
    }
}





