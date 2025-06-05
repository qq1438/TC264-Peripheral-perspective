#include "perspective.h"
#include <math.h>

uint8 FL_corner_index, FR_corner_index;
uint8 up_distance = 50;
double inverse_matrix[3][3];

#define EPSILON 1e-10

void solveLinearSystem(double A[64], double b[8], double x[8]) {
    int n = 8;

    for (int k = 0; k < n; k++) {
        double max_val = fabs(A[k * n + k]);
        int max_row = k;
        for (int i = k + 1; i < n; i++) {
            if (fabs(A[i * n + k]) > max_val) {
                max_val = fabs(A[i * n + k]);
                max_row = i;
            }
        }

        if (max_row != k) {
            for (int j = k; j < n; j++) {
                double temp = A[k * n + j];
                A[k * n + j] = A[max_row * n + j];
                A[max_row * n + j] = temp;
            }
            double temp = b[k];
            b[k] = b[max_row];
            b[max_row] = temp;
        }

        for (int i = k + 1; i < n; i++) {
            double factor = A[i * n + k] / A[k * n + k];
            for (int j = k; j < n; j++) {
                A[i * n + j] -= factor * A[k * n + j];
            }
            b[i] -= factor * b[k];
        }
    }

    for (int i = n - 1; i >= 0; i--) {
        double sum = 0;
        for (int j = i + 1; j < n; j++) {
            sum += A[i * n + j] * x[j];
        }
        x[i] = (b[i] - sum) / A[i * n + i];
    }
}

void NormalizePoints(const uint8_t points[][2], double normalized[8], double T[3][3]) {
    double meanX = 0, meanY = 0;
    for (int i = 0; i < 4; ++i) {
        meanX += points[i][0];
        meanY += points[i][1];
    }
    meanX /= 4;
    meanY /= 4;

    double sumDist = 0;
    for (int i = 0; i < 4; ++i) {
        double dx = points[i][0] - meanX;
        double dy = points[i][1] - meanY;
        sumDist += sqrt(dx * dx + dy * dy);
    }
    double avgDist = sumDist / 4;
    double scale = (avgDist > EPSILON) ? (sqrt(2.0) / avgDist) : 1.0;

    T[0][0] = scale;  T[0][1] = 0;      T[0][2] = -scale * meanX;
    T[1][0] = 0;      T[1][1] = scale;  T[1][2] = -scale * meanY;
    T[2][0] = 0;      T[2][1] = 0;      T[2][2] = 1;

    for (int i = 0; i < 4; ++i) {
        normalized[2 * i]     = (points[i][0] - meanX) * scale;
        normalized[2 * i + 1] = (points[i][1] - meanY) * scale;
    }
}

void DenormalizeMatrix(double H[3][3], const double T1[3][3], const double T2_inv[3][3]) {
    double temp[3][3];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            temp[i][j] = 0;
            for (int k = 0; k < 3; ++k) {
                temp[i][j] += T2_inv[i][k] * H[k][j];
            }
        }
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            H[i][j] = 0;
            for (int k = 0; k < 3; ++k) {
                H[i][j] += temp[i][k] * T1[k][j];
            }
        }
    }
}

double determinant3x3(const double matrix[3][3]) {
    return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
           matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
           matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
}

void inverseMatrix3x3(const double matrix[3][3], double inverse[3][3]) {
    double det = determinant3x3(matrix);
    if (fabs(det) < EPSILON) {
        memset(inverse, 0, sizeof(double) * 9);
        return;
    }

    double invDet = 1.0 / det;

    inverse[0][0] = (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) * invDet;
    inverse[0][1] = (matrix[0][2] * matrix[2][1] - matrix[0][1] * matrix[2][2]) * invDet;
    inverse[0][2] = (matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1]) * invDet;
    inverse[1][0] = (matrix[1][2] * matrix[2][0] - matrix[1][0] * matrix[2][2]) * invDet;
    inverse[1][1] = (matrix[0][0] * matrix[2][2] - matrix[0][2] * matrix[2][0]) * invDet;
    inverse[1][2] = (matrix[0][2] * matrix[1][0] - matrix[0][0] * matrix[1][2]) * invDet;
    inverse[2][0] = (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]) * invDet;
    inverse[2][1] = (matrix[0][1] * matrix[2][0] - matrix[0][0] * matrix[2][1]) * invDet;
    inverse[2][2] = (matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]) * invDet;
}

