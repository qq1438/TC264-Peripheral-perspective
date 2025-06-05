#include "image_processing.h"
#include <math.h>   
#include <float.h>  

uint8 Image[MT9V03X_H][MT9V03X_W];
uint8 Seed_left[2];
uint8 Seed_right[2];
uint8 original_left[2];
uint8 original_right[2];

uint8 mid_position = 94;

uint8 Seed_State = 0;
uint8 width_base = 0;

uint32 left_threshold = 0;
uint32 right_threshold = 0;
uint8 low_threshold = 0;
void Image_Copy(void){
    for (uint16_t i = 0; i < MT9V03X_H; i++) {
        for (uint16_t j = 0; j < MT9V03X_W; j++) {
            Image[i][j] = mt9v03x_image[i][j];
        }
    }
}

float perspective_Mat[3][3];

uint8 CalculateTransform(uint8 y, uint8 x, uint8 output[2]) {
    if(y >= MT9V03X_H || x >= MT9V03X_W) return 0;

    float fx = (float)x;
    float fy = (float)y;
    float denominator = (perspective_Mat[2][0] * fx + perspective_Mat[2][1] * fy + perspective_Mat[2][2]);

    if (fabsf(denominator) < FLT_EPSILON) {
        return 0;
    }

    float transformed_x = (perspective_Mat[0][0] * fx + perspective_Mat[0][1] * fy + perspective_Mat[0][2]) / denominator;
    float transformed_y = (perspective_Mat[1][0] * fx + perspective_Mat[1][1] * fy + perspective_Mat[1][2]) / denominator;

    if (transformed_x >= 0.0f && transformed_x < (float)MT9V03X_W &&
        transformed_y >= 0.0f && transformed_y < (float)MT9V03X_H) {
        output[0] = (uint8)roundf(transformed_x);
        output[1] = (uint8)roundf(transformed_y);
        if (output[0] < MT9V03X_W && output[1] < MT9V03X_H) {
           return 1;
        }
    }

    return 0;
}

void CalculateTransform_inverse(uint8 y, uint8 x, uint8 output[2]) {
    float fx = (float)x;
    float fy = (float)y;
    float denominator = (perspective_Mat[2][0] * fx + perspective_Mat[2][1] * fy + perspective_Mat[2][2]);
    
    float transformed_x = (perspective_Mat[0][0] * fx + perspective_Mat[0][1] * fy + perspective_Mat[0][2]) / denominator;  
    float transformed_y = (perspective_Mat[1][0] * fx + perspective_Mat[1][1] * fy + perspective_Mat[1][2]) / denominator;
            
    output[0] = (uint8)roundf(transformed_x);
    output[1] = (uint8)roundf(MT9V03X_H - transformed_y);
}

void ImagePerspective_Init(float change_un_Mat[3][3]) {
    for (int i = 0; i < 3; i++) {
         for (int j = 0; j < 3; j++) {
             perspective_Mat[i][j] = change_un_Mat[i][j];
         }
     }
}

void ImagePerspective(uint8 pts_in[2]){
    uint8 x,y;
    x = pts_in[0];
    y = pts_in[1];
    float denominator = (perspective_Mat[0][0] * perspective_Mat[1][1] - perspective_Mat[0][0] * perspective_Mat[2][1] * y -
                        perspective_Mat[0][1] * perspective_Mat[1][0] + perspective_Mat[0][1] * perspective_Mat[2][0] * y +
                        perspective_Mat[1][0] * perspective_Mat[2][1] * x - perspective_Mat[1][1] * perspective_Mat[2][0] * x);

    if (fabsf(denominator) < FLT_EPSILON) {
        pts_in[0] = 0;
        pts_in[1] = 0;
        return;
    }

    float inv_transformed_x = (perspective_Mat[0][1] * perspective_Mat[1][2] - perspective_Mat[0][1] * perspective_Mat[2][2] * y -
                            perspective_Mat[0][2] * perspective_Mat[1][1] + perspective_Mat[0][2] * perspective_Mat[2][1] * y +
                                perspective_Mat[1][1] * perspective_Mat[2][2] * x - perspective_Mat[1][2] * perspective_Mat[2][1] * x) / denominator;

    float inv_transformed_y = (-perspective_Mat[0][0] * perspective_Mat[1][2] + perspective_Mat[0][0] * perspective_Mat[2][2] * y +
                            perspective_Mat[0][2] * perspective_Mat[1][0] - perspective_Mat[0][2] * perspective_Mat[2][0] * y -
                            perspective_Mat[1][0] * perspective_Mat[2][2] * x + perspective_Mat[1][2] * perspective_Mat[2][0] * x) / denominator;

    int rounded_x = (int)roundf(inv_transformed_x);
    int rounded_y = (int)roundf(inv_transformed_y);

    pts_in[0] = (uint8)fmaxf(0.0f, fminf((float)(MT9V03X_W - 1), (float)rounded_x));
    pts_in[1] = (uint8)fmaxf(0.0f, fminf((float)(MT9V03X_H - 1), (float)rounded_y));
}


