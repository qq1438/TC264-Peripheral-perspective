#ifndef CROSS_H
#define CROSS_H

#include "zf_common_typedef.h"
#include "stdbool.h"
#include "PTS_Deal.h"

typedef enum
{
    no_cross,        // �޽����
    mid_cross,       // �м佻���
}cross_kind;

typedef enum
{
    cross_state1,    
    cross_state2,      
}cross_state;


// ��������״̬�ṹ��
typedef struct {
    cross_kind kind;         // ���������
    cross_state state;            // �����״̬
    uint8 left_x;
    uint8 left_y;
    uint8 right_x;
    uint8 right_y;
    uint8 phase_counter;    // �׶μ�����
    uint32 sum_distance;       // �ۼƾ���
    uint8 flag;            // ��־λ

} CrossState;


#include "zf_common_headfile.h"

#define CROSS_single_high 10
#define CROSS_single_width 10
#define MIN_Point 30
#define Cross_Count_Threshold 3
#define CROSS_OFFSET 10

extern CrossState cross_ctrl;

void cross_process(void);

#endif