bool CalculateInversePerspectiveMatrix(const uint8_t perspective_points[][2], const uint8_t topdown_points[][2], double matrix[3][3]) {
    double norm_perspective[8], norm_topdown[8];
    double T_perspective[3][3], T_topdown[3][3];
    double T_topdown_inv[3][3];

    NormalizePoints(perspective_points, norm_perspective, T_perspective);
    NormalizePoints(topdown_points, norm_topdown, T_topdown);

    inverseMatrix3x3(T_topdown, T_topdown_inv);

    double A[64] = { 0 };
    double b[8] = { 0 };

    for (int i = 0; i < 4; ++i) {
        double x = norm_perspective[2 * i];
        double y = norm_perspective[2 * i + 1];
        double u = norm_topdown[2 * i];
        double v = norm_topdown[2 * i + 1];

        int row = 2 * i;
        A[row * 8 + 0] = x;
        A[row * 8 + 1] = y;
        A[row * 8 + 2] = 1;
        A[row * 8 + 6] = -u * x;
        A[row * 8 + 7] = -u * y;
        b[row] = u;

        row = 2 * i + 1;
        A[row * 8 + 3] = x;
        A[row * 8 + 4] = y;
        A[row * 8 + 5] = 1;
        A[row * 8 + 6] = -v * x;
        A[row * 8 + 7] = -v * y;
        b[row] = v;
    }

    double h[8];
    solveLinearSystem(A, b, h);

    double H_norm[3][3] = {
        {h[0], h[1], h[2]},
        {h[3], h[4], h[5]},
        {h[6], h[7], 1.0}
    };

    DenormalizeMatrix(H_norm, T_perspective, T_topdown_inv);
    
    double scale = -1.0 / H_norm[2][2];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            H_norm[i][j] *= scale;
        }
    }

    memcpy(matrix, H_norm, sizeof(double) * 9);
    return true;
}

void Image_Transformed(double inverse_matrix[3][3],uint8 transformed_image[MT9V03X_H][MT9V03X_W]){
    for (int v = 0; v < MT9V03X_H; v++) {
        for (int u = 0; u < MT9V03X_W; u++) {
            float x = u;
            float y = v;

            float denominator = (inverse_matrix[0][0] * inverse_matrix[1][1] - inverse_matrix[0][0] * inverse_matrix[2][1] * y -
                                  inverse_matrix[0][1] * inverse_matrix[1][0] + inverse_matrix[0][1] * inverse_matrix[2][0] * y +
                                      inverse_matrix[1][0] * inverse_matrix[2][1] * x - inverse_matrix[1][1] * inverse_matrix[2][0] * x);

            float transformed_x = (inverse_matrix[0][1] * inverse_matrix[1][2] - inverse_matrix[0][1] * inverse_matrix[2][2] * y -
                                inverse_matrix[0][2] * inverse_matrix[1][1] + inverse_matrix[0][2] * inverse_matrix[2][1] * y +
                                inverse_matrix[1][1] * inverse_matrix[2][2] * x - inverse_matrix[1][2] * inverse_matrix[2][1] * x) / denominator;

            float transformed_y = (-inverse_matrix[0][0] * inverse_matrix[1][2] + inverse_matrix[0][0] * inverse_matrix[2][2] * y +
                            inverse_matrix[0][2] * inverse_matrix[1][0] - inverse_matrix[0][2] * inverse_matrix[2][0] * y -
                            inverse_matrix[1][0] * inverse_matrix[2][2] * x + inverse_matrix[1][2] * inverse_matrix[2][0] * x) / denominator;
            int x1 = (int)(transformed_x + 0.5f);
            int y1 = (int)(transformed_y + 0.5f);

            if(x1 >= 0 && x1 < MT9V03X_W && y1 >= 0 && y1 < MT9V03X_H) {
                transformed_image[v][u] = mt9v03x_image[y1][x1];
            } else {
                transformed_image[v][u] = 255;
            }
        }
    }
}