uint8 adaptiveThreshold(uint8 y,uint8 x,uint8 block,int8 clip_value)
{
    int8 half_block = block / 2;
    if(y < half_block || y > MT9V03X_H - half_block || x < half_block || x > MT9V03X_W - half_block)half_block = 1;
    int16 thres = 0;
    for(int8 dy = -half_block; dy <= half_block; dy++)
    {
        for(int8 dx=-half_block; dx<=half_block; dx++)
        {
            thres += Image[y+dy][x+dx];
        }
    }
    thres = thres / (block*block) - clip_value > 0 ? thres / (block*block) - clip_value : 0;
    return (uint8)thres;
}

uint8 Region_Otsu(uint8 x_start, uint8 x_end, uint8 y_start, uint8 y_end) {    
    if (x_start >= MT9V03X_W || x_end >= MT9V03X_W || y_start >= MT9V03X_H || y_end >= MT9V03X_H) {
        return 0;
    }
    if (x_start > x_end) {
        uint8 temp = x_start;
        x_start = x_end;
        x_end = temp;
    }
    if (y_start > y_end) {
        uint8 temp = y_start;
        y_start = y_end;
        y_end = temp;
    }

    #define GrayScale 256
    int HistGram[GrayScale] = {0};
    
    for (uint8 y = y_start; y <= y_end; y++) {
        for (uint8 x = x_start; x <= x_end; x++) {
            HistGram[Image[y][x]]++;
        }
    }
    
    uint8 MinValue = 0, MaxValue = 255;
    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++);
    for (MaxValue = 255; MaxValue > MinValue && HistGram[MaxValue] == 0; MaxValue--);
    
    if (MaxValue == MinValue) {
        return MaxValue;
    }
    if (MinValue + 1 == MaxValue) {
        return MinValue;
    }
    
    uint32 Amount = 0;
    for (uint16 y = MinValue; y <= MaxValue; y++) {
        Amount += HistGram[y];
    }
    
    uint32 PixelIntegral = 0;
    for (uint16 y = MinValue; y <= MaxValue; y++) {
        PixelIntegral += HistGram[y] * y;
    }
    
    double SigmaB = -1;
    uint8 Threshold = 0;
    uint32 PixelBack = 0;
    uint32 PixelIntegralBack = 0;
    
    for (uint16 y = MinValue; y < MaxValue; y++) {
        PixelBack += HistGram[y];
        uint32 PixelFore = Amount - PixelBack;
        
        if (PixelBack == 0) continue;
        if (PixelFore == 0) break;
        
        PixelIntegralBack += HistGram[y] * y;
        uint32 PixelIntegralFore = PixelIntegral - PixelIntegralBack;
        
        double OmegaBack = (double)PixelBack / Amount;
        double OmegaFore = (double)PixelFore / Amount;
        double MicroBack = (double)PixelIntegralBack / PixelBack;
        double MicroFore = (double)PixelIntegralFore / PixelFore;
        
        double Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);
        
        if (Sigma > SigmaB) {
            SigmaB = Sigma;
            Threshold = (uint8)y;
        }
    }

    return Threshold;
}


