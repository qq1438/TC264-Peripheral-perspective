#include "image_storage.h"
#include "zf_common_headfile.h"
#include <string.h>

// 图像数据存储相关定义
#define IMAGE_SIZE             (MT9V03X_W * MT9V03X_H)  // 一帧图像大小
#define COMPRESSED_IMAGE_SIZE  (IPCH * IPCW)  // 压缩后图像大小
#define COMPRESSED_PAGES_PER_FRAME ((COMPRESSED_IMAGE_SIZE + W25N04_DATA_SIZE - 1) / W25N04_DATA_SIZE)  // 压缩后每帧所需页数

// 定点数计算相关定义
#define FIXED_POINT_BITS 8
#define FIXED_POINT_SCALE (1 << FIXED_POINT_BITS)

// 临时存储缓冲区
static uint8 storage_buffer[STORAGE_BUFFER_SIZE];
storage_config_t storage_config;

// 图像压缩函数
void image_compress(uint8 src[MT9V03X_H][MT9V03X_W])
{
    memset(storage_buffer, 0xff, STORAGE_BUFFER_SIZE);
    storage_config.current_num = 0;
    storage_buffer[storage_config.current_num] = image_type;
    storage_config.current_num++;
    const int32_t scale_h = ((int32_t)(MT9V03X_H << FIXED_POINT_BITS)) / IPCH;
    const int32_t scale_w = ((int32_t)(MT9V03X_W << FIXED_POINT_BITS)) / IPCW;
    
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < IPCH; i++)
    {
        for (int j = 0; j < IPCW; j++)
        {
            int32_t y = (i * scale_h + (scale_h >> 1)) >> FIXED_POINT_BITS;
            int32_t x = (j * scale_w + (scale_w >> 1)) >> FIXED_POINT_BITS;
            
            if (y >= MT9V03X_H) y = MT9V03X_H - 1;
            if (x >= MT9V03X_W) x = MT9V03X_W - 1;
            
            storage_buffer[storage_config.current_num] = src[y][x];
            storage_config.current_num++;
        }
    }
}

void image_compress_boundary(BoundaryData *boundary,uint8 type)
{
    storage_buffer[storage_config.current_num] = type;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = boundary->original_step;
    storage_config.current_num++;
    for(uint8 i = 0; i < boundary->original_step; i++)
    {
        storage_buffer[storage_config.current_num] = boundary->original_pts[i][0];
        storage_config.current_num++;
        storage_buffer[storage_config.current_num] = boundary->original_pts[i][1];
        storage_config.current_num++;
    }
    storage_buffer[storage_config.current_num] = boundary->now_step;
    storage_config.current_num++;
    for(uint8 i = 0; i < boundary->now_step; i++)
    {
        storage_buffer[storage_config.current_num] = boundary->now_pts[i][0];
        storage_config.current_num++;
        storage_buffer[storage_config.current_num] = boundary->now_pts[i][1];
        storage_config.current_num++;
    }
    storage_buffer[storage_config.current_num] = boundary->Lp_id;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = boundary->is_straight;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = boundary->Lp_state;
    storage_config.current_num++;
}

void image_compress_midline(MidlineData *midline,uint8 type)
{
    storage_buffer[storage_config.current_num] = type;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = midline->step;
    storage_config.current_num++;
    for(uint8 i = 0; i < midline->step; i++)
    {
        storage_buffer[storage_config.current_num] = midline->pts[i][0];
        storage_config.current_num++;
        storage_buffer[storage_config.current_num] = midline->pts[i][1];
        storage_config.current_num++;
    }
}   

void image_compress_fboundary(FBoundaryData *fboundary,uint8 type)
{
    storage_buffer[storage_config.current_num] = type;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = fboundary->original_step;
    storage_config.current_num++;
    for(uint8 i = 0; i < fboundary->original_step; i++)
    {
        storage_buffer[storage_config.current_num] = fboundary->original_pts[i][0];
        storage_config.current_num++;
        storage_buffer[storage_config.current_num] = fboundary->original_pts[i][1];
        storage_config.current_num++;
    }   
}

