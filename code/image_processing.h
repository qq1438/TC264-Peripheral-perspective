#ifndef _image_processing_h_
#define _image_processing_h_

//==================================================================================
// ���Ͷ���Ͱ����ļ�
//==================================================================================
#include "PTS_Deal.h" // ���� PTS_Deal.h
#include "zf_common_headfile.h" // ��������ͷ�ļ�

typedef enum
{
    no_pts ,    // ��Ч��״̬
    left_pts,   // ��߽��״̬
    Right_pts,  // �ұ߽��״̬
    two_pts,    // ˫�߽��״̬
}Pts_state;


//==================================================================================
// �ⲿ��������
//==================================================================================
extern uint8 Image[MT9V03X_H][MT9V03X_W]; // ԭʼͼ������
extern uint8 Seed_State;                 // ���ӵ�״̬
extern uint8 original_left[2];           // ԭʼ��߽��
extern uint8 original_right[2];          // ԭʼ�ұ߽��
extern uint8 mid_position;               // ����λ��
extern uint8 Index;                      // ��������
extern uint8 width_base;                 // �������
extern uint32 left_threshold;            // ��߽���ֵ
extern uint32 right_threshold;           // �ұ߽���ֵ
extern uint8 low_threshold;              // �ײ���ֵ

//==================================================================================
// ͼ�����������
//==================================================================================
/**
 * @brief ����ͼ������
 */
void Image_Copy(void);


//==================================================================================
// ��͸�ӱ任��غ��� (Image Coordinate -> Bird's-Eye View)
//==================================================================================
/**
 * @brief ��ʼ��ͼ����͸�ӱ任����
 * @param change_un_Mat 3x3 ����͸�ӱ任����
 */
void ImagePerspective_Init(float change_un_Mat[3][3]);

/**
 * @brief ������ĵ����������͸�ӱ任
 * @param y ������ Y ����
 * @param x ������ X ����
 * @param output ����任����������� [x, y]
 */
void CalculateTransform_inverse(uint8 y, uint8 x, uint8 output[2]);

/**
 * @brief ������ĵ��������͸�ӱ任 (ʹ��Ԥ�������ԭ���޸�)
 * @param pts_in ������ͼ�����ͼ���� [x, y]���任���ԭͼ���� [x, y] �����Ǵ�����
 */
void ImagePerspective(uint8 pts_in[2]);

/**
 * @brief �Ե㼯������͸�ӱ任
 * @param pts_in ����㼯ͼ����������
 * @param pts_out ����任��ĵ㼯���ͼ��������
 * @param step ����㼯����Ч����
 * @param step_Max pts_in �� pts_out ������������
 * @return uint8 �任�����Ч����
 */
uint8 PtsPerspective(uint8 pts_in[][2], uint8 pts_out[][2], uint8 step, uint8 step_Max);

/**
 * @brief �Ե����������͸�ӱ任 (ͨ������ CalculateTransform)
 * @param pt_in ������ͼ���������� [x, y]
 * @param pt_out ����任��ĵ����ͼ�������� [x, y]
 * @return uint8 CalculateTransform �ķ���ֵ (״̬����־)
 */
uint8 TransformSinglePoint(uint8 pt_in[2], uint8 pt_out[2]);

/**
 * @brief ��������任 (��������͸�ӱ任�ĺ���ʵ��)
 * @param y ������ Y ����
 * @param x ������ X ����
 * @param output ����任����������� [x, y]
 * @return uint8 ״̬����־
 */
uint8 CalculateTransform(uint8 y, uint8 x, uint8 output[2]);


//==================================================================================
// ͼ����ֵ����
//==================================================================================
/**
 * @brief ��ͼ��ָ��������д�� (Otsu) ��ֵ����
 * @param x_start ������ʼ X ����
 * @param x_end ������� X ����
 * @param y_start ������ʼ Y ����
 * @param y_end ������� Y ����
 * @return uint8 ����õ�����ֵ
 */
uint8 Region_Otsu(uint8 x_start, uint8 x_end, uint8 y_start, uint8 y_end);

/**
 * @brief ����ָ���еĴ�� (Otsu) ��ֵ
 * @param column_x ��Ҫ������ֵ��ͼ��������
 * @return uint8 ����õ�����ֵ
 */
uint8 otsu_threshold(uint8 column_x);

/**
 * @brief ��������Ӧ��ֵ
 * @param y ���ص� Y ����
 * @param x ���ص� X ����
 * @param block ����ֲ���ֵ��������С (�߳�)
 * @param clip_value ��ֵ�����Ĳü�ֵ
 * @return uint8 ����õ�������Ӧ��ֵ
 */
