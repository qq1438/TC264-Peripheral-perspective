#ifndef _image_processing_h_
#define _image_processing_h_

#include "PTS_Deal.h"
#include "zf_common_headfile.h"

/**
 * @brief ���ӵ�״̬ö��
 * @details
 *   - no_pts: δ�ҵ����ӵ�
 *   - left_pts: ���ҵ������ӵ�
 *   - Right_pts: ���ҵ������ӵ�
 *   - two_pts: ͬʱ�ҵ��������ӵ�
 */
typedef enum
{
    no_pts,      ///< δ�ҵ����ӵ�
    left_pts,    ///< ���ҵ������ӵ�
    Right_pts,   ///< ���ҵ������ӵ�
    two_pts,     ///< ͬʱ�ҵ��������ӵ�
} Pts_state;

/**
 * @brief ͼ��ԭʼ���ݣ��Ҷ�ͼ�񣬷ֱ���ΪMT9V03X_H x MT9V03X_W
 */
extern uint8 Image[MT9V03X_H][MT9V03X_W];

/**
 * @brief ��ǰ���ӵ�״̬���μ�Pts_state
 */
extern uint8 Seed_State;

/**
 * @brief ������ӵ�ԭʼ���꣬original_left[0]=x, original_left[1]=y
 */
extern uint8 original_left[2];

/**
 * @brief �Ҳ����ӵ�ԭʼ���꣬original_right[0]=x, original_right[1]=y
 */
extern uint8 original_right[2];

/**
 * @brief ͼ������λ�ã������꣩
 */
extern uint8 mid_position;

/**
 * @brief ���߽�����Ӧ��ֵ
 */
extern uint32 left_threshold;

/**
 * @brief �Ҳ�߽�����Ӧ��ֵ
 */
extern uint32 right_threshold;

/**
 * @brief ����ԭʼͼ�񵽴�������
 */
void Image_Copy(void);

/**
 * @brief ��ʼ����͸�ӱ任����
 * @param change_un_Mat �����3x3�任����
 */
void ImagePerspective_Init(float change_un_Mat[3][3]);

/**
 * @brief ������任��ĵ�����
 * @param y ������������
 * @param x ������������
 * @param output ��������꣬output[0]=x, output[1]=y
 */
void CalculateTransform_inverse(uint8 y, uint8 x, uint8 output[2]);

/**
 * @brief ������������͸�ӱ任�����д��pts_in
 * @param pts_in ��������㣬pts_in[0]=x, pts_in[1]=y
 */
void ImagePerspective(uint8 pts_in[2]);

/**
 * @brief ��һ��������͸�ӱ任
 * @param pts_in ����㼯����ά���飬ÿ��Ϊ[x, y]
 * @param pts_out ����㼯����ά���飬ÿ��Ϊ[x, y]
 * @param step ����������
 * @param step_Max �������������
 * @retval ʵ���������
 */
uint8 PtsPerspective(uint8 pts_in[][2], uint8 pts_out[][2], uint8 step, uint8 step_Max);

/**
 * @brief �Ե��������͸�ӱ任
 * @param pt_in ����� [x, y]
 * @param pt_out ����� [x, y]
 * @retval 0-�ɹ�������-ʧ��
 */
uint8 TransformSinglePoint(uint8 pt_in[2], uint8 pt_out[2]);

/**
 * @brief �������任��ĵ�����
 * @param y ������������
 * @param x ������������
 * @param output ��������꣬output[0]=x, output[1]=y
 * @retval 0-�ɹ�������-ʧ��
 */
uint8 CalculateTransform(uint8 y, uint8 x, uint8 output[2]);

/**
 * @brief ����Ӧ��ֵ����
 * @param y ������
 * @param x ������
 * @param block ��ֵ���㴰�ڴ�С
 * @param clip_value ��ֵ����ֵ
 * @retval ��ֵ
 */
