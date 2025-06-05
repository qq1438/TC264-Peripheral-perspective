#include "Show_data.h"

#define IPCH 60
#define IPCW 80

float Mh = MT9V03X_H;
float Lh = IPCH;
float Mw = MT9V03X_W;
float Lw = IPCW;


void Send_Zoomimg(void)
{
//    uint8 Image_Use[IPCH][IPCW];
//    int i, j, row, line;
//    const float div_h = Mh / Lh, div_w = Mw / Lw;
//    for (i = 0; i < IPCH; i++)
//    {
//        row = i * div_h + 0.5;
//        for (j = 0; j < IPCW; j++)
//        {
//            line = j * div_w + 0.5;
//            Image_Use[i][j] = Image[row][line];
//        }
//    }
//    sendimg((pico_uint8 *)Image_Use, 80, 60);
}

void point_show(uint8 PTS[][2],uint8 ID)
{
    uint8 x = PTS[ID][0];
    uint8 y = PTS[ID][1];
    for(int8 i = -1; i < 2; i++)
    {
        for(int8 j = -1; j < 2; j++)
        {
            if(x + i < 0 || y + j < 0 || x + i > ips200_width_max - 1 || y + j > ips200_height_max - 1)continue;
            ips200_draw_point(x + i, y + j, RGB565_BLACK);
        }
    }
}

void point_show1(uint8 point[2])
{
    uint8 x = point[0];
    uint8 y = point[1];
    for(int8 i = -1; i < 2; i++)
    {
        for(int8 j = -1; j < 2; j++)
        {
            if(x + i < 0 || y + j < 0 || x + i > ips200_width_max - 1 || y + j > ips200_height_max - 1)continue;
            ips200_draw_point(x + i, y + j, RGB565_RED);
        }
    }
}

void point_show2(uint8 image[MT9V03X_H][MT9V03X_W],uint8 point[2])
{
    uint8 x = point[0];
    uint8 y = point[1];
    for(int8 i = -1; i < 2; i++)
    {
        for(int8 j = -1; j < 2; j++)
        {
            if(x + i < 0 || y + j < 0 || x + i > ips200_width_max - 1 || y + j > ips200_height_max - 1)continue;
            image[y + j][x + i] = 255;
        }

    }
}

void Pts_Show(uint8 image[MT9V03X_H][MT9V03X_W],uint8 PTS[][2],uint8 step)
{
    for(uint8 i = 0; i < step; i++)
    {
        if(PTS[i][0] < 0 || PTS[i][0] > 187 || PTS[i][1] < 0 || PTS[i][1] > 119)continue;
        image[PTS[i][1]][PTS[i][0]] = 255;
    }
}

void Pts_Show1(uint8 PTS[][2],uint8 step)
{
    for(uint8 i = 0; i < step; i++)
    {
        if(PTS[i][0] < 0 || PTS[i][0] > 187 || PTS[i][1] < 0 || PTS[i][1] > 119)continue;
        ips200_draw_point(PTS[i][0], PTS[i][1], RGB565_RED);
    }
}

void send_line(uint8 pts_in[][2],uint16 step){
    uint8 pts_x[255];
    uint8 pts_y[255];
    for(int i = 0; i < step; i++){
        pts_x[i] = pts_in[i][0];
        pts_y[i] = pts_in[i][1];
    }
    sendline2(2,(pico_uint8 *)pts_x,(pico_uint8 *)pts_y,step);
}