uint8 adaptiveThreshold(uint8 y, uint8 x, uint8 block, int8 clip_value);

/**
 * @brief ��ͼ����и�˹ģ������
 * @param x ���ص� X ����
 * @param y ���ص� Y ����
 */
void blur(uint8 x,uint8 y);


//==================================================================================
// ���ӵ�ͱ߽�����
//==================================================================================
/**
 * @brief ��ָ���߶ȷ�Χ��Ѱ�����ӵ�
 * @param High ������������� Y ����
 * @param mini_high ������������� Y ����
 */
void Seek_Pts_Seed(uint8 High, uint8 mini_high);

/**
 * @brief �����ӵ����������߽�
 * @param point ���ӵ���������� (point[0] = y, point[1] = x)
 * @param seed_kind ���ӵ����� (���磬��߽硢�ұ߽�)
 */
void seek_up(uint8 point[2], uint8 seed_kind);

/**
 * @brief �����ӵ����������߽�
 * @param x ��Ҫ���ҵ��е�X����
 * @return uint8 �ҵ��ĵ��y����
 */
uint8 Seek_TOP(uint8 x);

/**
 * @brief ʹ������Ӧ��ֵ������߽��ߵ㼯
 * @param block_size ����Ӧ��ֵ�����������С
 * @param clip_value ����Ӧ��ֵ����Ĳü�ֵ
 * @param pts_arry ���ڴ洢�ҵ�����߽�������
 * @return uint8 �ҵ��ĵ������
 */
uint8 findline_lefthand_adaptive(uint8 block_size, uint8 clip_value, uint8 pts_arry[][2]);

/**
 * @brief ʹ������Ӧ��ֵ�����ұ߽��ߵ㼯
 * @param block_size ����Ӧ��ֵ�����������С
 * @param clip_value ����Ӧ��ֵ����Ĳü�ֵ
 * @param pts_arry ���ڴ洢�ҵ����ұ߽�������
 * @return uint8 �ҵ��ĵ������
 */
uint8 findline_righthand_adaptive(uint8 block_size, uint8 clip_value, uint8 pts_arry[][2]);


//==================================================================================
// �߽��߸���
//==================================================================================
/**
 * @brief ������߽���
 * @param pts_in �������߽��ѡ�㼯
 * @param num ����㼯����Ч����
 * @param pts_out ������ٵ�����߽��ߵ㼯
 * @param approx_num ���Ƽ���ʱʹ�õĵ���
 * @param dist ������ƥ�������ֵ
 * @return uint8 ���ٵ�����߽����ϵĵ���
 */
uint8 track_leftline(uint8 pts_in[][2], uint8 num, uint8 pts_out[][2], int8 approx_num, uint8 dist);

/**
 * @brief �����ұ߽���
 * @param pts_in ������ұ߽��ѡ�㼯
 * @param num ����㼯����Ч����
 * @param pts_out ������ٵ����ұ߽��ߵ㼯
 * @param approx_num ���Ƽ���ʱʹ�õĵ���
 * @param dist ������ƥ�������ֵ
 * @return uint8 ���ٵ����ұ߽����ϵĵ���
 */
uint8 track_rightline(uint8 pts_in[][2], uint8 num, uint8 pts_out[][2], int8 approx_num, uint8 dist);

/**
 * @brief ������߽����ϵ��ض���
 * @param pts_in �������߽��ߵ㼯
 * @param num ����㼯����Ч����
 * @param ID Ҫ���ٵĵ���������ʶ
 * @param pts_out ������ٵ��ĵ������
 * @param approx_num ���Ƽ���ʱʹ�õĵ���
 * @param dist ������ƥ�������ֵ
 */
void track_leftpoint(uint8 pts_in[][2], uint8 num, uint8 ID, uint8 pts_out[2], int8 approx_num, uint8 dist);

/**
 * @brief �����ұ߽����ϵ��ض���
 * @param pts_in ������ұ߽��ߵ㼯
 * @param num ����㼯����Ч����
 * @param ID Ҫ���ٵĵ���������ʶ
 * @param pts_out ������ٵ��ĵ������
 * @param approx_num ���Ƽ���ʱʹ�õĵ���
 * @param dist ������ƥ�������ֵ
 */
void track_rightpoint(uint8 pts_in[][2], uint8 num, uint8 ID, uint8 pts_out[2], int8 approx_num, uint8 dist);


