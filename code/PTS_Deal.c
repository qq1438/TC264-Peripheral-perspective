#include "PTS_Deal.h"
#include "Zebra.h"

BoundaryData left_boundary = {0};
BoundaryData right_boundary = {0};
FBoundaryData Fleft_boundary = {0};
FBoundaryData Fright_boundary = {0};
MidlineData midline = {0};

void LeftPts_Deal_Original(void)
{
    left_boundary.original_step = findline_lefthand_adaptive(5,3,left_boundary.original_pts);
}

void LeftPts_Deal_Now(void)
{
    left_boundary.now_step = PtsPerspective(left_boundary.original_pts,left_boundary.now_pts,left_boundary.original_step,MT9V03X_H);
    left_boundary.now_step_original = left_boundary.now_step;
    left_boundary.now_step = blur_points(left_boundary.now_step,3,left_boundary.now_pts);
    left_boundary.now_step = resample_points(left_boundary.now_pts,left_boundary.now_step,Step_Max,resam_point);
}

void RightPts_Deal_Original(void)
{
    right_boundary.original_step = findline_righthand_adaptive(5,3,right_boundary.original_pts);
}

void RightPts_Deal_Now(void)
{
    right_boundary.now_step = PtsPerspective(right_boundary.original_pts,right_boundary.now_pts,right_boundary.original_step,MT9V03X_H);
    right_boundary.now_step_original = right_boundary.now_step;
    right_boundary.now_step = blur_points(right_boundary.now_step,3,right_boundary.now_pts);
    right_boundary.now_step = resample_points(right_boundary.now_pts,right_boundary.now_step,Step_Max,resam_point);
}

void FLeftPts_Deal_Original(void)
{
    Fleft_boundary.original_step = findline_lefthand_adaptive(5,3,Fleft_boundary.original_pts);
}


void FRightPts_Deal_Original(void)
{
    Fright_boundary.original_step = findline_righthand_adaptive(5,3,Fright_boundary.original_pts);
}

bool track_type;

void MidPts_Deal(void)
{
    if (left_boundary.now_step < right_boundary.now_step / 2 && left_boundary.now_step < 20) {
        track_type = 0;
    } else if (right_boundary.now_step < left_boundary.now_step / 2 && right_boundary.now_step < 20) {
        track_type = 1;
    } else if (left_boundary.now_step < 10 && right_boundary.now_step > left_boundary.now_step) {
        track_type = 0;
    } else if (right_boundary.now_step < 10 && left_boundary.now_step > right_boundary.now_step) {
        track_type = 1;
    }

    if(track_type)
    {
        midline.step = track_leftline(left_boundary.now_pts, left_boundary.now_step, midline.pts, 10, 20);
    }
    else
    {
        midline.step = track_rightline(right_boundary.now_pts, right_boundary.now_step, midline.pts, 10, 20);
    }
    midline.step = blur_points(midline.step,3,midline.pts);
    if(midline.pts[0][1] < 120)
    {
        uint8 point[2] = {0};
        if(left_boundary.now_step > 0 && right_boundary.now_step > 0)
        {
            point[0] = (left_boundary.now_pts[0][0] + right_boundary.now_pts[0][0]) / 2;
        }
        else if(left_boundary.now_step > 0)
        {
            point[0] = left_boundary.now_pts[0][0] + WIDTH / 2;
        }
        else if(right_boundary.now_step > 0)
        {
            point[0] = right_boundary.now_pts[0][0] - WIDTH / 2;
        }
        point[1] = 120;
        addline(point,midline.pts[0],midline.pts,0,resam_point);
    }
    midline.step = resample_points(midline.pts, midline.step, Step_Max,resam_point);
    uint8 point[2] = {0};
    if(midline.pts[midline.step - 1][0] - midline.pts[0][0] > 1)
    {
        point[0] = MT9V03X_W - 1;
        point[1] = 0;
        midline.step += addline(midline.pts[midline.step - 1],point,midline.pts,midline.step - 1,resam_point);
    }else if(midline.pts[0][0] - midline.pts[midline.step - 1][0] > 1)
    {
        point[0] = 0;
        point[1] = 0;
        midline.step += addline(midline.pts[midline.step - 1],point,midline.pts,midline.step - 1,resam_point);
    }
    uint8 start_dist = 0;
    for(uint8 i = 0; i < midline.step; i++) {
        if(midline.pts[i][1] < Start_high) {
            start_dist = i;
            break;
        }
    }

    for (uint8 i = 0; i < midline.step; i++) {
        if(i >= start_dist) {
            uint8 dx = abs(midline.pts[i][0] - midline.pts[0][0]);
            uint8 dy = abs(midline.pts[i][1] - midline.pts[0][1]);
            midline.dist[i] = dx + dy;
        }
        else {
            midline.dist[i] = 0;
        }
    }
}

