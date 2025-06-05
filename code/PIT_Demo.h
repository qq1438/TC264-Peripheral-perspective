#ifndef _PIT_Demo_h_
#define _PIT_Demo_h_

#include "zf_common_headfile.h"

extern PID MID_PID;
extern LADRC speed_ladrc;
extern LADRC gyroz_ladrc;
extern float speed;
extern float add_speed;
extern float Zangle_acc;
extern LADRC speed_ladrc;
float Get_gyro_z(uint8 time);

#endif