uint8 otsu_threshold(uint8 column_x) {
    if(column_x >= MT9V03X_W) {
        return 0;
    }
    
    return Region_Otsu(column_x, column_x, 0, MT9V03X_H - 1);
}

void seek_up(uint8 point[2], uint8 seed_kind)
{
    uint8 column_x = point[0];
    uint8 start_y = point[1];
    uint8 thres = 0;

    if(column_x >= MT9V03X_W || start_y == 0 || start_y >= MT9V03X_H) {
        return;
    }
    uint8 point1[2] = {column_x,start_y};
    TransformSinglePoint(point,point);
    point1[1] = point[1] - 60 > 0 ? point[1] - 60 : 0;
    ImagePerspective(point);
    uint8 otsu_x_start = (column_x > 4) ? (column_x - 4) : 0;
    uint8 otsu_x_end = (column_x < MT9V03X_W - 5) ? (column_x + 4) : (MT9V03X_W - 1);
    uint8 otsu_y_start = point1[1] - 20 > 0 ? point1[1] - 20 : 0;
    uint8 otsu_y_end = MT9V03X_H - 1;

    thres = Region_Otsu(otsu_x_start, otsu_x_end, otsu_y_start, otsu_y_end);

    for(uint8 i = start_y; i > 0; i--)
    {
        if(Image[i - 1][column_x] < thres)
        {
            if(seed_kind == left_pts)
            {
                Seed_left[0] = column_x;
                Seed_left[1] = i - 1;
            }
            else if(seed_kind == Right_pts)
            {
                Seed_right[0] = column_x;
                Seed_right[1] = i - 1;
            }
            else
            {
                 point[0] = column_x;
                 point[1] = i - 1;
            }
            break;
        }
    }
}

uint8 Seek_TOP(uint8 x)
{
    uint8 all_thres = 0;
    if(left_threshold > 0 && right_threshold > 0) {
        all_thres = (left_threshold + right_threshold) / 2;
    }else if(left_threshold > 0) {
        all_thres = left_threshold;
    }else if(right_threshold > 0) {
        all_thres = right_threshold;
    }
    for(uint8 i = Start_high - 1; i > 2; i--) {
        uint8 thres = (all_thres * 6 + Image[i][x] + Image[i + 1][x] + Image[i - 1][x]) / 9;
        if(Image[i - 1][x] < thres) {
            return i;
        }
    }
    return 0;
}

void Seek_Pts_Seed(uint8 High,uint8 mini_high)
{
    if(High >= MT9V03X_H || High < 1) {
        Seed_State = no_pts;
        return;
    }
    Seed_left[0] = 0;
    Seed_right[0] = 188;
    Seed_left[1] = High;
    Seed_right[1] = High;
    uint8 result = no_pts;
    low_threshold = Region_Otsu(0, MT9V03X_W - 1, mini_high, High);
    low_threshold = low_threshold > 21 ? low_threshold - 20 : 0;
    for(uint8 i = mid_position; i > 2; i--) {
        if(Image[High][i] < low_threshold) {
            Seed_left[0] = i + 1;
            result += left_pts;
            for(uint8 j = i; j > 0; j--)Image[High][j] = Image[High][i];
            break;
        }
    }
    for(uint8 i = mid_position; i < MT9V03X_W - 2; i++) {
        if(Image[High][i] < low_threshold) {
            Seed_right[0] = i - 1;
            result += Right_pts;
            for(uint8 j = i; j < MT9V03X_W; j++)Image[High][j] = Image[High][i];
            break;
        }
    } 
    if(result == two_pts && width_base == 0)width_base = Seed_right[0] - Seed_left[0];
    mid_position = (Seed_left[0] + Seed_right[0]) / 2;
    if(result != two_pts) {
        if(result == left_pts) {
            Seed_right[0] = 183;
            for(uint8 i = High - 1; i >= mini_high; i--) {
                if(Image[i][Seed_right[0]] < low_threshold) {
                    Seed_right[1] = i + 1;
                    result += Right_pts;
                    for(uint8 j = i; j >= mini_high; j--)Image[j][Seed_right[0]] = Image[i][Seed_right[0]];
                    break;
                }
            }
        } else if(result == Right_pts) {
            Seed_left[0] = 5;
            for(uint8 i = High - 1; i >= mini_high; i--) {
                if(Image[i][Seed_left[0]] < low_threshold) {
                    Seed_left[1] = i + 1;
                    result += left_pts;
                    for(uint8 j = i; j >= mini_high; j--)Image[j][Seed_left[0]] = Image[i][Seed_left[0]];
                    break;
                }
            }
        }
    }
    Seed_State = result;
    memcpy(original_left, Seed_left, sizeof(Seed_left));
    memcpy(original_right, Seed_right, sizeof(Seed_right));
}


