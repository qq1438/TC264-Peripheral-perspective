#include "PTS_Deal.h"

bool zebra = false;

static bool judge_time(int position, int target);

/**
 * @brief �����߼�⺯���������ٶȺ���ֵ�ж��Ƿ��⵽������
 * @param target ��Ե�仯������ֵ�����仯�������ڸ�ֵʱ����Ϊ��⵽������
 */
void zebra_judge(int target)
{
    uint8 point[2] = {94,Start_high};
    TransformSinglePoint(point,point);
    point[1] = point[1] - 10;
    ImagePerspective(point);
    int base_high = point[1];
    int offsets[3] = {0, -5, 5};
    for(int i = 0; i < 3; i++) {
        if(judge_time(base_high + offsets[i], target)) {
            zebra = true;
            break;
        }
    }
}

/**
 * @brief �ж�ָ��λ���Ƿ�����������ж�����
 * @param position ͼ��ֱλ�ã���Ӧ�߽���������
 * @param target ��Ե�仯������ֵ
 * @return ��������������� true�����򷵻� false
 */
bool judge_time(int position, int target)
{
    uint8 left_x = 0;
    uint8 right_x = 0;
    uint8 count = 0;
    bool now_state = false, last_state = false;
    
    // �ҵ���λ�ö�Ӧ����߽�� - ����ӽ���С�ڵ���position�ĵ�
    for(uint8 i = 0; i < left_boundary.original_step; i++) {
        if(left_boundary.original_pts[i][1] <= position) {
            left_x = left_boundary.original_pts[i][0];
            break;
        }
    }
    
    // �ҵ���λ�ö�Ӧ���ұ߽�� - ����ӽ���С�ڵ���position�ĵ�
    for(uint8 i = 0; i < right_boundary.original_step; i++) {
        if(right_boundary.original_pts[i][1] <= position) {
            right_x = right_boundary.original_pts[i][0];
            break;
        }
    }
    
    // �߽簲ȫ���
    if(left_x >= right_x || left_x >= MT9V03X_W || right_x >= MT9V03X_W || position < 0 || position >= MT9V03X_H || left_x == 0 || right_x == 0) {
        return false;
    }
    
    // ���ó�ʼ״̬Ϊ��һ�����ص�״̬
    if(Image[position][left_x] > low_threshold) {
        last_state = true;
    } else {
        last_state = false;
    }
    
    // �ӵڶ������ؿ�ʼ�����Ե�仯����
    for(uint8 i = left_x + 1; i <= right_x; i++) {
        if(Image[position][i] > low_threshold) {
            now_state = true;
        } else {
            now_state = false;
        }
        
        // ��⵽״̬�仯ʱ������1
        if(now_state != last_state) {
            count++;
        }
        last_state = now_state;
    }
    return (count > target);
}
