#ifndef _Filter_h_
#define _Filter_h_

#include <stdint.h>
typedef struct Kalman{
        int32_t  Last_P;
        int32_t  Now_P;
        int32_t  out;
        int32_t  Kg;
        int32_t  Q;
        int32_t  R;
}Kalman;

#include "zf_common_headfile.h"

void Kalman_Init(Kalman *kfp,float Q,float R);
float KalmanFilter(Kalman *kfp,float input);

extern Kalman Kspeed;
extern Kalman Kmid_error;
extern Kalman KZangle;
extern Kalman KGtarget;

#endif
