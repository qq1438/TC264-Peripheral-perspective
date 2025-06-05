#ifndef PID_H
#define PID_H

typedef struct {
    float KP;
    float KI;
    float KD;
    float e_ration;
    float de_ration;
    float target;
    float last_error;
    float llast_error;
    float Integral_error;
    int last_output;
    float limit;
}PID;

#include "zf_common_headfile.h"

void PID_Init(PID *PIDX, float e_ration,float de_ration);
float PID_Position(PID *PIDX, float Value);
float PID_Incream(PID *PIDX, float Value);
int PID_Incream_IMU(PID *PIDX, float Value);
int angle_pid(PID *PIDX,float Value);
int vague_speed_PID(PID *pid,int value);
void PID_dtarget(PID *pid,float MAX_target,float target,float now_value,uint16 step);

#endif
