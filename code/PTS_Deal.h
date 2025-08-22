#ifndef _PTS_Deal_h_
#define _PTS_Deal_h_

#include "zf_common_typedef.h"
#include "zf_device_mt9v03x.h"
#include "stdbool.h"

/**
 * @brief �յ�����ӳ��ṹ��
 * @details ���ڴ洢��������ԭʼ�㼯���任��㼯�Լ���ǰ�㼯�е�������Ϣ
 * @var CornerMapping::original ԭʼ�㼯�е����� (x, y)
 * @var CornerMapping::transformed �任��㼯�е����� (x, y)
 * @var CornerMapping::now_index �任����ڵ�ǰ�㼯�е�����
 * @var CornerMapping::original_index �õ���ԭʼ�㼯�е�����
 */
typedef struct {
    uint8 original[2];      ///< ԭʼ����
    uint8 transformed[2];   ///< �任������
    uint8 now_index;        ///< ��Now�����е�����
    uint8 original_index;   ///< ��Original�����е�����
} CornerMapping;

/**
 * @brief �߽����ݽṹ��
 * @details �洢�߽��ԭʼ�㡢����㡢������ӳ�估���״̬��Ϣ
 * @var BoundaryData::original_step ԭʼ�㼯����
 * @var BoundaryData::now_step ��ǰ�㼯����
 * @var BoundaryData::now_step_original ����ԭʼ����
 * @var BoundaryData::original_pts ԭʼ�㼯 (ÿ��Ϊһ�����x, y)
 * @var BoundaryData::now_pts ��ǰ�㼯 (ÿ��Ϊһ�����x, y)
 * @var BoundaryData::map ������ӳ������
 * @var BoundaryData::Lp_id �ǵ�ID
 * @var BoundaryData::angle ��ǰ�߽�Ƕ�
 * @var BoundaryData::is_straight �Ƿ�Ϊֱ�߶�
 * @var BoundaryData::Lp_state �ǵ�״̬
 * @var BoundaryData::is_lost ���߱�־
 */
typedef struct {
    uint8 original_step;
    uint8 now_step;
    uint8 now_step_original;
    uint8 original_pts[MT9V03X_W][2];
    uint8 now_pts[MT9V03X_H][2];
    CornerMapping map[3];
    uint8 Lp_id;
    float angle;
    bool is_straight;
    bool Lp_state;
    bool is_lost;
} BoundaryData;

/**
 * @brief ǰ�ӱ߽����ݽṹ��
 * @details �洢ǰ�ӱ߽��ԭʼ��ʹ����
 * @var FBoundaryData::original_step ԭʼ�㼯����
 * @var FBoundaryData::original_pts ԭʼ�㼯 (ÿ��Ϊһ�����x, y)
 * @var FBoundaryData::now_step ��ǰ�㼯����
 * @var FBoundaryData::now_pts ��ǰ�㼯 (ÿ��Ϊһ�����x, y)
 */
typedef struct {
    uint8 original_step;
    uint8 original_pts[MT9V03X_W][2];
    uint8 now_step;
    uint8 now_pts[MT9V03X_H][2];
} FBoundaryData;

#include "zf_common_headfile.h"

#define WIDTH 40         ///< ����㼯�����
#define Step_Max 40      ///< �㼯��󲽳�
#define Start_high 115   ///< ��ʼ�߶�
#define Mini_high 90     ///< ��С�߶�

/**
 * @brief ��߽����ݽṹ�壬�洢��߽��ԭʼ�㡢��������Ϣ
 */
extern BoundaryData left_boundary;

/**
 * @brief �ұ߽����ݽṹ�壬�洢�ұ߽��ԭʼ�㡢��������Ϣ
 */
extern BoundaryData right_boundary;

/**
 * @brief ǰ����߽����ݽṹ�壬�洢ǰ����߽��ԭʼ�㡢��������Ϣ
 */
extern FBoundaryData Fleft_boundary;

/**
 * @brief ǰ���ұ߽����ݽṹ�壬�洢ǰ���ұ߽��ԭʼ�㡢��������Ϣ
 */
extern FBoundaryData Fright_boundary;

/**
 * @brief ������߽�ԭʼ�㣬��ȡ��߽�ԭʼ�㼯
 * @note �������left_boundary.original_pts����������left_boundary.original_step
 */
void LeftPts_Deal_Original(void);

/**
 * @brief �����ұ߽�ԭʼ�㣬��ȡ�ұ߽�ԭʼ�㼯
 * @note �������right_boundary.original_pts����������right_boundary.original_step
 */
void RightPts_Deal_Original(void);

/**
 * @brief ����ǰ����߽�ԭʼ�㣬��ȡǰ����߽�ԭʼ�㼯
 * @note �������Fleft_boundary.original_pts����������Fleft_boundary.original_step
 */
void FLeftPts_Deal_Original(void);

/**
 * @brief ����ǰ���ұ߽�ԭʼ�㣬��ȡǰ���ұ߽�ԭʼ�㼯
 * @note �������Fright_boundary.original_pts����������Fright_boundary.original_step
 */
void FRightPts_Deal_Original(void);

#endif