void Get_Four_Points(void)
{
    Image_Copy();
    Seek_Pts_Seed(Start_high,Mini_high);
    LeftPts_Deal_Original();
    RightPts_Deal_Original();
    left_boundary.now_step_original = left_boundary.original_step;
    right_boundary.now_step_original = right_boundary.original_step;
    find_corner(&left_boundary, &right_boundary);
    left_boundary.Lp_id = left_boundary.map[0].original_index;
    right_boundary.Lp_id = right_boundary.map[0].original_index;
    uint8 left_statr_point[2] = { left_boundary.original_pts[left_boundary.Lp_id][0] - 5, left_boundary.original_pts[left_boundary.Lp_id][1] - 5 };
    uint8 right_statr_point[2] = { right_boundary.original_pts[right_boundary.Lp_id][0] + 5, right_boundary.original_pts[right_boundary.Lp_id][1] - 5 };
    seek_up(left_statr_point, left_pts);
    seek_up(right_statr_point, Right_pts);
    FLeftPts_Deal_Original();
    FRightPts_Deal_Original();
    Fleft_boundary.original_step = Arry_Filter_2(Fleft_boundary.original_pts, Fleft_boundary.original_step);
    Fright_boundary.original_step = Arry_Filter_2(Fright_boundary.original_pts, Fright_boundary.original_step);
    FL_corner_index = seek_corner(Fleft_boundary.original_pts, Fleft_boundary.original_step, 4);
    FR_corner_index = seek_corner(Fright_boundary.original_pts, Fright_boundary.original_step, 5);
    point_show1(Fleft_boundary.original_pts[FL_corner_index]);
    point_show1(Fright_boundary.original_pts[FR_corner_index]);
    point_show1(left_boundary.original_pts[left_boundary.Lp_id]);
    point_show1(right_boundary.original_pts[right_boundary.Lp_id]);
    ips200_show_gray_image(0, 0, Image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
}

void Perspective_Transform(void)
{
    uint8_t perspective_points[4][2] = {
        {Fleft_boundary.original_pts[FL_corner_index][0], Fleft_boundary.original_pts[FL_corner_index][1]},
        {Fright_boundary.original_pts[FR_corner_index][0], Fright_boundary.original_pts[FR_corner_index][1]},
        {left_boundary.original_pts[left_boundary.Lp_id][0], left_boundary.original_pts[left_boundary.Lp_id][1]},
        {right_boundary.original_pts[right_boundary.Lp_id][0], right_boundary.original_pts[right_boundary.Lp_id][1]}
    };
    uint8_t topdown_points[4][2] = {
        {74, up_distance - 20},
        {114, up_distance - 20},
        {74, up_distance + 20},
        {114, up_distance + 20}
    };

    if (CalculateInversePerspectiveMatrix(perspective_points, topdown_points, inverse_matrix)) {
        memset(Image,0,sizeof(Image));
        Image_Transformed(inverse_matrix,Image);
        ips200_show_gray_image(0, 130, Image[0], 
                              MT9V03X_W, MT9V03X_H,
                              MT9V03X_W, MT9V03X_H, 0);
    }
}

void Write_Matrix(void)
{
    flash_erase_page(0,MATRIX_INDEX);
    for(uint8 i = 0;i < 9;i++)
    {
        flash_union_buffer[i].float_type = inverse_matrix[i / 3][i % 3];
    }
    flash_write_page_from_buffer(0,MATRIX_INDEX);
}

void Read_Matrix(double matrix[3][3])
{
    flash_read_page_to_buffer(0,MATRIX_INDEX);
    for(uint8 i = 0;i < 9;i++)
    {
        matrix[i / 3][i % 3] = flash_union_buffer[i].float_type;
    }
}
void Get_inverse_matrix(void)
{
    if(key_flag == Button_1)
    {
        key_flag = 0;
        mt9v03x_finish_flag = 0;
        while(mt9v03x_finish_flag == 0);
        Get_Four_Points();
        Perspective_Transform();
    }
    if(key_flag == Button_2)
    {
        key_flag = 0;
        uint8 time = 0;
        uint8 flag = 0;
        for(int v = MT9V03X_H - 1; v > 0; v--)
        {
            for(int u = 0; u < MT9V03X_W; u++)
            {
                if(Image[v][u] != 255)
                {
                    time++;
                }
            }
            if(time != 0)
            {
                for(int u = 0; u < MT9V03X_W; u++)
                {
                    Image[v][u] = 255;
                }
                time = 0;
            }else{
                break;
            }
        }
        for (int v = MT9V03X_H - 1; v > 0; v--) {
            for (int u = 0; u < MT9V03X_W; u++) {
                if(Image[v][u] != 255)
                {
                    time++;
                }
            }
            if(time > INVERSE_MATRIX_SIZE)
            {
                flag = v + 1;
                break;
            }
        }
        up_distance += (MT9V03X_H - flag);
        Perspective_Transform();
        Write_Matrix();
        ImagePerspective_Init(inverse_matrix);
        up_distance = 50;
    }
    
}