int dir_front[4][2]={
        {0,-1},
        {1,0},
        {0,1},
        {-1,0}
};

int dir_frontleft[4][2]={
        {-1,-1},
        {1,-1},
        {1,1},
        {-1,1}
};

int dir_frontright[4][2]={
        {1,-1},
        {1,1},
        {-1,1},
        {-1,-1}
};

uint8 findline_lefthand_adaptive(uint8 block_size,uint8 clip_value,uint8 pts_arry[][2])
{
    uint8 half = block_size / 2;
    uint8 step = 0, dir = 0, turn = 0;
    uint8 thres = 0,time = 0;
    left_threshold = 0;
    while(step < MT9V03X_W - 2 && Seed_left[0] > 2 && Seed_left[0] < (MT9V03X_W - 2) && Seed_left[1] > 2 && Seed_left[1] < (MT9V03X_H - 2) && turn < 4 )
    {
        if(Seed_left[0] == original_right[0] && Seed_left[1] == original_right[1])
        {
            time++;
            if(time > 3)
            {
                step = 0;
                break;
            }
        }
        if(step == 1)
        {
            original_right[0] = Seed_left[0];
            original_right[1] = Seed_left[1];
        }
        if(turn==0){
            thres = adaptiveThreshold(Seed_left[1], Seed_left[0], block_size, clip_value);
            left_threshold += thres;
        }
        int front_value = Image[(Seed_left[1] + dir_front[dir][1])][(Seed_left[0] + dir_front[dir][0])];
        int frontleft_value = Image[(Seed_left[1] + dir_frontleft[dir][1])][(Seed_left[0] + dir_frontleft[dir][0])];

        if(front_value < thres)
        {
            dir = (dir+1) % 4;
            turn++;
        }
        else if(frontleft_value < thres)
        {
            Seed_left[0] += dir_front[dir][0];
            Seed_left[1] += dir_front[dir][1];
            pts_arry[step][0] = Seed_left[0];
            pts_arry[step][1] = Seed_left[1];
            step++;
            turn=0;
        }
        else{
            Seed_left[0] += dir_frontleft[dir][0];
            Seed_left[1] += dir_frontleft[dir][1];
            pts_arry[step][0] = Seed_left[0];
            pts_arry[step][1] = Seed_left[1];
            step++;
            turn=0;
            dir = (dir + 3) % 4;
        }
    }
    if(step > 0)left_threshold /= step;
    else left_threshold = 0;
    return step;
}

