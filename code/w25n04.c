#include "w25n04.h"
#include "zf_common_headfile.h"  // 包含头文件

/**
 * @brief 片选信号控制
 * 
 * @param state 1：取消片选，0：选中芯片
 */
static void w25n04_cs(uint8 state)
{
    gpio_set_level(W25N04_CS_PIN, state);   // 控制CS引脚电平
}

/**
 * @brief 发送一个字节到W25N04KV（不接收数据）
 * 
 * @param data 要发送的字节
 */
static void w25n04_send_byte(uint8 data)
{
    spi_write_8bit(W25N04_SPI, data);   // 通过SPI只发送一个字节
}

/**
 * @brief 从W25N04KV接收一个字节（发送dummy字节）
 * 
 * @return uint8 接收到的字节
 */
static uint8 w25n04_receive_byte(void)
{
    return spi_read_8bit(W25N04_SPI);   // 通过SPI只接收一个字节（发送0xFF作为dummy字节）
}

/**
 * @brief 发送多个字节数据
 * 
 * @param data 要发送的数据缓冲区
 * @param len 数据长度
 */
static void w25n04_send_data(const uint8 *data, uint16 len)
{
    spi_write_8bit_array(W25N04_SPI, data, len);
}

/**
 * @brief 接收多个字节数据
 * 
 * @param data 接收数据的缓冲区
 * @param len 要接收的数据长度
 */
static void w25n04_receive_data(uint8 *data, uint16 len)
{
    spi_read_8bit_array(W25N04_SPI, data, len);
}

/**
 * @brief 发送并接收一个字节（同步传输）
 * 
 * @param data 要发送的字节
 * @return uint8 同时接收到的字节
 */
static uint8 w25n04_transfer_byte(uint8 data)
{
    uint8 rx_data;
    spi_transfer_8bit(W25N04_SPI, &data, &rx_data, 1);
    return rx_data;
}

/**
 * @brief 发送16位数据到W25N04KV
 * 
 * @param data 要发送的16位数据
 */
static void w25n04_send_word(uint16 data)
{
    spi_write_16bit(W25N04_SPI, data);   // 通过SPI发送16位数据
}

/**
 * @brief 从W25N04KV接收16位数据
 * 
 * @return uint16 接收到的16位数据
 */
static uint16 w25n04_receive_word(void)
{
    return spi_read_16bit(W25N04_SPI);   // 通过SPI接收16位数据
}

/**
 * @brief 发送多个16位数据
 * 
 * @param data 要发送的16位数据缓冲区
 * @param len 16位数据的数量
 */
static void w25n04_send_data_16bit(const uint16 *data, uint16 len)
{
    spi_write_16bit_array(W25N04_SPI, data, len);
}

/**
 * @brief 接收多个16位数据
 * 
 * @param data 接收16位数据的缓冲区
 * @param len 要接收的16位数据数量
 */
static void w25n04_receive_data_16bit(uint16 *data, uint16 len)
{
    spi_read_16bit_array(W25N04_SPI, data, len);
}

/**
 * @brief 读取W25N04KV的JEDEC ID
 * 
 * @param manufacturer_id 存储制造商ID的指针
 * @param device_id 存储设备ID的指针
 * @return uint8 0:失败，1:成功
 */
uint8 w25n04_read_id(uint8 *manufacturer_id, uint16 *device_id)
{
    uint8 id_data[3];
    
    // 选中芯片
    w25n04_cs(0);
    
    // 发送读取JEDEC ID指令
    w25n04_send_byte(W25N04_CMD_READ_JEDEC_ID);
    w25n04_send_byte(0xFF);
    
    // 读取3个字节的ID数据
    id_data[0] = w25n04_receive_byte();  // 制造商ID
    id_data[1] = w25n04_receive_byte();  // 设备ID高字节
    id_data[2] = w25n04_receive_byte();  // 设备ID低字节
    
    // 取消片选
    w25n04_cs(1);
    
    // 保存读取到的ID值
    *manufacturer_id = id_data[0];
    *device_id = (id_data[1] << 8) | id_data[2];
    
    // 验证ID是否正确
    if (*manufacturer_id == W25N04_MANUFACTURER_ID && 
        *device_id == W25N04_DEVICE_ID) {
        return 1;  // ID验证成功
    } else {
        return 0;  // ID验证失败
    }
}

