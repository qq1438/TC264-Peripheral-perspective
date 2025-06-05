#include "clip.h"

int clip(int x, int low, int up) {
    return x > up ? up : x < low ? low : x;
}

float fclip(float x, float low, float up) {
    return x > up ? up : x < low ? low : x;
}

float SquareRootFloat(float number)
{
    long i;
    float x, y;
    const float f = 1.5F;

    x = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( f - ( x * y * y ) );
    y  = y * ( f - ( x * y * y ) );
    return number * y;
}

int My_fabs(int number){
    if(number < 0)return (-number);
    return number;
}

uint8 Arry_Copy(uint8 pts_in[][2],uint8 pts_out[][2],uint8 step)
{
    uint8 i = 0;
    for(i = 0; i < step; i++)
    {
        uint8 x =  pts_in[i][0];
        uint8 y =  pts_in[i][1];
        if(x == 0 && y == 0)break;
        pts_out[i][0] = x;
        pts_out[i][1] = y;
    }
    return i;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

uint8 Arry_rollback(uint8 pts_in[][2],uint8 pts_out[][2],uint8 location1,uint8 location2,uint8 step_Max)
{
    uint8 j = location1;
    for(int16 i = location2 - 1; i > 0; i--)
    {
        pts_out[j][0] = pts_in[i][0];
        pts_out[j++][1] = pts_in[i][1];
        if(j >= step_Max)break;
    }
    return j;
}


uint8 Arry_roll(uint8 pts_in[][2],uint8 pts_out[][2],uint8 location1,uint8 location2,uint8 step,uint8 step_Max)
{
    uint8 j = location1;
    for(int16 i = location2; i < step; i++)
    {
        pts_out[j][0] = pts_in[i][0];
        pts_out[j++][1] = pts_in[i][1];
        if(j >= step_Max)break;
    }
    return j;
}

uint8 Arry_Filter(uint8 pts_in[][2],uint8 pts_out[][2],uint8 step)
{
    uint8 j = 0;
    for(uint8 i = 0; i < step; i++)
    {
        if(pts_in[i][1]  <= Start_high)
        {
            pts_out[j][0] = pts_in[i][0];
            pts_out[j++][1] = pts_in[i][1];
        }
    }
    return j;
}

uint8 Arry_Filter_2(uint8 pts_in[][2],uint8 step)
{
    uint8 min_y = Start_high,id = 0;
    for(uint8 i = 0; i < step; i++)
    {
        if(pts_in[i][1] < min_y)
        {
            min_y = pts_in[i][1];
            id = i;
        }
    }
    return id;
}