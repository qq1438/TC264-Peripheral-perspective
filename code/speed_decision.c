#include "speed_decision.h"


uint8 calculate_minihigh(BoundaryData *left, BoundaryData *right)
{
    static uint8 results[10] = {120};
    static uint8 index = 0;
    uint32 sum = 0;
    uint8 max = 0;
    uint8 min = 255;
    uint8 high_Signal[120][2] = {{0,0}};
    uint8 left_count[120]  = {0};
    uint8 right_count[120] = {0};
    uint8 x_position = mid_position;
    uint8 width = 0;
    if(left->original_step > 10){
        for (uint8 i = 0; i < left->original_step; i++) {
            uint8 y = left->original_pts[i][1];
            left_count[y]++;
            if (left_count[y] == 1) {
                high_Signal[y][0] = left->original_pts[i][0];
            }
        }
    }
    if(right->original_step > 10){
        for (uint8 i = 0; i < right->original_step; i++) {
            uint8 y = right->original_pts[i][1];
            right_count[y]++;
            if (right_count[y] == 1) {
                high_Signal[y][1] = right->original_pts[i][0];
            }
        }
    }
    
    for (uint8 i = 0; i < 120; i++) {
        if (left_count[i] == 1 && right_count[i] == 1) {
            x_position = (high_Signal[i][0] + high_Signal[i][1]) / 2;
            width = high_Signal[i][1] - high_Signal[i][0];
            break;
        }
    }
    
    uint8 top1 = Seek_TOP(x_position);
    uint8 top2 = Seek_TOP(x_position + 1);
    uint8 top3 = Seek_TOP(x_position - 1);
    uint8 top4 = Seek_TOP(mid_position);

    uint8 tops[4] = {top1, top2, top3, top4};
    // 简单选择排序
    for(uint8 i = 0; i < 3; i++) {
        for(uint8 j = i + 1; j < 4; j++) {
            if(tops[i] > tops[j]) {
                uint8 temp = tops[i];
                tops[i] = tops[j];
                tops[j] = temp;
            }
        }
    }
    // 取中间两个的平均值作为中值
    uint8 top = (tops[1] + tops[2]) / 2;

    results[index] = top ;
    index = (index + 1) % 10;

    for (uint8 i = 0; i < 10; i++) {
        if (results[i] > max) max = results[i];
        if (results[i] < min) min = results[i];
        sum += results[i];
    }

    sum -= (max + min);

    return sum / 8;
}

float calculate_curvature(BoundaryData *left, BoundaryData *right)
{
    const uint8_t min_points = 10;
    float left_curv = 0;
    float right_curv = 0;
    if(left->now_step > 10){
        uint8_t P1_left[2] = { left->now_pts[1][0], left->now_pts[1][1] };
        uint8_t P2_left[2] = { left->now_pts[left->now_step / 2][0], left->now_pts[left->now_step / 2][1] };
        uint8_t P3_left[2] = { left->now_pts[left->now_step - 1][0], left->now_pts[left->now_step - 1][1] };
        left_curv = compute_curvature(P1_left, P2_left, P3_left);
    }else{
        left_curv = 1;
    }
    if(right->now_step > 10){
        uint8_t P1_right[2] = { right->now_pts[1][0], right->now_pts[1][1] };
        uint8_t P2_right[2] = { right->now_pts[right->now_step / 2][0], right->now_pts[right->now_step / 2][1] };
        uint8_t P3_right[2] = { right->now_pts[right->now_step - 1][0], right->now_pts[right->now_step - 1][1] };
        right_curv = compute_curvature(P1_right, P2_right, P3_right);
    }else{
        right_curv = 1;
    }
    return (left_curv + right_curv) * 1000 / 2.0f;
}