/**
 * @brief 初始化W25N04KV闪存
 * 
 * @return uint8 0:失败，1:成功
 */
uint8 w25n04_init(void)
{
    uint8 ret;
    uint8 manufacturer_id;
    uint16 device_id;
    
    // 初始化CS、WP和HOLD引脚，默认都输出高电平
    gpio_init(W25N04_CS_PIN, GPO, 1, GPO_PUSH_PULL);
    gpio_init(W25N04_WP_PIN, GPO, 1, GPO_PUSH_PULL);
    gpio_init(W25N04_HOLD_PIN, GPO, 1, GPO_PUSH_PULL);
    
    // 初始化SPI引脚，使用SPI_CS_NULL表示CS引脚由软件控制
    spi_init(W25N04_SPI, W25N04_SPI_MODE, W25N04_SPI_SPEED, 
             W25N04_SCK_PIN, W25N04_MOSI_PIN, W25N04_MISO_PIN, SPI_CS_NULL);
             
    ret = w25n04_reset(0);
    if(ret == 0)
    {
       zf_log(0, "W25N04KV reset failed!");
    }

    ret = w25n04_disable_write_protection();
    if(ret == 0)
    {
        zf_log(0, "W25N04KV write protection failed!");
    }

    ret = w25n04_read_id(&manufacturer_id, &device_id);
    if(ret == 0)
    {
        zf_log(0, "W25N04KV read id failed!");
    }
    return 1;  // 初始化成功
}

/**
 * @brief 读取W25N04KV的状态寄存器
 * 
 * @param reg_addr 寄存器地址(0xA0/0xB0/0xC0)
 * @return uint8 状态寄存器值
 */
uint8 w25n04_read_status(uint8 reg_addr)
{
    uint8 status;
    
    // 选中芯片
    w25n04_cs(0);
    
    // 发送读取状态寄存器指令和寄存器地址
    w25n04_send_byte(W25N04_CMD_GET_FEATURES);
    w25n04_send_byte(reg_addr);
    
    // 读取状态寄存器值
    status = w25n04_receive_byte();
    
    // 取消片选
    w25n04_cs(1);
    
    return status;
}

/**
 * @brief 等待W25N04KV不忙
 * 
 * @param timeout_ms 超时时间(毫秒)，0表示一直等待直到不忙
 * @return uint8 0:超时，1:成功(设备不忙)
 */
uint8 w25n04_wait_busy(uint32 timeout_ms)
{
    uint8 status;
    uint32 start_time = system_getval_ms();  // 获取当前时间
    
    do {
        // 读取状态寄存器3，检查BUSY位
        status = w25n04_read_status(W25N04_SR3_ADDR);
        
        // 如果BUSY=0，表示设备不忙，返回成功
        if (!(status & W25N04_SR3_BUSY)) {
            return 1;
        }
        
        // 短暂延时，避免频繁查询造成的系统负担
        system_delay_us(10);
        
        // 检查是否超时(如果timeout_ms=0则一直等待)
        if (timeout_ms > 0 && (system_getval_ms() - start_time) > timeout_ms) {
            return 0;  // 超时返回失败
        }
    } while (1);  // 持续检查直到不忙或超时
}

/**
 * @brief 系统复位W25N04KV芯片
 * 
 * @param reset_mode 复位模式：0-使用单一复位指令(0xFF)，1-使用复位使能+复位设备指令(0x66/0x99)
 * @return uint8 0:失败，1:成功
 */
