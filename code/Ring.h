#ifndef _Ring_h_
#define _Ring_h_

#include "zf_common_typedef.h"
#include "stdbool.h"
#include "PTS_Deal.h"

// ����״̬����
typedef enum
{
    Ring_state1,    // ��⻷�����
    Ring_state2,    // ȷ�Ͻ��뻷��
    Ring_state3,    // �����ڲ�����׶�1
    Ring_state4,    // �����ڲ�����׶�2
    Ring_state5,    // �������ڴ���
    Ring_state6,    // ������������׶�1
    Ring_state7,    // ������������׶�2
    Ring_state8,    // �������˳�
    Ring_state9,    // ������ȫ�˳�
}ring_state;

// �������Ͷ���
typedef enum
{
    no_ring,        // �޻���
    left_ring,      // �󻷵�
    right_ring,     // �һ���
}ring_kind;

// ��������״̬�ṹ��
typedef struct {
    ring_state state;       // ��ǰ����״̬
    ring_kind kind;         // ��������
    uint8 phase_counter;    // �׶μ�����
    float sum_zangle;       // �ۼƽǶ�
    uint8 last_high;
    uint32 time_counter;  // ʱ�������
    uint8 flag;            // ��־λ
} RingState;

// ������ʼ���ṹ��
typedef struct {
    bool is_left;
    struct {
        BoundaryData *current;
        BoundaryData *opposite;
        FBoundaryData *Fcurrent;
        FBoundaryData *Fopposite;
    } bounds;
} RingContext;


#include "zf_common_headfile.h"

// ��������
#define MIN_POINTS 20
#define PHASE_COUNT_THRESHOLD 3
#define ANGLE_THRESHOLD_LARGE 300.0f
#define ANGLE_THRESHOLD_MEDIUM 45.0f
#define ANGLE_THRESHOLD_SMALL 30.0f
#define WIDTH_THRESHOLD (Step_Max >> 1)
#define WIDTH_THRESHOLD_SMALL (Step_Max / 3)

extern RingState ring_ctrl;
extern uint8 IDID;

// ��������
void Ring_Process(void);
void Ring_evolve(void);
void Ring_Out(void);
void Ring_end(void);
#endif
