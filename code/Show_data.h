#ifndef _Show_data_h_
#define _Show_data_h_

#include "zf_common_headfile.h"

void Send_Zoomimg(void);
void point_show(uint8 PTS[][2],uint8 ID);
void point_show1(uint8 point[2]);
void point_show2(uint8 image[MT9V03X_H][MT9V03X_W],uint8 point[2]);
void Pts_Show(uint8 image[MT9V03X_H][MT9V03X_W],uint8 PTS[][2],uint8 step);
void Pts_Show1(uint8 PTS[][2],uint8 step);

void send_line(uint8 pts_in[][2],uint16 step);

#endif
