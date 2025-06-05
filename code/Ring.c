 #include "Ring.h"

RingState ring_ctrl = {
    .state = Ring_state1,
    .kind = no_ring,
    .phase_counter = 0,
    .sum_zangle = 0,
    .time_counter = 0,
    .flag = 0
};

static RingContext ring_ctx;  // 改为静态变量

void InitRingContext(ring_kind kind) {
    ring_ctx.is_left = (kind == left_ring);
    ring_ctx.bounds.current = ring_ctx.is_left ? &left_boundary : &right_boundary;
    ring_ctx.bounds.opposite = ring_ctx.is_left ? &right_boundary : &left_boundary;
    ring_ctx.bounds.Fcurrent = ring_ctx.is_left ? &Fleft_boundary : &Fright_boundary;
    ring_ctx.bounds.Fopposite = ring_ctx.is_left ? &Fright_boundary : &Fleft_boundary;
    ring_ctrl.last_high = 0;
    ring_ctrl.state = Ring_state1;
    ring_ctrl.phase_counter = 0;
    ring_ctrl.sum_zangle = 0;
    ring_ctrl.time_counter = 0;
    ring_ctrl.flag = 0;

}

void Ring_evolve(void);
void Ring_Out(void);
void Ring_end(void);


void Ring_Process(void)
{
    if(ring_ctrl.kind == no_ring )
    {
        if(left_boundary.Lp_state && right_boundary.is_straight && cross_ctrl.kind == no_cross){
            ring_ctrl.flag++;
            if(ring_ctrl.flag == 3){
                ring_ctrl.kind = left_ring;
            }
        }
        else if(right_boundary.Lp_state && left_boundary.is_straight && cross_ctrl.kind == no_cross){
            ring_ctrl.flag++;
            if(ring_ctrl.flag == 3){
                ring_ctrl.kind = right_ring;
            }
        }
        else {
            ring_ctrl.flag = 0;
        }
        if(ring_ctrl.kind != no_ring && ring_ctrl.flag == 3){
            InitRingContext(ring_ctrl.kind);  // 统一初始化
            ring_ctrl.time_counter = system_getval_ms();
        }

    }
    else
    {
        if(ring_ctrl.state >= Ring_state3)
            ring_ctrl.sum_zangle += fabs(Zangle_acc) * (system_getval_ms() - ring_ctrl.time_counter) / 1000;
        ring_ctrl.time_counter = system_getval_ms();
        switch(ring_ctrl.state){
            case Ring_state1:  // 检测环岛入口
                if(ring_ctx.bounds.current->is_lost)
                    ring_ctrl.phase_counter++;
                else{
                    if(ring_ctrl.phase_counter > 0)
                        ring_ctrl.phase_counter--;
                }
                ring_ctx.bounds.current->now_step = 0;
                if(ring_ctrl.phase_counter > PHASE_COUNT_THRESHOLD){
                    ring_ctrl.state = Ring_state2;
                    ring_ctrl.phase_counter = 0;
                }
                break;

            case Ring_state2:  // 确认进入环岛
                if(!ring_ctx.bounds.current->is_lost)
                    ring_ctrl.phase_counter++;
                else{
                    if(ring_ctrl.phase_counter > 0)
                        ring_ctrl.phase_counter--;
                }
                ring_ctx.bounds.current->now_step = 0;
                if(ring_ctrl.phase_counter > PHASE_COUNT_THRESHOLD){
                    ring_ctrl.state = Ring_state3;
                    ring_ctrl.phase_counter = 0;
                }
                break;

            case Ring_state3:  // 环岛内部处理阶段1
            case Ring_state4:  // 环岛内部处理阶段2
                Ring_evolve();
                break;

            case Ring_state5:  // 环岛出口处理
                Ring_Out();
                break;

            case Ring_state6:  // 环岛结束处理阶段1
                Ring_end();
                break;
    
            case Ring_state7:  // 环岛结束处理阶段2
                Ring_end();
                break;  

            case Ring_state8:  // 环岛退出
                if(ring_ctx.bounds.current->now_step < Step_Max / 3)
                    ring_ctrl.phase_counter++;
                ring_ctx.bounds.current->now_step = 0;
                if(ring_ctrl.phase_counter > PHASE_COUNT_THRESHOLD){
                    ring_ctrl.state = Ring_state9;
                    ring_ctrl.phase_counter = 0;
                }
                break;

            case Ring_state9:  // 环岛完全退出
                if(ring_ctx.bounds.current->now_step > Step_Max / 2)
                    ring_ctrl.phase_counter++;
                ring_ctx.bounds.current->now_step = 0;
                if(fabs(ring_ctrl.sum_zangle) > ANGLE_THRESHOLD_LARGE && ring_ctrl.phase_counter > PHASE_COUNT_THRESHOLD){
                    ring_ctrl.kind = no_ring; 
                    ring_ctrl.phase_counter = 0;
                    ring_ctrl.sum_zangle = 0;
                }
                break;
        }
    }
}