uint8 findline_righthand_adaptive(uint8 block_size,uint8 clip_value,uint8 pts_arry[][2])
{
    uint8 half = block_size / 2;
    uint8 step = 0, dir = 0, turn = 0;
    uint8 thres = 0,time = 0;
    right_threshold = 0;
    while(step < MT9V03X_W-2 && Seed_right[0] > 2 && Seed_right[0] < (MT9V03X_W - 2) && Seed_right[1] > 2 && Seed_right[1] < (MT9V03X_H - 2) && turn <4)
    {
        if(Seed_right[0] == original_left[0] && Seed_right[1] == original_left[1])
        {
            time++;
            if(time > 3)
            {
                step = 0;
                break;
            }
        }
        if(step == 1)
        {
            original_left[0] = Seed_right[0];
            original_left[1] = Seed_right[1];
        }
        if(turn==0){
            thres = adaptiveThreshold(Seed_right[1], Seed_right[0], block_size, clip_value);
            right_threshold += thres;
        }
        int front_value = Image[(Seed_right[1] + dir_front[dir][1])][(Seed_right[0] + dir_front[dir][0])];
        int frontright_value = Image[(Seed_right[1] + dir_frontright[dir][1])][(Seed_right[0] + dir_frontright[dir][0])];

        if(front_value < thres)
        {
            dir = (dir+3) % 4;
            turn++;
        }
        else if(frontright_value < thres)
        {
            Seed_right[0] += dir_front[dir][0];
            Seed_right[1] += dir_front[dir][1];
            pts_arry[step][0] = Seed_right[0];
            pts_arry[step][1] = Seed_right[1];
            step++;
            turn=0;
        }
        else{
            Seed_right[0] += dir_frontright[dir][0];
            Seed_right[1] += dir_frontright[dir][1];
            pts_arry[step][0] = Seed_right[0];
            pts_arry[step][1] = Seed_right[1];
            step++;
            turn=0;
            dir = (dir + 1) % 4;
        }
    }
    if(step > 0)right_threshold /= step;
    else right_threshold = 0;
    return step;
}

uint8 PtsPerspective(uint8 pts_in[][2], uint8 pts_out[][2], uint8 step, uint8 step_Max) {
    uint8 new_step = 0;
    for(uint8 i = 0; i < step; i++) {
        if(CalculateTransform(pts_in[i][1], pts_in[i][0], pts_out[i])) {
            new_step++;
        } else {
            break;
        }
        if(new_step >= step_Max) break;
    }
    return new_step;
}

uint8 TransformSinglePoint(uint8 pt_in[2], uint8 pt_out[2]) {
    return CalculateTransform(pt_in[1], pt_in[0], pt_out);
}


uint8 blur_pts_out[255][2];
uint8 blur_points(uint8 step, uint8 kernel, uint8 pts_in[][2]){
    int8 half = kernel / 2;
    memset(blur_pts_out, 0, sizeof(blur_pts_out));
    blur_pts_out[0][0] = pts_in[0][0];
    blur_pts_out[0][1] = pts_in[0][1];

    for (int i = 1; i < step - 1; i++) {
        int weight_sum = 0;
        int temp_x = 0;
        int temp_y = 0;

        for (int8 j = -half; j <= half; j++) {
            int valid_index = i + j;

            if (valid_index < 0) valid_index = 0;
            if (valid_index >= step) valid_index = step - 1;

            int weight = half + 1 - abs(j);
            temp_x += pts_in[valid_index][0] * weight;
            temp_y += pts_in[valid_index][1] * weight;
            weight_sum += weight;
        }

        blur_pts_out[i][0] = temp_x / weight_sum;
        blur_pts_out[i][1] = temp_y / weight_sum;
    }

    blur_pts_out[step - 1][0] = pts_in[step - 1][0];
    blur_pts_out[step - 1][1] = pts_in[step - 1][1];
    return Arry_Copy(blur_pts_out,pts_in,step);
}

#define MAX_RESAMPLE_POINTS 255