void image_compress_cross(void)
{
    storage_buffer[storage_config.current_num] = cross_type;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = cross_ctrl.kind;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = cross_ctrl.state;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = cross_ctrl.flag;
    storage_config.current_num++;
}

void image_compress_ring(void)
{
    storage_buffer[storage_config.current_num] = ring_type;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = ring_ctrl.kind;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = ring_ctrl.state;
    storage_config.current_num++;
}

void parameter_compress_float(float parameter,uint8 parameter_type)
{
    storage_buffer[storage_config.current_num] = parameter_type;
    storage_config.current_num++;
    uint32_t *p = (uint32_t *)&parameter;
    storage_buffer[storage_config.current_num] = (*p >> 24) & 0xFF;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = (*p >> 16) & 0xFF; 
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = (*p >> 8) & 0xFF;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = *p & 0xFF;
    storage_config.current_num++;
}

void parameter_compress_uint8(uint8 parameter,uint8 parameter_type)
{
    storage_buffer[storage_config.current_num] = parameter_type;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = parameter;
    storage_config.current_num++;
}

void parameter_compress_int(int parameter, uint8 parameter_type)
{
    storage_buffer[storage_config.current_num] = parameter_type;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = (parameter >> 24) & 0xFF;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = (parameter >> 16) & 0xFF;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = (parameter >> 8) & 0xFF;
    storage_config.current_num++;
    storage_buffer[storage_config.current_num] = parameter & 0xFF;
    storage_config.current_num++;
}


// 图像还原函数
void image_decompress(uint8 *src, uint8 dst[MT9V03X_H][MT9V03X_W])
{
    const int32_t scale_h = ((int32_t)(IPCH << FIXED_POINT_BITS)) / MT9V03X_H;
    const int32_t scale_w = ((int32_t)(IPCW << FIXED_POINT_BITS)) / MT9V03X_W;

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < MT9V03X_H; i++)
    {
        for (int j = 0; j < MT9V03X_W; j++)
        {
            int32_t y = (i * scale_h);
            int32_t x = (j * scale_w);

            int y_int = y >> FIXED_POINT_BITS;
            int x_int = x >> FIXED_POINT_BITS;

            if (y_int >= IPCH - 1) y_int = IPCH - 2;
            if (x_int >= IPCW - 1) x_int = IPCW - 2;

            int32_t wy = y & (FIXED_POINT_SCALE - 1);
            int32_t wx = x & (FIXED_POINT_SCALE - 1);

            uint8 p00 = src[y_int * IPCW + x_int];
            uint8 p01 = src[y_int * IPCW + x_int + 1];
            uint8 p10 = src[(y_int + 1) * IPCW + x_int];
            uint8 p11 = src[(y_int + 1) * IPCW + x_int + 1];

            int32_t col0 = p00 * (FIXED_POINT_SCALE - wx) + p01 * wx;
            int32_t col1 = p10 * (FIXED_POINT_SCALE - wx) + p11 * wx;
            
            int32_t value = col0 * (FIXED_POINT_SCALE - wy) + col1 * wy;
            
            dst[i][j] = ((value + (FIXED_POINT_SCALE * FIXED_POINT_SCALE / 2)) 
                         >> (FIXED_POINT_BITS * 2));
        }
    }
}

uint32 image_decompress_boundary(BoundaryData *boundary,uint32 address)
{
    boundary->original_step = storage_buffer[address];
    address++;
    for(uint8 i = 0; i < boundary->original_step; i++)
    {
        boundary->original_pts[i][0] = storage_buffer[address];
        address++;
        boundary->original_pts[i][1] = storage_buffer[address];
        address++;
    }
    boundary->now_step = storage_buffer[address];
    address++;
    for(uint8 i = 0; i < boundary->now_step; i++)
    {
        boundary->now_pts[i][0] = storage_buffer[address];
        address++;
        boundary->now_pts[i][1] = storage_buffer[address];
        address++;
    }
    boundary->Lp_id = storage_buffer[address];
    address++;
    boundary->is_straight = storage_buffer[address];
    address++;
    boundary->Lp_state = storage_buffer[address];
    address++;
    return address; 
}