void Ring_switch(void)
{
    if(ring_ctrl.phase_counter > 3)
    {
        ring_ctrl.state++;
        ring_ctrl.phase_counter = 0; 
        Ring_Process();
    }
}

void Ring_evolve(void)
{
    Ring_switch();
    if(ring_ctx.bounds.opposite->now_step > Step_Max / 2 && ring_ctrl.state == Ring_state4) 
    {
        ring_ctrl.phase_counter++;
        if(ring_ctrl.phase_counter > 3 && fabs(ring_ctrl.sum_zangle) > 45)
        {
            Ring_switch();
            return;
        }
    }
    else if(ring_ctx.bounds.current->now_step < Step_Max / 2)
    {
        if(ring_ctrl.state == Ring_state3) {
            if(ring_ctx.bounds.opposite->original_step < MIN_POINTS)ring_ctrl.phase_counter++;
            if(ring_ctrl.phase_counter > 3 && fabs(ring_ctrl.sum_zangle) > 30){
                Ring_switch();
                return;
            }
        }
        uint8 start[2] = {0};
        start[1] = Start_high;
        if(ring_ctx.bounds.current->original_step > 0) {
            int16 temp_x = ring_ctx.bounds.current->original_pts[0][0] + (ring_ctx.is_left ? width_base : -width_base);
            start[0] = temp_x < 3 ? 3 : (temp_x > MT9V03X_W - 3 ? MT9V03X_W - 3 : temp_x);
        }
        else if(ring_ctx.bounds.opposite->original_step > Step_Max / 3)
        {
            start[0] = ring_ctx.bounds.opposite->original_pts[0][0];
            start[1] = ring_ctx.bounds.opposite->original_pts[0][1];
        }
        else
        {
            int16 temp_x = mid_position + (ring_ctx.is_left ? width_base / 2 : -width_base / 2);
            start[0] = temp_x < 3 ? 3 : (temp_x > MT9V03X_W - 3 ? MT9V03X_W - 3 : temp_x);
        }
        uint8 point[2] = {0};   
        if(ring_ctx.bounds.current->original_step > 0)   
        {
            uint8 id = Arry_Filter_2(ring_ctx.bounds.current->original_pts,ring_ctx.bounds.current->original_step);
            point[0] = ring_ctx.bounds.current->original_pts[id][0] + (ring_ctx.is_left ? 5 : -5);
            point[1] = ring_ctx.bounds.current->original_pts[id][1] - 3;
        }
        else
        {
            point[0] = (ring_ctx.is_left ? 10 : 178);
            point[1] = Start_high;
        }
        seek_up(point, ring_ctx.is_left ? left_pts : Right_pts);
        (ring_ctx.is_left ? FLeftPts_Deal_Original : FRightPts_Deal_Original)();
        ring_ctx.bounds.Fcurrent->original_step = PtsPerspective(ring_ctx.bounds.Fcurrent->original_pts,ring_ctx.bounds.Fcurrent->now_pts,ring_ctx.bounds.Fcurrent->original_step,MT9V03X_H);
        uint8 corner_id = seek_corner(
                                   ring_ctx.bounds.Fcurrent->original_pts,
                                   ring_ctx.bounds.Fcurrent->original_step,
                                   ring_ctx.is_left + 2);

        uint8 turn_point[2] = {
            ring_ctx.bounds.Fcurrent->original_pts[corner_id][0],
            ring_ctx.bounds.Fcurrent->original_pts[corner_id][1]
        };
        point[0] = (start[0] + turn_point[0]) / 2;
        point[1] = turn_point[1] * 1.3  < Start_high ? turn_point[1] * 1.3 : Start_high;
        ring_ctx.bounds.opposite->original_step = add_three_point_bezier(start, point,turn_point, ring_ctx.bounds.opposite->original_pts,0,1);
        ring_ctx.bounds.opposite->original_step = Arry_rollback(
            ring_ctx.bounds.Fcurrent->original_pts,
            ring_ctx.bounds.opposite->original_pts,
            ring_ctx.bounds.opposite->original_step,
            corner_id,
            MT9V03X_W

        );

       (ring_ctx.is_left ? RightPts_Deal_Now : LeftPts_Deal_Now)();
    }
    else
    {
        if(ring_ctrl.state == Ring_state3) {
            if(ring_ctx.bounds.opposite->original_step < MIN_POINTS)ring_ctrl.phase_counter++;
            if(ring_ctrl.phase_counter > 3 && fabs(ring_ctrl.sum_zangle) > 30){
                Ring_switch();
                return;
            }
        }
        ring_ctx.bounds.opposite->now_step = 0;
    }
}

