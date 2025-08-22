#ifndef __PERSPECTIVE_H__
#define __PERSPECTIVE_H__

#include "zf_common_headfile.h"

/**
 * @brief Flash�д洢��͸�Ӿ����ҳ����
 */
#define MATRIX_INDEX 0

/**
 * @brief �������
 */
#define INVERSE_MATRIX_SIZE 40

/**
 * @brief ���㲢��ȡ��͸�ӱ任����
 * 
 * �ú�������ݵ�ǰͼ���е������㣬������͸�ӱ任�����3x3���󣬲�������洢��ȫ�ֱ���inverse_matrix�С�
 * ͨ�����ڱ궨��ͼ��任ǰ��׼��������
 */
void Get_inverse_matrix(void);

/**
 * @brief ��Flash�ж�ȡ��͸�ӱ任����
 * 
 * @param matrix ���������3x3��double���;������ڴ�Ŷ�ȡ������͸�Ӿ���
 * 
 * �ú������Flashָ��ҳ��ȡ9�����������������ȷ�ʽ��䵽matrix[3][3]�С�
 */
void Read_Matrix(double matrix[3][3]);

/**
 * @brief ��ԭʼͼ�������͸�ӱ任�����ɸ���ͼ��
 * 
 * @param inverse_matrix ���������3x3����͸�ӱ任����
 * @param transformed_image ����������任���ͼ�����ݣ���СΪMT9V03X_H x MT9V03X_W
 * 
 * �ú��������Ŀ��ͼ���ÿ�����أ�ͨ����͸�Ӿ����������ԭͼ���еĶ�Ӧλ�ã������ɱ任��ĸ���ͼ��
 */
void Image_Transformed(double inverse_matrix[3][3], uint8 transformed_image[MT9V03X_H][MT9V03X_W]);

#endif
