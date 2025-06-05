#include "cross.h"

CrossState cross_ctrl = {
    .kind = no_cross,
    .phase_counter = 0,
    .sum_distance = 0,
    .flag = 0,
    .left_x = 0,
    .left_y = 0,
    .right_x = 0,
    .right_y = 0,
};

void cross_judge(void);
void Cross_evolve(void);
void Cross_out(void);

void cross_process(void)
{
    if(cross_ctrl.kind == no_cross)
    {
        cross_judge();
    }
    else
    {
        switch(cross_ctrl.state)
        {
            case cross_state1:
               // gpio_set_level(P33_10,1);
                Cross_evolve();
                break;
            case cross_state2:
                Cross_out();
                break;
        }
    }

}

void cross_init(void)
{
    cross_ctrl.left_x = 0;
    cross_ctrl.left_y = 0;
    cross_ctrl.right_x = 0;
    cross_ctrl.right_y = 0;
    cross_ctrl.phase_counter = 0;
    cross_ctrl.sum_distance = 0;
    cross_ctrl.flag = 0;
}

void cross_judge(void)
{
    bool cross_flag = 0;
    if(left_boundary.Lp_state || right_boundary.Lp_state)
    {
        uint8 left_point[2] = {0};
        uint8 right_point[2] = {0};
        if(left_boundary.Lp_state)
        {
            left_point[0] = left_boundary.now_pts[left_boundary.Lp_id][0];
            left_point[1] = left_boundary.now_pts[left_boundary.Lp_id][1];
        }
        else
        {
            left_point[0] = left_boundary.map[1].transformed[0];
            left_point[1] = left_boundary.map[1].transformed[1];
        }
        if(right_boundary.Lp_state)
        {
            right_point[0] = right_boundary.now_pts[right_boundary.Lp_id][0];
            right_point[1] = right_boundary.now_pts[right_boundary.Lp_id][1];
        }
        else
        {
            right_point[0] = right_boundary.map[1].transformed[0];
            right_point[1] = right_boundary.map[1].transformed[1];
        }
        float distance = Get_Distance(left_point,right_point);
        if(left_boundary.Lp_state && right_boundary.Lp_state)cross_flag = 1;
        else if(left_boundary.Lp_state || right_boundary.Lp_state)
        {
            if(left_boundary.Lp_state && right_point[1] > left_point[1] && left_boundary.now_pts[left_boundary.now_step - 1][0] <= left_point[0])cross_ctrl.flag++;
            else if(right_boundary.Lp_state && right_point[1] < left_point[1] && right_boundary.now_pts[right_boundary.now_step - 1][0] >= right_point[0])cross_ctrl.flag++;
            else{
                if(cross_ctrl.flag > 0)cross_ctrl.flag--;
            }
            if(cross_ctrl.flag >= 3)cross_flag = 1;
        }
        if(distance < WIDTH + CROSS_single_width && distance > WIDTH - CROSS_single_width && cross_flag && ring_ctrl.kind == no_ring)
        {
            cross_ctrl.kind = mid_cross;
            cross_init();
        }
        if(left_boundary.Lp_state && right_boundary.original_step < 3 && left_boundary.now_step < Step_Max / 2)
        {
            cross_ctrl.kind = mid_cross;
            cross_init();
        }
        if(right_boundary.Lp_state && left_boundary.original_step < 3 && right_boundary.now_step > Step_Max / 2)
        {
            cross_ctrl.kind = mid_cross;
            cross_init();
        }
    }
    cross_ctrl.state = 0;
}