uint8 w25n04_reset(uint8 reset_mode)
{
    // 首先确保不在忙状态
    if (w25n04_wait_busy(100) == 0) {
        return 0;  // 设备忙，无法执行复位
    }
    
    if (reset_mode == 0) {
        // 使用单一复位指令
        w25n04_cs(0);
        w25n04_send_byte(W25N04_CMD_RESET);
        w25n04_cs(1);
    } else {
        // 使用复位使能+复位设备指令
        w25n04_cs(0);
        w25n04_send_byte(W25N04_CMD_RESET_ENABLE);
        w25n04_cs(1);
        
        // 等待设备准备好接收复位设备指令
        
        if(w25n04_wait_busy(10) == 0){
            return 0;
        } 
        
        w25n04_cs(0);
        w25n04_send_byte(W25N04_CMD_RESET_DEVICE);
        w25n04_cs(1);
    }
    
    // 等待复位完成，根据规格书，复位时间取决于当前状态，最长可达500us
    system_delay_us(500);
    
    // 等待设备不忙，超时时间100ms
    if (w25n04_wait_busy(100) == 0) {
        return 0;  // 复位失败
    }
    
    return 1;  // 复位成功
}

/**
 * @brief 解除W25N04KV的写保护
 * 
 * @return uint8 0:失败，1:成功
 */
uint8 w25n04_disable_write_protection(void)
{
    uint8 status;
    
    // 读取状态寄存器1的当前值
    status = w25n04_read_status(W25N04_SR1_ADDR);
    
    // 清除所有写保护相关位：WP_E(位1)及BP0-BP3(位3-6)
    status &= ~(W25N04_SR1_WP_E | W25N04_SR1_BP0 | W25N04_SR1_BP1 | 
                W25N04_SR1_BP2 | W25N04_SR1_BP3);
    
    // 选中芯片
    w25n04_cs(0);
    
    // 发送写使能指令
    w25n04_send_byte(W25N04_CMD_WRITE_ENABLE);
    
    // 取消片选
    w25n04_cs(1);
    
    // 短暂延时确保写使能生效
    system_delay_us(10);
    
    // 选中芯片
    w25n04_cs(0);
    
    // 发送设置特性指令和状态寄存器1地址
    w25n04_send_byte(W25N04_CMD_SET_FEATURES);
    w25n04_send_byte(W25N04_SR1_ADDR);
    
    // 发送清除了写保护位的状态值
    w25n04_send_byte(status);
    
    // 取消片选
    w25n04_cs(1);
    
    // 等待写入完成
    if (w25n04_wait_busy(100) == 0) {
        return 0;  // 等待超时，失败
    }
    
    // 验证写保护是否成功关闭
    status = w25n04_read_status(W25N04_SR1_ADDR);
    if (status & (W25N04_SR1_WP_E | W25N04_SR1_BP0 | W25N04_SR1_BP1 | 
                 W25N04_SR1_BP2 | W25N04_SR1_BP3)) {
        return 0;  // 写保护位未清除，失败
    }
    
    return 1;  // 成功关闭写保护
}

/**
 * @brief 擦除指定的128KB块
 * 
 * @param block_addr 块地址（0-4095）
 * @return uint8 0:失败，1:成功，2:写使能失败，3:擦除失败(E_FAIL位置1)
 */
uint8 w25n04_block_erase(uint16 block_addr)
{
    uint8 status;
    
    // 块地址范围检查
    if (block_addr >= W25N04_BLOCK_COUNT)
    {
        return 0;  // 无效的块地址
    }
    
    // 解除写保护（可选，根据实际需求决定是否调用）
    // w25n04_disable_write_protection();
    
    // 设置写使能
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_WRITE_ENABLE);
    w25n04_cs(1);
    
    // 检查WEL位是否被置位
    status = w25n04_read_status(W25N04_SR3_ADDR);
    if ((status & W25N04_SR3_WEL) == 0)
    {
        return 2;  // 写使能失败
    }
    
    // 计算页地址（每个块包含64个页）
    uint32 page_addr = block_addr * W25N04_PAGES_PER_BLOCK;
    
    // 发送块擦除指令
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_BLOCK_ERASE_128KB);
    
    // 发送页地址（24位地址：高8位、中8位、低8位）
    // 注意：低6位对应页内偏移，应为0
    w25n04_send_byte((uint8)(page_addr >> 16));     // 高8位
    w25n04_send_byte((uint8)(page_addr >> 8));      // 中8位
    w25n04_send_byte((uint8)(page_addr & 0xC0));    // 低8位（确保低6位为0）
    
    w25n04_cs(1);
    
    // 等待擦除完成（可能需要10ms级别的时间）
    if (w25n04_wait_busy(W25N04_TIMEOUT_BLOCK_ERASE) == 0)
    {
        return 0;  // 等待超时
    }
    
    // 检查擦除是否成功
    status = w25n04_read_status(W25N04_SR3_ADDR);
    if (status & W25N04_SR3_E_FAIL)
    {
        // 清除擦除失败标志位
        uint8 sr3 = status & ~W25N04_SR3_E_FAIL;
        
        // 写入新的状态寄存器值
        w25n04_cs(0);
        w25n04_send_byte(W25N04_CMD_SET_FEATURES);
        w25n04_send_byte(W25N04_SR3_ADDR);
        w25n04_send_byte(sr3);
        w25n04_cs(1);
        
        return 3;  // 擦除失败
    }
    
    return 1;  // 擦除成功
}