uint8 resample_points(uint8 pts_in[][2], uint8 step1, uint8 step2, uint8 dist) {
    uint8 resample_pts_out[MAX_RESAMPLE_POINTS][2] = {0};
    float target_dist = (float)dist;
    float accumulated_dist = 0.0f;
    uint8 len = 0;

    if (step1 < 2 || step2 == 0 || dist == 0) {
        return 0;
    }

    if (len < step2 && len < MAX_RESAMPLE_POINTS) {
        resample_pts_out[len][0] = pts_in[0][0];
        resample_pts_out[len][1] = pts_in[0][1];
        len++;
    }

    for (uint8 i = 0; i < step1 - 1 && len < step2; i++) {
        float x0 = (float)pts_in[i][0];
        float y0 = (float)pts_in[i][1];
        float x1 = (float)pts_in[i + 1][0];
        float y1 = (float)pts_in[i + 1][1];

        float dx = x1 - x0;
        float dy = y1 - y0;
        float segment_len = SquareRootFloat(dx * dx + dy * dy);

        if (segment_len < FLT_EPSILON) continue;

        float nx = dx / segment_len;
        float ny = dy / segment_len;

        float dist_to_next_sample = target_dist - accumulated_dist;

        while (dist_to_next_sample <= segment_len && len < step2) {
            float sample_x = x0 + nx * dist_to_next_sample;
            float sample_y = y0 + ny * dist_to_next_sample;

            int rounded_x = (int)roundf(sample_x);
            int rounded_y = (int)roundf(sample_y);

            if (len < MAX_RESAMPLE_POINTS &&
                rounded_x >= 0  && rounded_y >= 0 )
            {
                resample_pts_out[len][0] = (uint8)rounded_x;
                resample_pts_out[len][1] = (uint8)rounded_y;
                len++;
            } else {
                goto end_resampling;
            }

            accumulated_dist = 0.0f;
            x0 = sample_x;
            y0 = sample_y;
            segment_len -= dist_to_next_sample;
            dist_to_next_sample = target_dist;
        }

        accumulated_dist += segment_len;
    }

end_resampling:
    for (uint8 i = 0; i < len; i++) {
        pts_in[i][0] = resample_pts_out[i][0];
        pts_in[i][1] = resample_pts_out[i][1];
    }

    for (uint8 i = len; i < step1; i++) {
        pts_in[i][0] = 0;
        pts_in[i][1] = 0;
    }

    return len;
}

void track_leftpoint(uint8 pts_in[][2], uint8 num, uint8 ID, uint8 pts_out[2], int8 approx_num, uint8 dist) {
    uint8 i = 0;
    float dx = pts_in[clip(ID + approx_num, 0, num - 1)][0] - pts_in[clip(ID - approx_num, 0, num - 1)][0];
    float dy = pts_in[clip(ID + approx_num, 0, num - 1)][1] - pts_in[clip(ID - approx_num, 0, num - 1)][1];
    float dn = SquareRootFloat(dx * dx + dy * dy);
    if(dn == 0)
    {
        pts_out[0] = 0;
        pts_out[1] = 0;
        return;
    }
    dx /= dn;
    dy /= dn;
    pts_out[0] = pts_in[ID][0] - dy * dist;
    pts_out[1] = pts_in[ID][1] + dx * dist;
}


void track_rightpoint(uint8 pts_in[][2], uint8 num, uint8 ID, uint8 pts_out[2], int8 approx_num, uint8 dist) {
    uint8 i = 0;
    float dx = pts_in[clip(ID + approx_num, 0, num - 1)][0] - pts_in[clip(ID - approx_num, 0, num - 1)][0];
    float dy = pts_in[clip(ID + approx_num, 0, num - 1)][1] - pts_in[clip(ID - approx_num, 0, num - 1)][1];
    float dn = SquareRootFloat(dx * dx + dy * dy);
    if(dn == 0)
    {
        pts_out[0] = 0;
        pts_out[1] = 0;
        return;
    }
    dx /= dn;
    dy /= dn;
    pts_out[0] = pts_in[ID][0] + dy * dist;
    pts_out[1] = pts_in[ID][1] - dx * dist;
}