void Cross_evolve(void)
{
    if(left_boundary.now_pts > 0)
    {
        bool left_state = 1;
        uint8 left_point[2] = {0};
        track_leftpoint(left_boundary.now_pts,
                        left_boundary.now_step,
                        clip(left_boundary.map[2].now_index + WINDOW_SIZE / 2,0,left_boundary.now_step - 1),
                        left_point,
                        WINDOW_SIZE / 2,
                        30);
        ImagePerspective(left_point);
        seek_up(left_point,left_pts);
        FLeftPts_Deal_Original();
        Fleft_boundary.original_step = Arry_Filter_2(Fleft_boundary.original_pts,Fleft_boundary.original_step);
        uint8 corner_index = seek_corner(Fleft_boundary.original_pts,Fleft_boundary.original_step,4);
        uint8 Fleft_point[2] = {0};
        TransformSinglePoint(Fleft_boundary.original_pts[corner_index],Fleft_point);
        float distance = Get_Distance(Fleft_point,left_boundary.now_pts[left_boundary.map[2].now_index]);
        if(left_boundary.map[2].transformed[1] > right_boundary.map[2].transformed[1])
        {
            float dis = Get_Distance(Fleft_point,right_boundary.map[2].transformed);
            if(dis < WIDTH / 2)left_state = 0;
        }
        if(distance < WIDTH + CROSS_single_width && distance > WIDTH - CROSS_single_width && left_state)
        {
            left_boundary.original_step = addline(left_boundary.map[2].original,
                                             Fleft_boundary.original_pts[corner_index],
                                             left_boundary.original_pts,
                                             left_boundary.map[2].original_index,
                                             1) + left_boundary.map[2].original_index;

            left_boundary.original_step = Arry_roll(Fleft_boundary.original_pts,
                                               left_boundary.original_pts,
                                               left_boundary.original_step,
                                               corner_index,
                                               Fleft_boundary.original_step,
                                               MT9V03X_W);
            LeftPts_Deal_Now();
            cross_ctrl.left_x = Fleft_boundary.original_pts[corner_index][0] - CROSS_OFFSET;
            cross_ctrl.left_y = (Fleft_boundary.original_pts[corner_index][1] + left_boundary.map[2].original[1]) / 2;
        }else
        {
            left_boundary.now_step = left_boundary.map[2].now_index;
        }
    }
    if(right_boundary.now_pts > 0)
    {
        bool right_state = 1;
        uint8 right_point[2] = {0};
        track_rightpoint(right_boundary.now_pts,
                        right_boundary.now_step,
                        clip(right_boundary.map[2].now_index + WINDOW_SIZE / 2,0,right_boundary.now_step - 1),
                        right_point,
                        WINDOW_SIZE / 2,
                        30);
        ImagePerspective(right_point);
        seek_up(right_point,Right_pts);
        FRightPts_Deal_Original();
        Fright_boundary.original_step = Arry_Filter_2(Fright_boundary.original_pts,Fright_boundary.original_step);
        uint8 corner_index = seek_corner(Fright_boundary.original_pts,Fright_boundary.original_step,5);
        uint8 Fright_point[2] = {0};
        TransformSinglePoint(Fright_boundary.original_pts[corner_index],Fright_point);
        float distance = Get_Distance(Fright_point,right_boundary.now_pts[right_boundary.map[2].now_index]);
        if(right_boundary.map[2].transformed[1] > left_boundary.map[2].transformed[1])
        {
            float dis = Get_Distance(Fright_point,left_boundary.map[2].transformed);
            if(dis < WIDTH / 2)right_state = 0;
        }
        if(distance < WIDTH + CROSS_single_width && distance > WIDTH - CROSS_single_width && right_state)
        {
            right_boundary.original_step = addline(right_boundary.map[2].original,
                                             Fright_boundary.original_pts[corner_index],
                                             right_boundary.original_pts,
                                             right_boundary.map[2].original_index,
                                             1) + right_boundary.map[2].original_index;

            right_boundary.original_step = Arry_roll(Fright_boundary.original_pts,
                                                   right_boundary.original_pts,
                                                   right_boundary.original_step,
                                                   corner_index,
                                                   Fright_boundary.original_step,
                                                   MT9V03X_W);
            RightPts_Deal_Now();
            cross_ctrl.right_x = Fright_boundary.original_pts[corner_index][0] + CROSS_OFFSET;
            cross_ctrl.right_y = (Fright_boundary.original_pts[corner_index][1] + right_boundary.map[2].original[1]) / 2;
        }else
        {
            right_boundary.now_step = right_boundary.map[2].now_index;
        }
    }

    if(left_boundary.map[2].original_index <= MIN_Point && right_boundary.map[2].original_index <= MIN_Point)
    {
        cross_ctrl.phase_counter++;
    }
    if(cross_ctrl.phase_counter > Cross_Count_Threshold)
    {
        cross_ctrl.state++;
    }
}

void Cross_out(void)
{
    if(left_boundary.now_step > Step_Max / 2 
       && right_boundary.now_step > Step_Max / 2)
    {
       cross_ctrl.kind = no_cross;
       gpio_set_level(P33_10,0);
    }
    if(cross_ctrl.left_x <= mid_position)
    {
        uint8 left_point[2] = {cross_ctrl.left_x,cross_ctrl.left_y};
        seek_up(left_point,left_pts);
        FLeftPts_Deal_Original();
        Fleft_boundary.original_step = Arry_Filter_2(Fleft_boundary.original_pts,Fleft_boundary.original_step);
        uint8 corner_index = seek_corner(Fleft_boundary.original_pts,Fleft_boundary.original_step,4);
        left_boundary.original_step = Arry_roll(Fleft_boundary.original_pts,
                                               left_boundary.original_pts,
                                               0,
                                               corner_index,
                                               Fleft_boundary.original_step,
                                               MT9V03X_W);
        LeftPts_Deal_Now();
        cross_ctrl.left_x = Fleft_boundary.original_pts[corner_index][0] - CROSS_OFFSET ;
        cross_ctrl.left_y = (Fleft_boundary.original_pts[corner_index][1] + Start_high) / 2;
    }
    if(cross_ctrl.right_x >= mid_position)
    {
        uint8 right_point[2] = {cross_ctrl.right_x,cross_ctrl.right_y};
        seek_up(right_point,Right_pts);
        FRightPts_Deal_Original();
        Fright_boundary.original_step = Arry_Filter_2(Fright_boundary.original_pts,Fright_boundary.original_step);
        uint8 corner_index = seek_corner(Fright_boundary.original_pts,Fright_boundary.original_step,5);
        right_boundary.original_step = Arry_roll(Fright_boundary.original_pts,
                                               right_boundary.original_pts,
                                               0,
                                               corner_index,
                                               Fright_boundary.original_step,
                                               MT9V03X_W);
        RightPts_Deal_Now();
        cross_ctrl.right_x = Fright_boundary.original_pts[corner_index][0] + CROSS_OFFSET;
        cross_ctrl.right_y = (Fright_boundary.original_pts[corner_index][1] + Start_high) / 2;
    }
}