/**
 * @brief 将指定页数据加载到缓冲区
 * 
 * @param page_addr 页地址
 * @return uint8 0:失败或超时，1:成功无ECC错误，2:成功但有ECC可纠正错误，3:ECC错误无法纠正
 */
uint8 w25n04_page_data_read(uint32 page_addr)
{
    uint8 status;
    
    // 页地址范围检查
    if (page_addr >= W25N04_TOTAL_PAGES)
    {
        return 0;  // 无效的页地址
    }
    
    // 发送页数据读取命令
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_PAGE_DATA_READ);
    
    // 发送页地址（24位地址）
    w25n04_send_byte((uint8)(page_addr >> 16));     // 高8位
    w25n04_send_byte((uint8)(page_addr >> 8));      // 中8位
    w25n04_send_byte((uint8)(page_addr));           // 低8位
    
    w25n04_cs(1);
    
    // 等待读取完成
    if (w25n04_wait_busy(W25N04_TIMEOUT_PAGE_READ) == 0)
    {
        return 0;  // 等待超时
    }
    
    // 读取状态寄存器3检查ECC状态
    status = w25n04_read_status(W25N04_SR3_ADDR);
    
    // 检查ECCS[1:0]位
    uint8 ecc_status = (status >> 4) & 0x03;
    
    switch (ecc_status)
    {
        case 0:  // 00 - 没有ECC错误
            return 1;
        case 1:  // 01 - 1位ECC错误，已纠正
            return 2;
        case 2:  // 10 - 2位ECC错误，已纠正（需确认）
            return 2;
        case 3:  // 11 - 无法纠正的ECC错误
            return 3;
        default:
            return 0;  // 不应该到达这里
    }
}

/**
 * @brief 从缓冲区读取数据
 * 
 * @param column_addr 列地址(0-2111)
 * @param data 读取数据的存储位置
 * @param len 要读取的数据长度
 * @return uint8 0:失败，1:成功
 */
uint8 w25n04_read_data(uint16 column_addr, uint8 *data, uint16 len)
{
    // 列地址范围检查
    if (column_addr >= W25N04_PAGE_SIZE)
    {
        return 0;  // 无效的列地址
    }
    
    // 确保不会读取超出页边界的数据
    if (column_addr + len > W25N04_PAGE_SIZE)
    {
        len = W25N04_PAGE_SIZE - column_addr;
    }
    
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_READ_DATA);
    w25n04_send_byte((uint8)(column_addr >> 8));
    w25n04_send_byte((uint8)(column_addr));
    w25n04_send_byte(0xFF);  // 空字节
    
    // 使用16位传输
    if (len >= 2)
    {
        uint16 word_count = len / 2;
        uint16 *word_data = (uint16 *)data;
        
        // 读取16位数据
        for (uint16 i = 0; i < word_count; i++)
        {
            word_data[i] = w25n04_receive_word();
        }
        
        // 处理剩余的单字节
        if (len % 2)
        {
            data[len - 1] = w25n04_receive_byte();
        }
    }
    else
    {
        // 对于单字节数据，使用8位传输
        for (uint16 i = 0; i < len; i++)
        {
            data[i] = w25n04_receive_byte();
        }
    }
    
    w25n04_cs(1);
    return 1;
}