uint32 image_decompress_midline(MidlineData *midline,uint32 address)
{
    midline->step = storage_buffer[address];
    address++;
    for(uint8 i = 0; i < midline->step; i++)
    {
        midline->pts[i][0] = storage_buffer[address];
        address++;
        midline->pts[i][1] = storage_buffer[address];
        address++;
    }
    return address;
}

uint32 image_decompress_fboundary(FBoundaryData *fboundary,uint32 address)
{
    fboundary->original_step = storage_buffer[address];
    address++;
    for(uint8 i = 0; i < fboundary->original_step; i++)
    {
        fboundary->original_pts[i][0] = storage_buffer[address];
        address++;
        fboundary->original_pts[i][1] = storage_buffer[address];
        address++;
    }
    return address;
}

uint32 image_decompress_cross(CrossState *cross,uint32 address)
{
    cross->kind = storage_buffer[address];
    address++;
    cross->state = storage_buffer[address];
    address++;
    cross->flag = storage_buffer[address];
    address++;
    return address;
}

uint32 image_decompress_ring(RingState *ring,uint32 address)
{
    ring->kind = storage_buffer[address];
    address++;
    ring->state = storage_buffer[address];  
    address++;
    return address;
}

uint32 image_decompress_float(float *error,uint32 address)
{
    uint32_t *p = (uint32_t *)error;
    *p = (storage_buffer[address] << 24) | (storage_buffer[address + 1] << 16) | (storage_buffer[address + 2] << 8) | storage_buffer[address + 3];
    address += 4;
    return address;
}

uint32 image_decompress_uint8(uint8 *parameter,uint32 address)
{
    *parameter = storage_buffer[address];
    address++;
    return address;
}

uint32 image_decompress_int(int *parameter, uint32 address)
{
    *parameter = (storage_buffer[address] << 24) | (storage_buffer[address + 1] << 16) | 
                 (storage_buffer[address + 2] << 8) | storage_buffer[address + 3];
    address += 4;
    return address;
}


// 初始化存储系统
storage_error_t image_storage_init(void)
{
    // 复位设备
    if (!w25n04_reset(1))
    {
        storage_config.error = STORAGE_INIT_FAILED;
        storage_config.state = STORAGE_ERROR;
        return STORAGE_INIT_FAILED;
    }
    
    // 解除写保护
    if (!w25n04_disable_write_protection())
    {
        storage_config.error = STORAGE_INIT_FAILED;
        storage_config.state = STORAGE_ERROR;
        return STORAGE_INIT_FAILED;
    }
    
    storage_config.state = STORAGE_IDLE;
    storage_config.error = STORAGE_OK;
    storage_config.current_num = 0;
    storage_config.frame_count = 0;
    
    return STORAGE_OK;
}