uint8 track_leftline(uint8 pts_in[][2], uint8 num, uint8 pts_out[][2], int8 approx_num, uint8 dist) {
    uint8 i = 0;
    for (i = 0; i < num - 1; i++) {
        if(pts_in[i][0] < 0 || pts_in[i][0] >= MT9V03X_W || pts_in[i][1] < 0 || pts_in[i][1] >= MT9V03X_H)break;
        float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
        float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
        float dn = SquareRootFloat(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        uint8 x = pts_in[i][0] - dy * dist;
        uint8 y = pts_in[i][1] + dx * dist;
        if(x == 0 && y == 0)break;
        pts_out[i][0] = x;
        pts_out[i][1] = y;
    }
    return i;
}


uint8 track_rightline(uint8 pts_in[][2], uint8 num, uint8 pts_out[][2], int8 approx_num, uint8 dist) {
    uint8 i = 0;
    for (i = 0; i < num - 1; i++) {
        if(pts_in[i][0] < 0 || pts_in[i][0] >= MT9V03X_W || pts_in[i][1] < 0 || pts_in[i][1] >= MT9V03X_H)break;
        float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
        float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
        float dn = SquareRootFloat(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        uint8 x = pts_in[i][0] + dy * dist;
        uint8 y = pts_in[i][1] - dx * dist;
        if(x == 0 && y == 0)break;
        pts_out[i][0] = x;
        pts_out[i][1] = y;
    }
    return i;
}

float local_angle_points(uint8 pts_in[][2], uint8 num, uint8 ID,uint8 dist){
    float dx1 = pts_in[ID][0] - pts_in[clip(ID - dist, 0, num - 1)][0];
    float dy1 = pts_in[ID][1] - pts_in[clip(ID - dist, 0, num - 1)][1];
    float dn1 = SquareRootFloat(dx1 * dx1 + dy1 * dy1);
    float dx2 = pts_in[clip(ID + dist, 0, num - 1)][0] - pts_in[ID][0];
    float dy2 = pts_in[clip(ID + dist, 0, num - 1)][1] - pts_in[ID][1];
    float dn2 = SquareRootFloat(dx2 * dx2 + dy2 * dy2);
    float c1 = dx1 / dn1;
    float s1 = dy1 / dn1;
    float c2 = dx2 / dn2;
    float s2 = dy2 / dn2;
    return atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);
}

void nms_angle(float angle_in[], int num, float angle_out[], int kernel){
    int half = kernel / 2;
    for (int i = 0; i < num; i++) {
        angle_out[i] = angle_in[i];
        for (int j = -half; j <= half; j++) {
            if (fabs(angle_in[clip(i + j, 0, num - 1)]) > fabs(angle_out[i])) {
                angle_out[i] = 0;
                break;
            }
        }
    }
}

float compute_curvature(uint8 P0[2], uint8 P1[2], uint8 P2[2]) {
    float cross =
        (float)(P1[0] - P0[0]) * (float)(P2[1] - P0[1]) -
        (float)(P2[0] - P0[0]) * (float)(P1[1] - P0[1]);

    float area = 0.5 * fabs(cross);

    if (area < 1e-8) {
        return 0.0;
    }

    float dx, dy;

    dx = (float)(P2[0] - P1[0]);
    dy = (float)(P2[1] - P1[1]);
    float a = sqrt(dx * dx + dy * dy);

    dx = (float)(P2[0] - P0[0]);
    dy = (float)(P2[1] - P0[1]);
    float b = sqrt(dx * dx + dy * dy);

    dx = (float)(P1[0] - P0[0]);
    dy = (float)(P1[1] - P0[1]);
    float c = sqrt(dx * dx + dy * dy);

    float radius = (a * b * c) / (4 * area);

    return (radius != 0) ? 1.0 / radius : 0.0;
}

uint8 Get_high(uint8 pts_in[][2],uint8 num)
{
    uint8 high = Start_high;
    for(uint8 i = 0; i < num; i++)
    {
        if(pts_in[i][1] < high)high = pts_in[i][1];
    }
    return high;
}


float Get_Distance(uint8 pt1[2], uint8 pt2[2]) {
    int dx = (int)pt1[0] - pt2[0];
    int dy = (int)pt1[1] - pt2[1];
    return SquareRootFloat(dx*dx + dy*dy);
}


uint8 addline(uint8 location1[2],uint8 location2[2],uint8 pts[][2],uint8 add_location,uint8 dist)
{
    float x0 = location1[0];
    float y0 = location1[1];
    float dx = location2[0] - x0;
    float dy = location2[1] - y0;
    float dn = SquareRootFloat(dx * dx + dy * dy);
    dx /= dn;
    dy /= dn;

    int len = 0,remain = dist;
    while (remain < dn && len < (MT9V03X_W - add_location)) {
        x0 += dx * remain;
        y0 += dy * remain;

        if(x0 < 0 || y0 < 0) break;

        pts[add_location + len][0] = (uint8)x0;
        pts[add_location + len][1] = (uint8)y0;
        len++;
        dn -= remain;
    }
    return len;
}



uint8 quan_speed_float[11]={1,2,2,3,4,5,4,3,2,2,1};
uint8 Index = 0;
float Get_error(MidlineData *midline,int speed, float speed_decision)
{
    static float last_error = 0;
    static int speed_temp[20] = {0};
    speed_temp[0] = speed;
    int sum_speed = speed;
    for(uint8 i = 19; i > 0; i--) {
        speed_temp[i] = speed_temp[i-1];
        sum_speed += speed_temp[i];
    }
    int now_speed = sum_speed / 20;
    uint8 target_dist = now_speed * speed_decision;
    uint8 target_idx = midline->step - 1;

    for(uint8 i = 0; i < midline->step; i++){
        if(midline->dist[i] > target_dist){
            target_idx = i;
            break;
        }
    }
    Index = target_idx;
    int error = 0;
    if (target_idx < midline->step) {
        error = midline->pts[target_idx][0] - 94;
    } else {
        error = last_error;
    }
    last_error = error;
    return (float)error;
}

uint8 add_three_point_bezier(uint8 location1[2], uint8 location2[2], uint8 location3[2],
                             uint8 pts[][2], uint8 add_location, uint8 dist)
{
    if (add_location >= MT9V03X_W) {
        return 0;
    }

    float dx1 = location2[0] - location1[0];
    float dy1 = location2[1] - location1[1];
    float len1 = SquareRootFloat(dx1 * dx1 + dy1 * dy1);

    float dx2 = location3[0] - location2[0];
    float dy2 = location3[1] - location2[1];
    float len2 = SquareRootFloat(dx2 * dx2 + dy2 * dy2);

    float curve_length = (len1 + len2) * 0.8f;

    uint8 num_points = curve_length / dist + 1;
    if (num_points > (MT9V03X_W - add_location)) {
        num_points = MT9V03X_W - add_location;
    }

    float step = 1.0f / (num_points - 1);
    float t = 0.0f;
    uint8 len = 0;
    float x, y, prev_x = 0, prev_y = 0;
    float current_dist = 0;

    pts[add_location + len][0] = location1[0];
    pts[add_location + len][1] = location1[1];
    prev_x = location1[0];
    prev_y = location1[1];
    len++;

    t = step;
    while (t <= 1.0f && len < (MT9V03X_W - add_location)) {
        float t_inv = 1.0f - t;
        float t_inv_squared = t_inv * t_inv;
        float t_squared = t * t;

        x = t_inv_squared * location1[0] + 2 * t_inv * t * location2[0] + t_squared * location3[0];
        y = t_inv_squared * location1[1] + 2 * t_inv * t * location2[1] + t_squared * location3[1];

        float dx = x - prev_x;
        float dy = y - prev_y;
        current_dist = SquareRootFloat(dx * dx + dy * dy);

        if (current_dist >= dist) {
            if (x < 0 || x >= MT9V03X_W || y < 0 || y >= MT9V03X_H) break;

            pts[add_location + len][0] = (uint8)x;
            pts[add_location + len][1] = (uint8)y;
            len++;

            prev_x = x;
            prev_y = y;
            current_dist = 0;
        }

        t += step;
    }

    if (len < (MT9V03X_W - add_location) &&
        !(location3[0] < 0 || location3[0] >= MT9V03X_W || location3[1] < 0 || location3[1] >= MT9V03X_H)) {
        pts[add_location + len][0] = location3[0];
        pts[add_location + len][1] = location3[1];
        len++;
    }

    return len;
}

void blur(uint8 x,uint8 y)
{
    if(x > 1 && x < 187)
    {
        Image[y][x]=(1 * Image[y - 1][x-1] + 2 * Image[y - 1][x] + 1 * Image[y - 1][x+1] +
                                2 * Image[y][x-1] + 4 * Image[y][x] + 2 * Image[y][x+1] +
                                1 * Image[y+1][x-1] + 2 * Image[y+1][x] + 1 * Image[y+1][x+1]) / 16;
    }
}