/**
 * @brief 读取指定页的数据
 * 
 * @param page_addr 页地址
 * @param column_addr 列地址
 * @param data 读取数据的存储位置
 * @param len 要读取的数据长度
 * @return uint8 0:失败，1:成功无ECC错误，2:成功但有ECC错误可纠正，3:ECC错误无法纠正
 */
uint8 w25n04_read_page(uint32 page_addr, uint16 column_addr, uint8 *data, uint16 len)
{
    uint8 result;
    
    // 页地址范围检查
    if (page_addr >= W25N04_TOTAL_PAGES)
    {
        return 0;  // 无效的页地址
    }
    
    // 列地址范围检查
    if (column_addr >= W25N04_PAGE_SIZE)
    {
        return 0;  // 无效的列地址
    }
    
    // 确保不会读取超出页边界的数据
    if (column_addr + len > W25N04_PAGE_SIZE)
    {
        len = W25N04_PAGE_SIZE - column_addr;  // 调整长度以适应页边界
    }
    
    // 第1步：将指定页数据加载到芯片内部缓冲区
    result = w25n04_page_data_read(page_addr);
    
    // 如果页读取失败或有不可纠正的ECC错误，则直接返回
    if (result == 0 || result == 3)
    {
        return result;
    }
    
    // 第2步：从缓冲区读取数据
    if (w25n04_read_data(column_addr, data, len) == 0)
    {
        return 0;  // 从缓冲区读取数据失败
    }
    
    // 返回页读取的结果（1=无错误，2=有可纠正的ECC错误）
    return result;
}

/**
 * @brief 加载数据到内部页缓冲区
 * 
 * @param column_addr 列地址(0-2111)
 * @param data 要加载的数据
 * @param len 数据长度
 * @return uint8 0:失败，1:成功
 */
uint8 w25n04_program_data_load(uint16 column_addr, uint8 *data, uint16 len)
{
    if (column_addr >= W25N04_PAGE_SIZE)
    {
        return 0;
    }
    
    if (column_addr + len > W25N04_PAGE_SIZE)
    {
        len = W25N04_PAGE_SIZE - column_addr;
    }
    
    if (!w25n04_write_enable())
    {
        return 0;
    }
    
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_LOAD_PROGRAM_DATA);
    w25n04_send_byte((uint8)(column_addr >> 8));
    w25n04_send_byte((uint8)(column_addr));
    
    // 使用16位传输
    if (len >= 2)
    {
        uint16 word_count = len / 2;
        uint16 *word_data = (uint16 *)data;
        
        // 发送16位数据
        for (uint16 i = 0; i < word_count; i++)
        {
            w25n04_send_word(word_data[i]);
        }
        
        // 处理剩余的单字节
        if (len % 2)
        {
            w25n04_send_byte(data[len - 1]);
        }
    }
    else
    {
        // 对于单字节数据，使用8位传输
        for (uint16 i = 0; i < len; i++)
        {
            w25n04_send_byte(data[i]);
        }
    }
    
    w25n04_cs(1);
    return 1;
}

/**
 * @brief 执行编程操作，将内部缓冲区数据写入到指定页
 * 
 * @param page_addr 要写入的页地址
 * @return uint8 0:失败，1:成功，2:编程失败(P_FAIL位置1)
 */
uint8 w25n04_program_execute(uint32 page_addr)
{
    // 页地址范围检查
    if (page_addr >= W25N04_TOTAL_PAGES)
    {
        return 0;  // 无效的页地址
    }
    
    // 写使能
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_WRITE_ENABLE);
    w25n04_cs(1);
    
    // 检查写使能是否成功
    uint8 status = w25n04_read_status(W25N04_SR3_ADDR);
    if ((status & W25N04_SR3_WEL) == 0)
    {
        return 0;  // 写使能失败
    }
    
    // 发送编程执行命令
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_PROGRAM_EXECUTE);
    
    // 发送页地址（24位地址）
    w25n04_send_byte((uint8)(page_addr >> 16));  // 高8位
    w25n04_send_byte((uint8)(page_addr >> 8));   // 中8位
    w25n04_send_byte((uint8)(page_addr));        // 低8位
    
    w25n04_cs(1);
    
    // 等待编程完成
    if (w25n04_wait_busy(W25N04_TIMEOUT_PAGE_PROG) == 0)
    {
        return 0;  // 等待超时
    }
    
    // 检查编程是否成功
    status = w25n04_read_status(W25N04_SR3_ADDR);
    if (status & W25N04_SR3_P_FAIL)
    {
        // 清除编程失败标志位
        uint8 sr3 = status & ~W25N04_SR3_P_FAIL;
        
        // 写入新的状态寄存器值
        w25n04_cs(0);
        w25n04_send_byte(W25N04_CMD_SET_FEATURES);
        w25n04_send_byte(W25N04_SR3_ADDR);
        w25n04_send_byte(sr3);
        w25n04_cs(1);
        
        return 2;  // 编程失败
    }
    
    return 1;  // 编程成功
}