uint8 adaptiveThreshold(uint8 y, uint8 x, uint8 block, int8 clip_value);

/**
 * @brief ��ָ�������ģ������
 * @param x ������
 * @param y ������
 */
void blur(uint8 x, uint8 y);

/**
 * @brief Ѱ���������ӵ�
 * @param High ��ʼ��
 * @param mini_high ��С��
 */
void Seek_Pts_Seed(uint8 High, uint8 mini_high);

/**
 * @brief ��ָ����������Ѱ�����ӵ�
 * @param point ��ʼ�� [x, y]
 * @param seed_kind ���ӵ����ͣ�left_pts��Right_pts��
 */
void seek_up(uint8 point[2], uint8 seed_kind);

/**
 * @brief ����Ӧ��ֵ��߽�Ѱ��
 * @param block_size ��ֵ���ڴ�С
 * @param clip_value ��ֵ����ֵ
 * @param pts_arry ����㼯
 * @retval ʵ���ҵ��ĵ���
 */
uint8 findline_lefthand_adaptive(uint8 block_size, uint8 clip_value, uint8 pts_arry[][2]);

/**
 * @brief ����Ӧ��ֵ�ұ߽�Ѱ��
 * @param block_size ��ֵ���ڴ�С
 * @param clip_value ��ֵ����ֵ
 * @param pts_arry ����㼯
 * @retval ʵ���ҵ��ĵ���
 */
uint8 findline_righthand_adaptive(uint8 block_size, uint8 clip_value, uint8 pts_arry[][2]);

/**
 * @brief �Ե㼯����ģ���˲�
 * @param step �㼯����
 * @param kernel �˲��˴�С
 * @param pts_in ����㼯
 * @retval ʵ���������
 */
uint8 blur_points(uint8 step, uint8 kernel, uint8 pts_in[][2]);

/**
 * @brief ����㼯ĳ��ľֲ��Ƕ�
 * @param pts_in ����㼯
 * @param num �㼯����
 * @param ID ��ǰ������
 * @param dist ǰ�����
 * @retval �ֲ��Ƕȣ����ȣ�
 */
float local_angle_points(uint8 pts_in[][2], uint8 num, uint8 ID, uint8 dist);

/**
 * @brief ������������
 * @param P0 ��һ���� [x, y]
 * @param P1 �ڶ����� [x, y]
 * @param P2 �������� [x, y]
 * @retval ����ֵ
 */
float compute_curvature(uint8 P0[2], uint8 P1[2], uint8 P2[2]);

/**
 * @brief ��ȡ�㼯�е����������
 * @param pts_in ����㼯
 * @param num �㼯����
 * @retval ���������
 */
uint8 Get_high(uint8 pts_in[][2], uint8 num);

/**
 * @brief ���������ŷ�Ͼ���
 * @param pt1 ��һ���� [x, y]
 * @param pt2 �ڶ����� [x, y]
 * @retval ����
 */
float Get_Distance(uint8 pt1[2], uint8 pt2[2]);

/**
 * @brief �ԽǶ����н��зǼ���ֵ����
 * @param angle_in ����Ƕ�����
 * @param num ���鳤��
 * @param angle_out ����Ƕ�����
 * @param kernel ���ƺ˴�С
 */
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);

/**
 * @brief ��ָ������Canny��Ե���Ѱ�ұ߽�
 * @param row �к�
 * @param start_x ��ʼ��
 * @param direction ����0-��1-�ң�
 * @retval �߽��������
 */
uint8 find_edge_canny_row(uint8 row, uint8 start_x, uint8 direction);

/**
 * @brief ��ָ������Canny��Ե���Ѱ�ұ߽�
 * @param col �к�
 * @param start_y ��ʼ��
 * @param direction ����0-�ϣ�1-�£�
 * @retval �߽��������
 */
uint8 find_edge_canny_col(uint8 col, uint8 start_y, uint8 direction);

#endif // _image_processing_h_