uint8 IDID = 0; 
void Ring_Out(void)
{
    Ring_switch();
    uint8 corner_id = seek_corner(ring_ctx.bounds.opposite->original_pts, ring_ctx.bounds.opposite->original_step, ring_ctx.is_left);
    IDID = corner_id;
    if(corner_id < ring_ctx.bounds.opposite->original_step * 2 / 3)
    {
        if(ring_ctx.bounds.opposite->original_pts[corner_id][1] > Mini_high - 10  && corner_id > 3) ring_ctrl.phase_counter++;
        if(ring_ctrl.phase_counter > 3)
        {
            Ring_switch();
            return;
        }
        uint8 point[2] = {
            ring_ctx.bounds.opposite->original_pts[corner_id][0] + (ring_ctx.is_left ? -5 : 5),
            ring_ctx.bounds.opposite->original_pts[corner_id][1] - 5
        };

        seek_up(point, ring_ctx.is_left ? Right_pts : left_pts);

        (ring_ctx.is_left ? FRightPts_Deal_Original : FLeftPts_Deal_Original)();
        
        if(ring_ctx.bounds.opposite->original_step > 3  )
        {
            uint8 index = 0;
            uint8 h1 = abs(ring_ctx.bounds.opposite->original_pts[corner_id][1] - ring_ctx.bounds.opposite->original_pts[0][1]);
            uint8 w1 = abs(ring_ctx.bounds.opposite->original_pts[corner_id][0] - ring_ctx.bounds.opposite->original_pts[0][0]);
            uint8 h2 = abs(ring_ctx.bounds.Fopposite->original_pts[0][1] - ring_ctx.bounds.opposite->original_pts[corner_id][1]);
            uint8 w2 = w1 * h2 / h1;
            int16 temp_mx = ring_ctx.bounds.opposite->original_pts[corner_id][0] + (ring_ctx.is_left ? -w2 : w2);
            uint8 MX;
            if(temp_mx < 0) MX = 0;
            else if(temp_mx > MT9V03X_W - 1) MX = MT9V03X_W - 1;
            else MX = (uint8)temp_mx;
            for(uint8 i = 0; i < ring_ctx.bounds.Fopposite->original_step; i++)
            {
                uint8 x = ring_ctx.bounds.Fopposite->original_pts[i][0];
                if((ring_ctx.is_left && x <= MX) || (!ring_ctx.is_left && x >= MX))
                {
                    index = i;
                    break;
                }
            }
            uint8 turn_point[2] = {ring_ctx.bounds.Fopposite->original_pts[ring_ctx.bounds.Fopposite->original_step / 2][0], ring_ctx.bounds.Fopposite->original_pts[ring_ctx.bounds.Fopposite->original_step / 2][1]};
            point[0] = ring_ctx.bounds.Fopposite->original_pts[index][0];
            point[1] = ring_ctx.bounds.Fopposite->original_pts[index][1];
            ring_ctrl.last_high = (ring_ctx.bounds.opposite->original_pts[corner_id][1] + point[1]) / 2;
            ring_ctx.bounds.opposite->original_step = add_three_point_bezier(   
                ring_ctx.bounds.opposite->original_pts[corner_id],
                point,
                turn_point,
                ring_ctx.bounds.opposite->original_pts,
                corner_id,
                1
            ) + corner_id;

            ring_ctx.bounds.opposite->original_step = Arry_roll(
                ring_ctx.bounds.Fopposite->original_pts,
                ring_ctx.bounds.opposite->original_pts,
                ring_ctx.bounds.opposite->original_step,
                ring_ctx.bounds.Fopposite->original_step / 2,
                ring_ctx.bounds.Fopposite->original_step,
                MT9V03X_W
            );
        }
        else
        {
            ring_ctx.bounds.opposite->original_step = corner_id;
        }
    }
    else
    {
        if(ring_ctx.bounds.opposite->original_pts[corner_id][1] > Mini_high - 10 && ring_ctrl.sum_zangle > 200 && corner_id > 3)ring_ctrl.phase_counter++;
        if(ring_ctrl.phase_counter > 3)
        {
            Ring_switch();
            return;
        }
        if(ring_ctx.bounds.opposite->is_lost && ring_ctrl.sum_zangle > 200)ring_ctrl.phase_counter++;
        ring_ctx.bounds.opposite->original_step = corner_id;
    }
    (ring_ctx.is_left ? RightPts_Deal_Now : LeftPts_Deal_Now)(); 
}