/**
 * @brief 写入数据到指定页
 * 
 * @param page_addr 页地址
 * @param column_addr 列地址
 * @param data 要写入的数据缓冲区
 * @param len 数据长度
 * @return uint8 0:失败，1:成功，2:程序执行失败
 */
uint8 w25n04_write_page(uint32 page_addr, uint16 column_addr, uint8 *data, uint16 len)
{
    uint8 result;
    
    // 页地址范围检查
    if (page_addr >= W25N04_TOTAL_PAGES)
    {
        return 0;  // 无效的页地址
    }
    
    // 列地址范围检查
    if (column_addr >= W25N04_PAGE_SIZE)
    {
        return 0;  // 无效的列地址
    }
    
    // 确保不会写入超出页边界的数据
    if (column_addr + len > W25N04_PAGE_SIZE)
    {
        len = W25N04_PAGE_SIZE - column_addr;  // 调整长度以适应页边界
    }
    
    // 第1步：将数据加载到芯片内部缓冲区
    if (w25n04_program_data_load(column_addr, data, len) == 0)
    {
        return 0;  // 数据加载失败
    }
    
    // 第2步：执行编程操作将缓冲区数据写入到Flash
    result = w25n04_program_execute(page_addr);
    
    return result;  // 返回编程结果
}

/**
 * @brief 随机加载数据到内部页缓冲区(不重置缓冲区)
 * 
 * @param column_addr 列地址(0-2111)
 * @param data 要加载的数据
 * @param len 数据长度
 * @return uint8 0:失败，1:成功
 */
uint8 w25n04_random_program_data_load(uint16 column_addr, uint8 *data, uint16 len)
{
    // 列地址范围检查
    if (column_addr >= W25N04_PAGE_SIZE)
    {
        return 0;  // 无效的列地址
    }
    
    // 确保不会加载超出页边界的数据
    if (column_addr + len > W25N04_PAGE_SIZE)
    {
        len = W25N04_PAGE_SIZE - column_addr;  // 调整长度以适应页边界
    }
    
    // 写使能
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_WRITE_ENABLE);
    w25n04_cs(1);
    
    // 检查写使能是否成功
    uint8 status = w25n04_read_status(W25N04_SR3_ADDR);
    if ((status & W25N04_SR3_WEL) == 0)
    {
        return 0;  // 写使能失败
    }
    
    // 随机加载程序数据命令(不重置缓冲区)
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_LOAD_RANDOM_PROGRAM_DATA);
    
    // 发送列地址（16位地址）
    w25n04_send_byte((uint8)(column_addr >> 8));  // 高8位
    w25n04_send_byte((uint8)(column_addr));       // 低8位
    
    // 发送数据内容
    for (uint16 i = 0; i < len; i++)
    {
        w25n04_send_byte(data[i]);
    }
    
    w25n04_cs(1);
    
    return 1;  // 数据加载成功
}

/**
 * @brief 写使能
 * 
 * @return uint8 0:失败，1:成功
 */
uint8 w25n04_write_enable(void)
{
    // 写使能
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_WRITE_ENABLE);
    w25n04_cs(1);
    
    // 检查写使能是否成功
    uint8 status = w25n04_read_status(W25N04_SR3_ADDR);
    if ((status & W25N04_SR3_WEL) == 0)
    {
        return 0;  // 写使能失败
    }
    
    return 1;  // 写使能成功
}