// 存储压缩图像
storage_error_t store_compressed_image(void)
{
    if (storage_config.state == STORAGE_WRITING || storage_config.state == STORAGE_READING) return STORAGE_BUSY;
    
    // 检查是否有数据需要写入
    if (storage_config.current_num == 0) {
        storage_config.error = STORAGE_INVALID_PARAM;
        return STORAGE_INVALID_PARAM;  // 不允许写入空数据
    }
    
    storage_config.state = STORAGE_WRITING;
    
    // 写入数据
    for (uint16 i = 0; i < storage_config.current_num; i += W25N04_DATA_SIZE)
    {
        if (!w25n04_write_enable())
        {
            storage_config.error = STORAGE_WRITE_FAILED;
            storage_config.state = STORAGE_ERROR;
            ips200_show_string(80, 180, "WRITE EN FAIL");
            return STORAGE_WRITE_FAILED;
        }
        
        if (!w25n04_program_data_load(0, storage_buffer + i, W25N04_DATA_SIZE))
        {
            storage_config.error = STORAGE_WRITE_FAILED;
            storage_config.state = STORAGE_ERROR;
            ips200_show_string(80, 180, "DATA LOAD FAIL");
            return STORAGE_WRITE_FAILED;
        }
        
        uint8 exec_result = w25n04_program_execute((storage_config.frame_count * STORAGE_BUFFER_SIZE + i) / W25N04_DATA_SIZE);
        if (exec_result != 1)
        {
            storage_config.error = STORAGE_WRITE_FAILED;
            storage_config.state = STORAGE_ERROR;
            if (exec_result == 2)
                ips200_show_string(80, 180, "PROG EXEC FAIL");
            else
                ips200_show_string(80, 180, "PAGE PROG ERR");
            return STORAGE_WRITE_FAILED;
        }
        
        if (!w25n04_wait_busy(20))
        {
            storage_config.error = STORAGE_WRITE_FAILED;
            storage_config.state = STORAGE_ERROR;
            ips200_show_string(80, 180, "TIMEOUT");
            return STORAGE_WRITE_FAILED;
        }
    }
    
    storage_config.frame_count++;
    storage_config.state = STORAGE_IDLE;
    return STORAGE_OK;
}

// 读取压缩图像
uint32 video_frame = 0;
storage_error_t read_compressed_image(video_process_t video_process)
{
    if (storage_config.state == STORAGE_WRITING || storage_config.state == STORAGE_READING) return STORAGE_BUSY;
    if (storage_config.frame_count == 0) return STORAGE_OK;  // 没有存储的帧
    
    storage_config.state = STORAGE_READING;
    
        // 读取压缩数据
    for (uint16 i = 0; i < STORAGE_BUFFER_SIZE; i += W25N04_DATA_SIZE)
    {
        if (!w25n04_read_page((video_frame * STORAGE_BUFFER_SIZE + i) / W25N04_DATA_SIZE, 0, storage_buffer + i, W25N04_DATA_SIZE))
        {
            storage_config.error = STORAGE_READ_FAILED;
            storage_config.state = STORAGE_ERROR;
            return STORAGE_READ_FAILED;
        }
    }
    switch(video_process) {
        case go:
            video_frame = video_frame + 1;
            break;
        case go_go:
            video_frame = video_frame + 20;
            break;
        case back_back:
            video_frame = video_frame > 20 ? video_frame - 20 : 0;
            break;
        default:
            video_frame = video_frame > 0 ? video_frame - 1 : 0;
    }
    video_frame = video_frame > storage_config.frame_count - 1 ? storage_config.frame_count - 1 : video_frame;
    ips200_show_uint(188,280,video_frame,6);
    ips200_show_uint(188,300,storage_config.frame_count,6);
    storage_config.state = STORAGE_IDLE;
    return STORAGE_OK;
}

void image_data_analysis(void)
{
    uint32 address = 0;
    while(address < STORAGE_BUFFER_SIZE)
    {
        switch(storage_buffer[address])
        {
            case image_type:
                address += 1;
                image_decompress(storage_buffer + address, Image);
                address += COMPRESSED_IMAGE_SIZE;
                break;

            case left_boundary_type:
                address += 1;
                address = image_decompress_boundary(&left_boundary, address);
                break;

            case right_boundary_type:
                address += 1;
                address = image_decompress_boundary(&right_boundary, address);
                break;

            case Fleft_boundary_type:
                address += 1;
                address = image_decompress_fboundary(&Fleft_boundary, address);
                break;

            case Fright_boundary_type:
                address += 1;
                address = image_decompress_fboundary(&Fright_boundary, address);
                break;

            case midline_type:
                address += 1;
                address = image_decompress_midline(&midline, address);
                break;  

            case cross_type:
                address += 1;
                address = image_decompress_cross(&cross_ctrl, address);
                break;

            case ring_type:
                address += 1;
                address = image_decompress_ring(&ring_ctrl, address);
                break;

            case error_type:
                address += 1;
                address = image_decompress_float(&MID_Error, address);
                break;

            case speed_type:
                address += 1;
                address = image_decompress_uint8(&high, address);
                break;
                
            default:
                address += 1;
                break;
        }
    }
}