float MID_Error;
float KMID_Error;
bool long_state = 0;
bool zebra_state = 0;
uint8 long_count = 0;
volatile bool is_compress = false;
float curve_mid = 0;
uint8 high = 0;
void WITE(void);
void Pts_Deal(void)
{
    if(mt9v03x_finish_flag)
    {
        Fleft_boundary.original_step = 0;
        Fright_boundary.original_step = 0;
        left_boundary.original_step = 0;
        right_boundary.original_step = 0;
        Image_Copy(); 
        mt9v03x_finish_flag = 0;
        Seek_Pts_Seed(Start_high,Mini_high);
        if(original_right[0] - original_left[0] < width_base / 2 && Seed_State == two_pts)Seek_Pts_Seed(Start_high - 30,Mini_high  - 30);
        LeftPts_Deal_Original();
        LeftPts_Deal_Now();
        RightPts_Deal_Original();
        RightPts_Deal_Now();
        find_corner(&left_boundary, &right_boundary);

        if(ring_ctrl.kind == no_ring && cross_ctrl.kind == no_cross && zebra_state)zebra_judge(10);
        if(ring_ctrl.kind != no_ring || cross_ctrl.kind != no_cross)zebra_state = 1;
        if(cross_ctrl.kind == no_cross)Ring_Process();
        if(ring_ctrl.kind == no_ring)cross_process();
        MidPts_Deal();
        high = calculate_minihigh(&left_boundary, &right_boundary);
        if(high < 35 && ring_ctrl.kind == no_ring)
        {
            if(long_count < 6)long_count++;
        }
        else{
            if(long_count > 0)long_count--;
        }
       if(long_count > 3)long_state = 1;
       else{long_state = 0;}
        float decision = 0.3;
        MID_Error = Get_error(&midline, speed * 28, decision);
        curve_mid = calculate_curvature(&left_boundary, &right_boundary);
        if(Send_state)
        {
            while(is_compress);
            image_compress(Image);
            image_compress_boundary(&left_boundary,left_boundary_type);
            image_compress_boundary(&right_boundary,right_boundary_type);
            image_compress_fboundary(&Fleft_boundary,Fleft_boundary_type);
            image_compress_fboundary(&Fright_boundary,Fright_boundary_type);
            image_compress_midline(&midline,midline_type);
            image_compress_cross();
            image_compress_ring();
            parameter_compress_float(MID_Error,error_type);
            parameter_compress_uint8(high,speed_type);
            is_compress = true;
        }
        //WITE();
    }   
}

void WITE(void)
{
    ips200_show_gray_image(0,0,Image[0],MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
    ips200_draw_line(0,Mini_high,MT9V03X_W,Mini_high,RGB565_RED);
    ips200_draw_line(0,Start_high,MT9V03X_W,Start_high,RGB565_RED);
    uint8 point[2] = {0};
    memset(Image[0],0,MT9V03X_W * MT9V03X_H);
    ips200_show_uint(188,20,left_boundary.original_step,3);
    ips200_show_uint(188,40,right_boundary.original_step,3);
    Pts_Show1(left_boundary.original_pts, left_boundary.original_step);
    Pts_Show1(right_boundary.original_pts, right_boundary.original_step);
    uint8 target_dist = midline.step - 1;
    for (uint8 i = 0; i < midline.step; i++) {
        uint8 dx = abs(midline.pts[i][0] - midline.pts[0][0]);
        uint8 dy = abs(midline.pts[i][1] - midline.pts[0][1]);
        midline.dist[i] = dx + dy;
    }
    float distance = 4.5 * 28 * 0.3;
    for (uint8 i = 0; i < midline.step; i++) {
        if (midline.dist[i] > distance) {
            target_dist = i;
            ips200_show_uint(130,300,midline.dist[i],3);
            ips200_show_uint(0,270,midline.pts[0][0],3);
            ips200_show_uint(60,270,midline.pts[0][1],3);
            break;
        }
    }
    point[0] = midline.pts[target_dist][0];
    point[1] = midline.pts[target_dist][1] + 130;
    point_show1(point);
    ips200_show_uint(90,300,target_dist,3);
    // œ‘ æ¥¶¿Ì
    Pts_Show(Image, left_boundary.now_pts, left_boundary.now_step);
    Pts_Show(Image, right_boundary.now_pts, right_boundary.now_step);
    Pts_Show(Image, midline.pts,midline.step);

    Pts_Show1(Fleft_boundary.original_pts, Fleft_boundary.original_step);
    Pts_Show1(Fright_boundary.original_pts, Fright_boundary.original_step);

     ips200_show_uint(188,0,midline.step,3);
     ips200_show_uint(188,60,left_boundary.is_straight,3);
     ips200_show_uint(188,80,right_boundary.is_straight,3);
     ips200_show_uint(188,100,left_boundary.Lp_state,3);
     ips200_show_uint(188,120,right_boundary.Lp_state,3);
     ips200_show_uint(188,140,ring_ctrl.kind,3);
     ips200_show_uint(188,160,ring_ctrl.state ,3);
     ips200_show_uint(188,180,original_left[0],3);
     ips200_show_uint(188,200,original_right[0],3);
     ips200_show_uint(188,220,cross_ctrl.state,3);
     ips200_show_uint(188,240,cross_ctrl.kind,3);
     ips200_show_uint(188,260,high,3);
    ips200_show_float(0,300,MID_Error,3,3);

    if(left_boundary.Lp_id < left_boundary.now_step && left_boundary.Lp_state) {
        point[0] = left_boundary.now_pts[left_boundary.Lp_id][0];
        point[1] = left_boundary.now_pts[left_boundary.Lp_id][1];
    }

    uint8 point1[2] = {0};
    if(right_boundary.Lp_id < right_boundary.now_step && right_boundary.Lp_state) {
        point1[0] = right_boundary.now_pts[right_boundary.Lp_id][0];
        point1[1] = right_boundary.now_pts[right_boundary.Lp_id][1];
    }

   point_show2(Image[0],point);
   point_show2(Image[0],point1);
    ips200_show_gray_image(0,130,Image[0],MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
}

void Pts_Test(void)
{
    if(mt9v03x_finish_flag)
    {
        Pts_Deal();
        WITE();
    }
}



