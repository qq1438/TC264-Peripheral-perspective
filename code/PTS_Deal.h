#ifndef _PTS_Deal_h_
#define _PTS_Deal_h_

#include "zf_common_typedef.h"
#include "zf_device_mt9v03x.h"
#include "stdbool.h"

// ���ӹյ�����ӳ��ṹ��
typedef struct {
    uint8 original[2];  // ԭʼ����
    uint8 transformed[2]; // �任������
    uint8 now_index;     // ��Now�����е�����
    uint8 original_index; // ��Original�����е�����
} CornerMapping;
// �����߽����ݽṹ��
typedef struct {
    uint8 original_step;
    uint8 now_step;
    uint8 now_step_original;  // ����ԭʼ����
    uint8 original_pts[MT9V03X_W][2];
    uint8 now_pts[MT9V03X_H][2];
    CornerMapping map[3];
    uint8 Lp_id;
    bool is_straight;
    bool Lp_state;
    bool is_lost;
} BoundaryData;

typedef struct {
    uint8 original_step;
    uint8 original_pts[MT9V03X_W][2];
    uint8 now_step;
    uint8 now_pts[MT9V03X_H][2];
} FBoundaryData;

// �м������ݽṹ��
typedef struct {
    uint8 pts[MT9V03X_W][2];
    uint8 dist[MT9V03X_W];
    uint8 step;
    int8 mid_dist;
} MidlineData;

#include "zf_common_headfile.h"

#define WIDTH 40
#define Step_Max 40
#define resam_point 3
#define Start_high 100
#define Mini_high 75

extern uint8 Mid_Pts[MT9V03X_H][2];
extern float MID_Error;
extern float KMID_Error;
extern bool long_state;
extern float curve_mid;
extern uint8 high;



// ����ȫ�ֱ�������
extern BoundaryData left_boundary;
extern BoundaryData right_boundary;
extern FBoundaryData Fleft_boundary;
extern FBoundaryData Fright_boundary;
extern MidlineData midline;
extern volatile bool is_compress;

void LeftPts_Deal_Original(void);
void LeftPts_Deal_Now(void);
void RightPts_Deal_Original(void);
void RightPts_Deal_Now(void);
void FLeftPts_Deal_Original(void);
void FRightPts_Deal_Original(void);

void Pts_Deal(void);
void WITE(void);
void Pts_Test(void);


#endif