void Ring_end(void)
{
    Ring_switch();
    
    // 处理Ring_state6的丢线情况和Ring_state7的找线情况
    if(ring_ctx.bounds.opposite->is_lost && ring_ctrl.state == Ring_state6) ring_ctrl.phase_counter++;
    if(!ring_ctx.bounds.opposite->is_lost && ring_ctrl.state == Ring_state7)
    {
        ring_ctrl.phase_counter++;
        return;
    }
    if(ring_ctrl.phase_counter > 3) {
        Ring_switch();
        return;
    }
    
    // 如果在Ring_state6且对侧边界丢线，直接累积计数等待恢复
    if(ring_ctx.bounds.opposite->is_lost && ring_ctrl.state == Ring_state6) {
        return;
    }
    
    uint8 start[2] = {0};
    
    // 确保start[1]有合理的值
    if(ring_ctrl.last_high != 0) {
        start[1] = ring_ctrl.last_high;
    } else {
        start[1] = Mini_high;  // 兜底值
    }
    
    if(ring_ctx.bounds.opposite->original_step > 3 && ring_ctrl.state == Ring_state6) {
        uint8 corner_id = seek_corner(ring_ctx.bounds.opposite->original_pts, ring_ctx.bounds.opposite->original_step, ring_ctx.is_left);
        // 边界保护corner_id
        corner_id = corner_id < ring_ctx.bounds.opposite->original_step ? corner_id : ring_ctx.bounds.opposite->original_step - 1;
        
        int16 temp_x = ring_ctx.bounds.opposite->original_pts[corner_id][0];
        if(ring_ctx.is_left)start[0] = temp_x > MT9V03X_W - 5 ? MT9V03X_W - 5 : temp_x + 3;
        else start[0] = temp_x < 5 ? 5 : temp_x - 3;
        start[1] = ring_ctx.bounds.opposite->original_pts[corner_id][1] - 3;
    }
    else
    {
        int16 temp_x = mid_position + (ring_ctx.is_left ? width_base / 2 : -width_base / 2);
        if(ring_ctx.is_left)start[0] = temp_x > MT9V03X_W - 5 ? MT9V03X_W - 5 : temp_x + 3;
        else start[0] = temp_x < 5 ? 5 : temp_x - 3;
    }

    seek_up(start, ring_ctx.is_left ? Right_pts : left_pts);

    (ring_ctx.is_left ? FRightPts_Deal_Original : FLeftPts_Deal_Original)();
    
    
    uint8 index = ring_ctx.bounds.Fopposite->original_step > 1 ? ring_ctx.bounds.Fopposite->original_step / 2 : 0;
    for(uint8 i = 0; i < ring_ctx.bounds.Fopposite->original_step; i++)
    {
        uint8 x = ring_ctx.bounds.Fopposite->original_pts[i][0];
        if((ring_ctx.is_left && x <= mid_position) || (!ring_ctx.is_left && x >= mid_position))
        {
            index = i;
            break;
        }
    }
    
    // 边界保护index和各个点的索引
    index = index < ring_ctx.bounds.Fopposite->original_step ? index : ring_ctx.bounds.Fopposite->original_step - 1;
    uint8 point_idx = ring_ctx.bounds.Fopposite->original_step / 3;
    point_idx = point_idx < ring_ctx.bounds.Fopposite->original_step ? point_idx : ring_ctx.bounds.Fopposite->original_step - 1;
    
    uint8 point[2] = {ring_ctx.bounds.Fopposite->original_pts[point_idx][0], ring_ctx.bounds.Fopposite->original_pts[point_idx][1]};
    uint8 turn_point[2] = {ring_ctx.bounds.Fopposite->original_pts[index][0], ring_ctx.bounds.Fopposite->original_pts[index][1]};
    ring_ctrl.last_high = (Start_high + ring_ctx.bounds.Fopposite->original_pts[0][1]) / 2;
    
    if(ring_ctx.is_left){
        if(turn_point[0] > start[0]){
            turn_point[0] = 0;
            turn_point[1] = ring_ctrl.last_high / 2;
            point[0] = start[0];
            point[1] = start[1];
        }
    }else{
        if(turn_point[0] < start[0]){
            turn_point[0] = MT9V03X_W - 1;
            turn_point[1] = ring_ctrl.last_high / 2;
            point[0] = start[0];
            point[1] = start[1];
        }
    }
    
    // ring_ctx.bounds.opposite->original_step = add_three_point_bezier(
    //     start,
    //     point,
    //     turn_point,
    //     ring_ctx.bounds.opposite->original_pts,
    //     0,
    //     1
    // );
    ring_ctx.bounds.opposite->original_step = addline(start, point, ring_ctx.bounds.opposite->original_pts, 0, 1);
    ring_ctx.bounds.opposite->original_step = Arry_roll(
        ring_ctx.bounds.Fopposite->original_pts,
        ring_ctx.bounds.opposite->original_pts,
        ring_ctx.bounds.opposite->original_step - 1,
        point_idx,
        ring_ctx.bounds.Fopposite->original_step - 1,
        MT9V03X_W
    );
    (ring_ctx.is_left ? RightPts_Deal_Now : LeftPts_Deal_Now)();  

    ring_ctx.bounds.current->original_step = 0;  
}
