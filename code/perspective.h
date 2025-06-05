#ifndef __PERSPECTIVE_H__
#define __PERSPECTIVE_H__

#include "zf_common_headfile.h"

#define MATRIX_INDEX 0

#define INVERSE_MATRIX_SIZE 40


void Get_inverse_matrix(void);
void Read_Matrix(double matrix[3][3]);
void Image_Transformed(double inverse_matrix[3][3],uint8 transformed_image[MT9V03X_H][MT9V03X_W]);

#endif
