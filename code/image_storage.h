#ifndef _IMAGE_STORAGE_H_
#define _IMAGE_STORAGE_H_

#include "zf_common_headfile.h"
#include "w25n04.h"

// 压缩后的图像尺寸
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

// 存储系统状态码
typedef enum {
    STORAGE_IDLE = 0,        // 空闲状态
    STORAGE_WRITING = 1,     // 正在写入
    STORAGE_READING = 2,     // 正在读取
    STORAGE_ERROR = 3        // 错误状态
} storage_state_t;

// 错误码定义
typedef enum {
    STORAGE_OK = 0,          // 操作成功
    STORAGE_INIT_FAILED,     // 初始化失败
    STORAGE_WRITE_FAILED,    // 写入失败
    STORAGE_READ_FAILED,     // 读取失败
    STORAGE_BUSY,           // 设备忙
    STORAGE_INVALID_PARAM   // 无效参数
} storage_error_t;

// 存储系统配置结构体
typedef struct {
    uint32 frame_count;    // 帧数
    uint32 current_num;   // 当前读写地址
    storage_state_t state;   // 当前状态
    storage_error_t error;   // 错误码
} storage_config_t;


/**
 * @brief 初始化图像存储系统
 * @param config 存储系统配置
 * @return storage_error_t 错误码
 */
storage_error_t image_storage_init(void);

/**
 * @brief 图像压缩函数（区域平均法）
 * @param src 源图像数据指针
 * @param dst 目标压缩图像数据指针
 */
void image_compress(uint8 src[MT9V03X_H][MT9V03X_W]);

/**
 * @brief 图像压缩函数（边界数据）
 * @param boundary 边界数据指针
 * @param type 数据类型
 */ 
void image_compress_boundary(BoundaryData *boundary,uint8 type);

/**
 * @brief 图像压缩函数（边界数据）
 * @param fboundary 边界数据指针
 * @param type 数据类型
 */ 
void image_compress_fboundary(FBoundaryData *fboundary,uint8 type);

/**
 * @brief 图像压缩函数（中间线数据）
 * @param midline 中间线数据指针
 * @param type 数据类型
 */ 
void image_compress_midline(MidlineData *midline,uint8 type);

/**
 * @brief 图像压缩函数（交叉数据）
 */ 
void image_compress_cross(void);    

/**
 * @brief 图像压缩函数（环数据）
 */ 
void image_compress_ring(void);

/**
 * @brief 参数压缩函数
 * @param parameter 参数值
 * @param parameter_type 参数类型
 */
void parameter_compress_float(float parameter,uint8 parameter_type);

/**
 * @brief 参数压缩函数
 * @param parameter 参数值
 * @param parameter_type 参数类型
 */
void parameter_compress_int(int parameter,uint8 parameter_type);

/**
 * @brief 参数压缩函数
 * @param parameter 参数值
 * @param parameter_type 参数类型
 */
void parameter_compress_uint8(uint8 parameter,uint8 parameter_type);

/**
 * @brief 存储一帧压缩图像数据
 * @param config 存储系统配置
 * @param image_data 原始图像数据指针
 * @return storage_error_t 错误码
 */
storage_error_t store_compressed_image(void);

/**
 * @brief 读取一帧压缩图像数据并还原
 * @param config 存储系统配置
 * @param image_data 图像数据输出指针
 * @return storage_error_t 错误码
 */
storage_error_t read_compressed_image(video_process_t video_process);

/**
 * @brief 图像数据分析
 */
void image_data_analysis(void);

/**
 * @brief 获取存储系统状态
 * @param config 存储系统配置
 * @return storage_state_t 当前状态
 */
storage_state_t get_storage_state(void);

/**
 * @brief 获取最后一次错误
 * @param config 存储系统配置
 * @return storage_error_t 错误码
 */
storage_error_t get_last_error(void);

/**
 * @brief 获取当前帧数
 * @return uint32_t 当前帧数
 */
uint32 get_frame_count(void);

/**
 * @brief 擦除整个芯片
 * @return storage_error_t 错误码
 */
storage_error_t erase_storage_block(void);

/**
 * @brief 读取flash中的帧数
 */
void image_read_frame_count(void);

#endif /* _IMAGE_STORAGE_H_ */
