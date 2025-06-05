#ifndef _IMAGE_STORAGE_H_
#define _IMAGE_STORAGE_H_

#include "zf_common_headfile.h"
#include "w25n04.h"

// ѹ�����ͼ��ߴ�
#define IPCH 60
#define IPCW 80
#define image_storage_page 4
#define STORAGE_BUFFER_SIZE  image_storage_page * W25N04_DATA_SIZE


#define left_boundary_type 0x01
#define right_boundary_type 0x02
#define Fleft_boundary_type 0x03
#define Fright_boundary_type 0x04
#define midline_type 0x05
#define cross_type 0x06
#define ring_type 0x07
#define image_type 0x08
#define error_type 0x09
#define speed_type 0x0A
#define speed_target_type 0x0B
#define mid_pwm_type 0x0C
#define mid_speed_type 0x0D
typedef enum
{
    go = 0,
    back = 1,
    go_go = 2,
    back_back = 3
}video_process_t;

// �洢ϵͳ״̬��
typedef enum {
    STORAGE_IDLE = 0,        // ����״̬
    STORAGE_WRITING = 1,     // ����д��
    STORAGE_READING = 2,     // ���ڶ�ȡ
    STORAGE_ERROR = 3        // ����״̬
} storage_state_t;

// �����붨��
typedef enum {
    STORAGE_OK = 0,          // �����ɹ�
    STORAGE_INIT_FAILED,     // ��ʼ��ʧ��
    STORAGE_WRITE_FAILED,    // д��ʧ��
    STORAGE_READ_FAILED,     // ��ȡʧ��
    STORAGE_BUSY,           // �豸æ
    STORAGE_INVALID_PARAM   // ��Ч����
} storage_error_t;

// �洢ϵͳ���ýṹ��
typedef struct {
    uint32 frame_count;    // ֡��
    uint32 current_num;   // ��ǰ��д��ַ
    storage_state_t state;   // ��ǰ״̬
    storage_error_t error;   // ������
} storage_config_t;


/**
 * @brief ��ʼ��ͼ��洢ϵͳ
 * @param config �洢ϵͳ����
 * @return storage_error_t ������
 */
storage_error_t image_storage_init(void);

/**
 * @brief ͼ��ѹ������������ƽ������
 * @param src Դͼ������ָ��
 * @param dst Ŀ��ѹ��ͼ������ָ��
 */
void image_compress(uint8 src[MT9V03X_H][MT9V03X_W]);

/**
 * @brief ͼ��ѹ���������߽����ݣ�
 * @param boundary �߽�����ָ��
 * @param type ��������
 */ 
void image_compress_boundary(BoundaryData *boundary,uint8 type);

/**
 * @brief ͼ��ѹ���������߽����ݣ�
 * @param fboundary �߽�����ָ��
 * @param type ��������
 */ 
void image_compress_fboundary(FBoundaryData *fboundary,uint8 type);

/**
 * @brief ͼ��ѹ���������м������ݣ�
 * @param midline �м�������ָ��
 * @param type ��������
 */ 
void image_compress_midline(MidlineData *midline,uint8 type);

/**
 * @brief ͼ��ѹ���������������ݣ�
 */ 
void image_compress_cross(void);    

/**
 * @brief ͼ��ѹ�������������ݣ�
 */ 
void image_compress_ring(void);

/**
 * @brief ����ѹ������
 * @param parameter ����ֵ
 * @param parameter_type ��������
 */
void parameter_compress_float(float parameter,uint8 parameter_type);

/**
 * @brief ����ѹ������
 * @param parameter ����ֵ
 * @param parameter_type ��������
 */
void parameter_compress_int(int parameter,uint8 parameter_type);

/**
 * @brief ����ѹ������
 * @param parameter ����ֵ
 * @param parameter_type ��������
 */
void parameter_compress_uint8(uint8 parameter,uint8 parameter_type);

/**
 * @brief �洢һ֡ѹ��ͼ������
 * @param config �洢ϵͳ����
 * @param image_data ԭʼͼ������ָ��
 * @return storage_error_t ������
 */
storage_error_t store_compressed_image(void);

/**
 * @brief ��ȡһ֡ѹ��ͼ�����ݲ���ԭ
 * @param config �洢ϵͳ����
 * @param image_data ͼ���������ָ��
 * @return storage_error_t ������
 */
storage_error_t read_compressed_image(video_process_t video_process);

/**
 * @brief ͼ�����ݷ���
 */
void image_data_analysis(void);

/**
 * @brief ��ȡ�洢ϵͳ״̬
 * @param config �洢ϵͳ����
 * @return storage_state_t ��ǰ״̬
 */
storage_state_t get_storage_state(void);

/**
 * @brief ��ȡ���һ�δ���
 * @param config �洢ϵͳ����
 * @return storage_error_t ������
 */
storage_error_t get_last_error(void);

/**
 * @brief ��ȡ��ǰ֡��
 * @return uint32_t ��ǰ֡��
 */
uint32 get_frame_count(void);

/**
 * @brief ��������оƬ
 * @return storage_error_t ������
 */
storage_error_t erase_storage_block(void);

/**
 * @brief ��ȡflash�е�֡��
 */
void image_read_frame_count(void);

#endif /* _IMAGE_STORAGE_H_ */
