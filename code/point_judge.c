#include "point_judge.h"

void update_corner_mapping(BoundaryData *data,uint8 ID[3]) 
{
    for(uint8 i=0; i<3; i++) {
        data->map[i].original[0] = data->original_pts[ID[i]][0];
        data->map[i].original[1] = data->original_pts[ID[i]][1];
        data->map[i].original_index = ID[i];
        TransformSinglePoint(data->original_pts[ID[i]], data->map[i].transformed);
        if(data->map[i].transformed[0] == 0 && data->map[i].transformed[1] == 0) continue;
        
        uint8 min_dist = 255;
        for(uint8 j=0; j<data->now_step; j++){
            // 快速曼哈顿距离计算
            uint8 dx = abs(data->now_pts[j][0] - data->map[i].transformed[0]);
            uint8 dy = abs(data->now_pts[j][1] - data->map[i].transformed[1]);
            uint8 dist = dx + dy;
            
            if(dist < min_dist){
                min_dist = dist;
                data->map[i].now_index = j;
            }
            if(dist < 2) break; // 提前退出优化
        }
    }
}

void filter_point(BoundaryData *data,bool side)
{
    uint8 ID[3] = {0};
    uint16 max_values[3] = {0};
    for(uint8 i=0;i<data->now_step_original;i++)
    {
        uint8 cur_x = side ? data->original_pts[i][0] : MT9V03X_W - data->original_pts[i][0];
        uint8 cur_y = MT9V03X_H - data->original_pts[i][1];
        uint16 XY = cur_x * cur_y;
        struct {
            uint16 *max;
            uint8 *id;
            uint16 compare_value;
        } checks[3] = {
            {&max_values[0], &ID[0], cur_x},          
            {&max_values[1], &ID[1], cur_y},      
            {&max_values[2], &ID[2], XY}             
        };

        for(uint8 c = 0; c < 3; c++) {
            if(checks[c].compare_value > *checks[c].max || 
              (checks[c].compare_value == *checks[c].max && 
               XY > *checks[c].max * (c == 0 ? (MT9V03X_H - data->original_pts[ID[0]][1]) : 
                                      (c == 1 ? (side ? data->original_pts[ID[1]][0] : MT9V03X_W - data->original_pts[ID[1]][0]) : 1))))
            {
                *checks[c].max = checks[c].compare_value;
                *checks[c].id = i;
            }
        }
    }
    update_corner_mapping(data,ID);
}

void count_angle(BoundaryData *data,uint8 window)
{
    float angle_max = 0;
    uint8 id = 0;
    for(uint8 i = 0; i < 3; i++)
    {
        uint8 im = clip(data->map[i].now_index, window, data->now_step-1) - window;
        uint8 ip = clip(data->map[i].now_index, 0, data->now_step-1 - window) + window;
        float angle = local_angle_points(data->now_pts, data->now_step, data->map[i].now_index, window);
        float angle_im = im < window ? 0 : local_angle_points(data->now_pts, data->now_step, im, window);
        float angle_ip = ip > data->now_step - window ? 0 : local_angle_points(data->now_pts, data->now_step, ip, window);
        float angle_diff = fabs(angle) - 0.5f * (fabs(angle_im) + fabs(angle_ip));
        if(angle_diff > angle_max)
        {
            angle_max = angle_diff;
            id = i;
        }
    }
    uint8 number = 0;

    if(data->map[id].now_index < data->now_step - window &&
        data->map[id].original[1] > 1 && data->map[id].original[1] < MT9V03X_H - 1 &&
        data->map[id].original[0] > 1 && data->map[id].original[0] < MT9V03X_W - 1)
    {
        uint8 threshold = adaptiveThreshold(data->map[id].original[1],data->map[id].original[0],3,3);
        for(int8 dy = -1; dy <= 1; dy++)
        {
            for(int8 dx=-1; dx<=1; dx++)
            {
                if(Image[data->map[id].original[1]+dy][data->map[id].original[0]+dx] > threshold)
                {
                    number++;
                }
            }
        }
    }
    data->Lp_id = data->map[id].now_index;
    angle_max = angle_max / PI * 180;
    if(angle_max > ANGLE_LOW && angle_max < ANGLE_HIGH && data->map[id].now_index < Step_Max - 2 * WINDOW_SIZE && number > 5)
    {
        data->Lp_state = 1;
    }
}

void find_corner(BoundaryData *left, BoundaryData *right)
{
    left->is_straight = right->is_straight = 0;
    left->Lp_state = right->Lp_state = 0;
    left->Lp_id = right->Lp_id = 0;
    left->is_lost = right->is_lost = 0;
    filter_point(left,1);
    filter_point(right,0);
    count_angle(left,WINDOW_SIZE);
    count_angle(right,WINDOW_SIZE);
    count_angle(left,WINDOW_SIZE / 2);
    count_angle(right,WINDOW_SIZE / 2);
    if(left->now_step > Step_Max / 2)
    {
        uint8 high = abs(left->now_pts[left->now_step - 1][1] - left->now_pts[0][1]);
        float curvature = compute_curvature(left->now_pts[0],left->now_pts[(left->now_step / 2)],left->now_pts[(left->now_step - 1)]);
        if(curvature * 1000 < CURVATURE_THRESHOLD && !left->Lp_state)
        {
            left->is_straight = 1;
        }
    }
    if(right->now_step > Step_Max / 2)
    {
        uint8 high = abs(right->now_pts[right->now_step - 1][1] - right->now_pts[0][1]);
        float curvature = compute_curvature(right->now_pts[0],right->now_pts[(right->now_step / 2)],right->now_pts[(right->now_step - 1)]);
        if(curvature * 1000 < CURVATURE_THRESHOLD && !right->Lp_state)
        {
            right->is_straight = 1;
        }
    }

    int L_length = abs(left->now_pts[left->now_step - 1][0] - left->now_pts[0][0]) + abs(left->now_pts[left->now_step - 1][1] - left->now_pts[0][1]);
    int R_length = abs(right->now_pts[right->now_step - 1][0] - right->now_pts[0][0]) + abs(right->now_pts[right->now_step - 1][1] - right->now_pts[0][1]);

    if(left->now_step < 3 || L_length < 5)
    {
        left->is_lost = 1;
    }

    if(right->now_step < 3 || R_length < 5)
    {
        right->is_lost = 1;
    }
    
}

uint8 seek_corner(uint8 pts_in[][2], uint8 step, uint8 reverse_order) 
{
    if(step == 0) return 0;
    
    uint8 best_index = step-1;
    int MAX_value = 0;
    uint8 min_y = pts_in[step - 1][1];
    uint8 min_x = pts_in[0][0];
    for(uint8 i = 0; i < step; i++) {
        uint8 cur_x = pts_in[i][0];  
        uint8 cur_y = pts_in[i][1];
        int now_value = 0;
        
        switch(reverse_order) {
            case 0: now_value = cur_x; break;
            case 1: now_value = (MT9V03X_W - cur_x); break;
            case 2: now_value = cur_y * (MT9V03X_W - cur_x); break;
            case 3: now_value = cur_y * cur_x; break;
            case 4: now_value = abs((cur_y - min_y) * (cur_x)); break;  
            case 5: now_value = abs((cur_y - min_y) * (MT9V03X_W - cur_x)); break;
        }
        
        if(now_value > MAX_value) {
            MAX_value = now_value;
            best_index = i;
        }
    }
    
    return best_index;
}