//==================================================================================
// �㼯�������ֵ
//==================================================================================
/**
 * @brief �Ե㼯����ģ������ (�����ֵ�˲�)
 * @param step ����㼯����Ч����
 * @param kernel ģ���˵Ĵ�С
 * @param pts_in ����㼯���� (ԭ���޸�)
 * @return uint8 ��������Ч���� (���ܲ���)
 */
uint8 blur_points(uint8 step, uint8 kernel, uint8 pts_in[][2]);

/**
 * @brief �Ե㼯�����ز�����ʹ�������ƾ���
 * @param pts_in ����㼯����
 * @param step1 ����㼯����Ч����
 * @param step2 ����㼯������������
 * @param dist �����ĵ���
 * @return uint8 �ز��������Ч����
 */
uint8 resample_points(uint8 pts_in[][2], uint8 step1, uint8 step2, uint8 dist);

/**
 * @brief ������֮�����ֱ���ϵĵ� (���Բ�ֵ)
 * @param location1 ֱ���������
 * @param location2 ֱ���յ�����
 * @param pts ���ڴ洢��ӵ������
 * @param add_location ��ʼ��ӵ����������
 * @param dist ��ӵ�֮��ľ���
 * @return uint8 ��ӵ���������µ���Ч������ (add_location + ��ӵĵ���)
 */
uint8 addline(uint8 location1[2], uint8 location2[2], uint8 pts[][2], uint8 add_location, uint8 dist);

/**
 * @brief �����㶨��Ķ��α�������������ӵ� (��ֵ)
 * @param location1 �������������
 * @param location2 ���������߿��Ƶ�
 * @param location3 �����������յ�
 * @param pts ���ڴ洢��ӵ������
 * @param add_location ��ʼ��ӵ����������
 * @param dist ��ӵ�֮��Ľ��ƾ��� (������)
 * @return uint8 ��ӵ���������µ���Ч������ (add_location + ��ӵĵ���)
 */
uint8 add_three_point_bezier(uint8 location1[2], uint8 location2[2], uint8 location3[2], uint8 pts[][2], uint8 add_location, uint8 dist);


//==================================================================================
// ���μ�����������ȡ
//==================================================================================
/**
 * @brief ����㼯��ָ���㴦�ľֲ��Ƕ�
 * @param pts_in ����ĵ㼯����
 * @param num ����㼯����Ч����
 * @param ID ����Ƕȵ�Ŀ��������
 * @param dist ����Ƕ�ʱ���ǵ��ڽ������
 * @return float ����õ��ľֲ��Ƕ� (���Ȼ�ȣ���Ҫ����ʵ��ȷ��)
 */
float local_angle_points(uint8 pts_in[][2], uint8 num, uint8 ID, uint8 dist);

/**
 * @brief ʹ��������������� (���� Menger ����)
 * @param P0 ��һ���������
 * @param P1 �ڶ���������� (���ĵ�)
 * @param P2 �������������
 * @return float ����õ�������
 */
float compute_curvature(uint8 P0[2], uint8 P1[2], uint8 P2[2]);

/**
 * @brief ��ȡ�㼯�е���� Y ���� (ͨ������ͼ���е���͵����Զ��)
 * @param pts_in ����ĵ㼯����
 * @param num ����㼯����Ч����
 * @return uint8 �㼯�е���� Y ����
 */
uint8 Get_high(uint8 pts_in[][2], uint8 num);

/**
 * @brief ��������֮���ŷ�Ͼ���
 * @param pt1 ��һ���������
 * @param pt2 �ڶ����������
 * @return float �����ľ���
 */
float Get_Distance(uint8 pt1[2], uint8 pt2[2]);

/**
 * @brief �ԽǶ�������зǼ���ֵ���� (NMS)
 * @param angle_in ����ĽǶ�����
 * @param num ���������Ԫ�ظ���
 * @param angle_out ������� NMS ����ĽǶ�����
 * @param kernel NMS �����Ĵ��ڴ�С
 */
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);


//==================================================================================
// ��������� (������Ҫ�������ϲ�ģ��)
//==================================================================================
/**
 * @brief �����������ݺ��ٶȼ���������
 * @param midline �������ݽṹָ��
 * @param speed ��ǰ�ٶ�
 * @param speed_decision �ٶ���صľ��߲���
 * @return float ����õ��Ŀ������ֵ
 */
float Get_error(MidlineData *midline, int speed, float speed_decision);


#endif // _image_processing_h_
