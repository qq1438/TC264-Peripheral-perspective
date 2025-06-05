#ifndef _clip_h_
#define _clip_h_

#include "zf_common_headfile.h"

#define clamp(val, lo, hi) ((val) < (lo) ? (lo) : ((val) > (hi) ? (hi) : (val)))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

int clip(int x, int low, int up);
float fclip(float x, float low, float up);
float SquareRootFloat(float number);
int My_fabs(int number);
uint8 Arry_Copy(uint8 pts_in[][2],uint8 pts_out[][2],uint8 step);
uint8 Arry_rollback(uint8 pts_in[][2],uint8 pts_out[][2],uint8 location1,uint8 location2,uint8 step_Max);
uint8 Arry_roll(uint8 pts_in[][2],uint8 pts_out[][2],uint8 location1,uint8 location2,uint8 step,uint8 step_Max);
uint8 Arry_Filter(uint8 pts_in[][2],uint8 pts_out[][2],uint8 step);
uint8 Arry_Filter_2(uint8 pts_in[][2],uint8 step);

#endif