// 获取存储状态
storage_state_t get_storage_state(void)
{
    return storage_config.state;
}

// 获取最后一次错误
storage_error_t get_last_error(void)
{
    return storage_config.error;
}

// 获取当前帧数
uint32 get_frame_count(void)
{
    return storage_config.frame_count;
}

// 擦除存储块
storage_error_t erase_storage_block(void)
{
    if (storage_config.state != STORAGE_IDLE) return STORAGE_BUSY;
    
    // 擦除整个芯片
    for (uint16 block = 0; block < W25N04_BLOCKS_PER_DIE; block++)
    {
        if (w25n04_block_erase(block) != 1)
        {
            storage_config.error = STORAGE_WRITE_FAILED;
            storage_config.state = STORAGE_ERROR;
            return STORAGE_WRITE_FAILED;
        }
        
        // 等待擦除完成
        if(w25n04_wait_busy(100) == 0)
        {
            storage_config.error = STORAGE_WRITE_FAILED;
            storage_config.state = STORAGE_ERROR;
            return STORAGE_WRITE_FAILED;
        }
    }
    return STORAGE_OK;
}

void image_read_frame_count(void)
{
    uint32 left = 0;
    uint32 right = W25N04_TOTAL_PAGES / image_storage_page;  // 总帧数
    uint32 last_valid_frame = 0;
    uint8 temp_buffer[W25N04_DATA_SIZE];
    bool is_all_ff;

    if (storage_config.state != STORAGE_IDLE) return;
    
    storage_config.state = STORAGE_READING;
    
    // 使用二分查找找到最后一个非全0xFF的帧
    while (left <= right)
    {
        uint32 mid = left + (right - left) / 2;
        uint32 frame_start_page = mid * image_storage_page;  // 当前帧的起始页
        
        // 读取该帧的第一页
        if (!w25n04_read_page(frame_start_page, 0, temp_buffer, W25N04_DATA_SIZE))
        {
            storage_config.error = STORAGE_READ_FAILED;
            storage_config.state = STORAGE_ERROR;
            return;
        }
        
        // 检查该页是否全为0xFF
        is_all_ff = true;
        for (uint16 i = 0; i < W25N04_DATA_SIZE; i++)
        {
            if (temp_buffer[i] != 0xFF)
            {
                is_all_ff = false;
                break;
            }
        }
        
        if (is_all_ff)
        {
            right = mid - 1;
        }
        else
        {
            last_valid_frame = mid;
            left = mid + 1;
        }
    }
    
    // 处理结果
    if (last_valid_frame == 0 && is_all_ff)
    {
        // 检查第一帧是否为有效帧
        if (!w25n04_read_page(0, 0, temp_buffer, W25N04_DATA_SIZE))
        {
            storage_config.error = STORAGE_READ_FAILED;
            storage_config.state = STORAGE_ERROR;
            return;
        }
        
        is_all_ff = true;
        for (uint16 i = 0; i < W25N04_DATA_SIZE; i++)
        {
            if (temp_buffer[i] != 0xFF)
            {
                is_all_ff = false;
                break;
            }
        }
        
        if (is_all_ff)
        {
            storage_config.frame_count = 0;
        }
        else
        {
            storage_config.frame_count = 1;
        }
    }
    else
    {
        storage_config.frame_count = last_valid_frame - 1;
    }
    
    storage_config.state = STORAGE_IDLE;
}
