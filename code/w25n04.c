#include "w25n04.h"
#include "zf_common_headfile.h"  // ����ͷ�ļ�

/**
 * @brief Ƭѡ�źſ���
 * 
 * @param state 1��ȡ��Ƭѡ��0��ѡ��оƬ
 */
static void w25n04_cs(uint8 state)
{
    gpio_set_level(W25N04_CS_PIN, state);   // ����CS���ŵ�ƽ
}

/**
 * @brief ����һ���ֽڵ�W25N04KV�����������ݣ�
 * 
 * @param data Ҫ���͵��ֽ�
 */
static void w25n04_send_byte(uint8 data)
{
    spi_write_8bit(W25N04_SPI, data);   // ͨ��SPIֻ����һ���ֽ�
}

/**
 * @brief ��W25N04KV����һ���ֽڣ�����dummy�ֽڣ�
 * 
 * @return uint8 ���յ����ֽ�
 */
static uint8 w25n04_receive_byte(void)
{
    return spi_read_8bit(W25N04_SPI);   // ͨ��SPIֻ����һ���ֽڣ�����0xFF��Ϊdummy�ֽڣ�
}

/**
 * @brief ���Ͷ���ֽ�����
 * 
 * @param data Ҫ���͵����ݻ�����
 * @param len ���ݳ���
 */
static void w25n04_send_data(const uint8 *data, uint16 len)
{
    spi_write_8bit_array(W25N04_SPI, data, len);
}

/**
 * @brief ���ն���ֽ�����
 * 
 * @param data �������ݵĻ�����
 * @param len Ҫ���յ����ݳ���
 */
static void w25n04_receive_data(uint8 *data, uint16 len)
{
    spi_read_8bit_array(W25N04_SPI, data, len);
}

/**
 * @brief ���Ͳ�����һ���ֽڣ�ͬ�����䣩
 * 
 * @param data Ҫ���͵��ֽ�
 * @return uint8 ͬʱ���յ����ֽ�
 */
static uint8 w25n04_transfer_byte(uint8 data)
{
    uint8 rx_data;
    spi_transfer_8bit(W25N04_SPI, &data, &rx_data, 1);
    return rx_data;
}

/**
 * @brief ����16λ���ݵ�W25N04KV
 * 
 * @param data Ҫ���͵�16λ����
 */
static void w25n04_send_word(uint16 data)
{
    spi_write_16bit(W25N04_SPI, data);   // ͨ��SPI����16λ����
}

/**
 * @brief ��W25N04KV����16λ����
 * 
 * @return uint16 ���յ���16λ����
 */
static uint16 w25n04_receive_word(void)
{
    return spi_read_16bit(W25N04_SPI);   // ͨ��SPI����16λ����
}

/**
 * @brief ���Ͷ��16λ����
 * 
 * @param data Ҫ���͵�16λ���ݻ�����
 * @param len 16λ���ݵ�����
 */
static void w25n04_send_data_16bit(const uint16 *data, uint16 len)
{
    spi_write_16bit_array(W25N04_SPI, data, len);
}

/**
 * @brief ���ն��16λ����
 * 
 * @param data ����16λ���ݵĻ�����
 * @param len Ҫ���յ�16λ��������
 */
static void w25n04_receive_data_16bit(uint16 *data, uint16 len)
{
    spi_read_16bit_array(W25N04_SPI, data, len);
}

/**
 * @brief ��ȡW25N04KV��JEDEC ID
 * 
 * @param manufacturer_id �洢������ID��ָ��
 * @param device_id �洢�豸ID��ָ��
 * @return uint8 0:ʧ�ܣ�1:�ɹ�
 */
uint8 w25n04_read_id(uint8 *manufacturer_id, uint16 *device_id)
{
    uint8 id_data[3];
    
    // ѡ��оƬ
    w25n04_cs(0);
    
    // ���Ͷ�ȡJEDEC IDָ��
    w25n04_send_byte(W25N04_CMD_READ_JEDEC_ID);
    w25n04_send_byte(0xFF);
    
    // ��ȡ3���ֽڵ�ID����
    id_data[0] = w25n04_receive_byte();  // ������ID
    id_data[1] = w25n04_receive_byte();  // �豸ID���ֽ�
    id_data[2] = w25n04_receive_byte();  // �豸ID���ֽ�
    
    // ȡ��Ƭѡ
    w25n04_cs(1);
    
    // �����ȡ����IDֵ
    *manufacturer_id = id_data[0];
    *device_id = (id_data[1] << 8) | id_data[2];
    
    // ��֤ID�Ƿ���ȷ
    if (*manufacturer_id == W25N04_MANUFACTURER_ID && 
        *device_id == W25N04_DEVICE_ID) {
        return 1;  // ID��֤�ɹ�
    } else {
        return 0;  // ID��֤ʧ��
    }
}

/**
 * @brief ��ʼ��W25N04KV����
 * 
 * @return uint8 0:ʧ�ܣ�1:�ɹ�
 */
uint8 w25n04_init(void)
{
    uint8 ret;
    uint8 manufacturer_id;
    uint16 device_id;
    
    // ��ʼ��CS��WP��HOLD���ţ�Ĭ�϶�����ߵ�ƽ
    gpio_init(W25N04_CS_PIN, GPO, 1, GPO_PUSH_PULL);
    gpio_init(W25N04_WP_PIN, GPO, 1, GPO_PUSH_PULL);
    gpio_init(W25N04_HOLD_PIN, GPO, 1, GPO_PUSH_PULL);
    
    // ��ʼ��SPI���ţ�ʹ��SPI_CS_NULL��ʾCS�������������
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
    return 1;  // ��ʼ���ɹ�
}

/**
 * @brief ��ȡW25N04KV��״̬�Ĵ���
 * 
 * @param reg_addr �Ĵ�����ַ(0xA0/0xB0/0xC0)
 * @return uint8 ״̬�Ĵ���ֵ
 */
uint8 w25n04_read_status(uint8 reg_addr)
{
    uint8 status;
    
    // ѡ��оƬ
    w25n04_cs(0);
    
    // ���Ͷ�ȡ״̬�Ĵ���ָ��ͼĴ�����ַ
    w25n04_send_byte(W25N04_CMD_GET_FEATURES);
    w25n04_send_byte(reg_addr);
    
    // ��ȡ״̬�Ĵ���ֵ
    status = w25n04_receive_byte();
    
    // ȡ��Ƭѡ
    w25n04_cs(1);
    
    return status;
}

/**
 * @brief �ȴ�W25N04KV��æ
 * 
 * @param timeout_ms ��ʱʱ��(����)��0��ʾһֱ�ȴ�ֱ����æ
 * @return uint8 0:��ʱ��1:�ɹ�(�豸��æ)
 */
uint8 w25n04_wait_busy(uint32 timeout_ms)
{
    uint8 status;
    uint32 start_time = system_getval_ms();  // ��ȡ��ǰʱ��
    
    do {
        // ��ȡ״̬�Ĵ���3�����BUSYλ
        status = w25n04_read_status(W25N04_SR3_ADDR);
        
        // ���BUSY=0����ʾ�豸��æ�����سɹ�
        if (!(status & W25N04_SR3_BUSY)) {
            return 1;
        }
        
        // ������ʱ������Ƶ����ѯ��ɵ�ϵͳ����
        system_delay_us(10);
        
        // ����Ƿ�ʱ(���timeout_ms=0��һֱ�ȴ�)
        if (timeout_ms > 0 && (system_getval_ms() - start_time) > timeout_ms) {
            return 0;  // ��ʱ����ʧ��
        }
    } while (1);  // �������ֱ����æ��ʱ
}

/**
 * @brief ϵͳ��λW25N04KVоƬ
 * 
 * @param reset_mode ��λģʽ��0-ʹ�õ�һ��λָ��(0xFF)��1-ʹ�ø�λʹ��+��λ�豸ָ��(0x66/0x99)
 * @return uint8 0:ʧ�ܣ�1:�ɹ�
 */
uint8 w25n04_reset(uint8 reset_mode)
{
    // ����ȷ������æ״̬
    if (w25n04_wait_busy(100) == 0) {
        return 0;  // �豸æ���޷�ִ�и�λ
    }
    
    if (reset_mode == 0) {
        // ʹ�õ�һ��λָ��
        w25n04_cs(0);
        w25n04_send_byte(W25N04_CMD_RESET);
        w25n04_cs(1);
    } else {
        // ʹ�ø�λʹ��+��λ�豸ָ��
        w25n04_cs(0);
        w25n04_send_byte(W25N04_CMD_RESET_ENABLE);
        w25n04_cs(1);
        
        // �ȴ��豸׼���ý��ո�λ�豸ָ��
        
        if(w25n04_wait_busy(10) == 0){
            return 0;
        } 
        
        w25n04_cs(0);
        w25n04_send_byte(W25N04_CMD_RESET_DEVICE);
        w25n04_cs(1);
    }
    
    // �ȴ���λ��ɣ����ݹ���飬��λʱ��ȡ���ڵ�ǰ״̬����ɴ�500us
    system_delay_us(500);
    
    // �ȴ��豸��æ����ʱʱ��100ms
    if (w25n04_wait_busy(100) == 0) {
        return 0;  // ��λʧ��
    }
    
    return 1;  // ��λ�ɹ�
}

/**
 * @brief ���W25N04KV��д����
 * 
 * @return uint8 0:ʧ�ܣ�1:�ɹ�
 */
uint8 w25n04_disable_write_protection(void)
{
    uint8 status;
    
    // ��ȡ״̬�Ĵ���1�ĵ�ǰֵ
    status = w25n04_read_status(W25N04_SR1_ADDR);
    
    // �������д�������λ��WP_E(λ1)��BP0-BP3(λ3-6)
    status &= ~(W25N04_SR1_WP_E | W25N04_SR1_BP0 | W25N04_SR1_BP1 | 
                W25N04_SR1_BP2 | W25N04_SR1_BP3);
    
    // ѡ��оƬ
    w25n04_cs(0);
    
    // ����дʹ��ָ��
    w25n04_send_byte(W25N04_CMD_WRITE_ENABLE);
    
    // ȡ��Ƭѡ
    w25n04_cs(1);
    
    // ������ʱȷ��дʹ����Ч
    system_delay_us(10);
    
    // ѡ��оƬ
    w25n04_cs(0);
    
    // ������������ָ���״̬�Ĵ���1��ַ
    w25n04_send_byte(W25N04_CMD_SET_FEATURES);
    w25n04_send_byte(W25N04_SR1_ADDR);
    
    // ���������д����λ��״ֵ̬
    w25n04_send_byte(status);
    
    // ȡ��Ƭѡ
    w25n04_cs(1);
    
    // �ȴ�д�����
    if (w25n04_wait_busy(100) == 0) {
        return 0;  // �ȴ���ʱ��ʧ��
    }
    
    // ��֤д�����Ƿ�ɹ��ر�
    status = w25n04_read_status(W25N04_SR1_ADDR);
    if (status & (W25N04_SR1_WP_E | W25N04_SR1_BP0 | W25N04_SR1_BP1 | 
                 W25N04_SR1_BP2 | W25N04_SR1_BP3)) {
        return 0;  // д����λδ�����ʧ��
    }
    
    return 1;  // �ɹ��ر�д����
}

/**
 * @brief ����ָ����128KB��
 * 
 * @param block_addr ���ַ��0-4095��
 * @return uint8 0:ʧ�ܣ�1:�ɹ���2:дʹ��ʧ�ܣ�3:����ʧ��(E_FAILλ��1)
 */
uint8 w25n04_block_erase(uint16 block_addr)
{
    uint8 status;
    
    // ���ַ��Χ���
    if (block_addr >= W25N04_BLOCK_COUNT)
    {
        return 0;  // ��Ч�Ŀ��ַ
    }
    
    // ���д��������ѡ������ʵ����������Ƿ���ã�
    // w25n04_disable_write_protection();
    
    // ����дʹ��
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_WRITE_ENABLE);
    w25n04_cs(1);
    
    // ���WELλ�Ƿ���λ
    status = w25n04_read_status(W25N04_SR3_ADDR);
    if ((status & W25N04_SR3_WEL) == 0)
    {
        return 2;  // дʹ��ʧ��
    }
    
    // ����ҳ��ַ��ÿ�������64��ҳ��
    uint32 page_addr = block_addr * W25N04_PAGES_PER_BLOCK;
    
    // ���Ϳ����ָ��
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_BLOCK_ERASE_128KB);
    
    // ����ҳ��ַ��24λ��ַ����8λ����8λ����8λ��
    // ע�⣺��6λ��Ӧҳ��ƫ�ƣ�ӦΪ0
    w25n04_send_byte((uint8)(page_addr >> 16));     // ��8λ
    w25n04_send_byte((uint8)(page_addr >> 8));      // ��8λ
    w25n04_send_byte((uint8)(page_addr & 0xC0));    // ��8λ��ȷ����6λΪ0��
    
    w25n04_cs(1);
    
    // �ȴ�������ɣ�������Ҫ10ms�����ʱ�䣩
    if (w25n04_wait_busy(W25N04_TIMEOUT_BLOCK_ERASE) == 0)
    {
        return 0;  // �ȴ���ʱ
    }
    
    // �������Ƿ�ɹ�
    status = w25n04_read_status(W25N04_SR3_ADDR);
    if (status & W25N04_SR3_E_FAIL)
    {
        // �������ʧ�ܱ�־λ
        uint8 sr3 = status & ~W25N04_SR3_E_FAIL;
        
        // д���µ�״̬�Ĵ���ֵ
        w25n04_cs(0);
        w25n04_send_byte(W25N04_CMD_SET_FEATURES);
        w25n04_send_byte(W25N04_SR3_ADDR);
        w25n04_send_byte(sr3);
        w25n04_cs(1);
        
        return 3;  // ����ʧ��
    }
    
    return 1;  // �����ɹ�
}

/**
 * @brief ��ָ��ҳ���ݼ��ص�������
 * 
 * @param page_addr ҳ��ַ
 * @return uint8 0:ʧ�ܻ�ʱ��1:�ɹ���ECC����2:�ɹ�����ECC�ɾ�������3:ECC�����޷�����
 */
uint8 w25n04_page_data_read(uint32 page_addr)
{
    uint8 status;
    
    // ҳ��ַ��Χ���
    if (page_addr >= W25N04_TOTAL_PAGES)
    {
        return 0;  // ��Ч��ҳ��ַ
    }
    
    // ����ҳ���ݶ�ȡ����
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_PAGE_DATA_READ);
    
    // ����ҳ��ַ��24λ��ַ��
    w25n04_send_byte((uint8)(page_addr >> 16));     // ��8λ
    w25n04_send_byte((uint8)(page_addr >> 8));      // ��8λ
    w25n04_send_byte((uint8)(page_addr));           // ��8λ
    
    w25n04_cs(1);
    
    // �ȴ���ȡ���
    if (w25n04_wait_busy(W25N04_TIMEOUT_PAGE_READ) == 0)
    {
        return 0;  // �ȴ���ʱ
    }
    
    // ��ȡ״̬�Ĵ���3���ECC״̬
    status = w25n04_read_status(W25N04_SR3_ADDR);
    
    // ���ECCS[1:0]λ
    uint8 ecc_status = (status >> 4) & 0x03;
    
    switch (ecc_status)
    {
        case 0:  // 00 - û��ECC����
            return 1;
        case 1:  // 01 - 1λECC�����Ѿ���
            return 2;
        case 2:  // 10 - 2λECC�����Ѿ�������ȷ�ϣ�
            return 2;
        case 3:  // 11 - �޷�������ECC����
            return 3;
        default:
            return 0;  // ��Ӧ�õ�������
    }
}

/**
 * @brief �ӻ�������ȡ����
 * 
 * @param column_addr �е�ַ(0-2111)
 * @param data ��ȡ���ݵĴ洢λ��
 * @param len Ҫ��ȡ�����ݳ���
 * @return uint8 0:ʧ�ܣ�1:�ɹ�
 */
uint8 w25n04_read_data(uint16 column_addr, uint8 *data, uint16 len)
{
    // �е�ַ��Χ���
    if (column_addr >= W25N04_PAGE_SIZE)
    {
        return 0;  // ��Ч���е�ַ
    }
    
    // ȷ�������ȡ����ҳ�߽������
    if (column_addr + len > W25N04_PAGE_SIZE)
    {
        len = W25N04_PAGE_SIZE - column_addr;
    }
    
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_READ_DATA);
    w25n04_send_byte((uint8)(column_addr >> 8));
    w25n04_send_byte((uint8)(column_addr));
    w25n04_send_byte(0xFF);  // ���ֽ�
    
    // ʹ��16λ����
    if (len >= 2)
    {
        uint16 word_count = len / 2;
        uint16 *word_data = (uint16 *)data;
        
        // ��ȡ16λ����
        for (uint16 i = 0; i < word_count; i++)
        {
            word_data[i] = w25n04_receive_word();
        }
        
        // ����ʣ��ĵ��ֽ�
        if (len % 2)
        {
            data[len - 1] = w25n04_receive_byte();
        }
    }
    else
    {
        // ���ڵ��ֽ����ݣ�ʹ��8λ����
        for (uint16 i = 0; i < len; i++)
        {
            data[i] = w25n04_receive_byte();
        }
    }
    
    w25n04_cs(1);
    return 1;
}

/**
 * @brief ��ȡָ��ҳ������
 * 
 * @param page_addr ҳ��ַ
 * @param column_addr �е�ַ
 * @param data ��ȡ���ݵĴ洢λ��
 * @param len Ҫ��ȡ�����ݳ���
 * @return uint8 0:ʧ�ܣ�1:�ɹ���ECC����2:�ɹ�����ECC����ɾ�����3:ECC�����޷�����
 */
uint8 w25n04_read_page(uint32 page_addr, uint16 column_addr, uint8 *data, uint16 len)
{
    uint8 result;
    
    // ҳ��ַ��Χ���
    if (page_addr >= W25N04_TOTAL_PAGES)
    {
        return 0;  // ��Ч��ҳ��ַ
    }
    
    // �е�ַ��Χ���
    if (column_addr >= W25N04_PAGE_SIZE)
    {
        return 0;  // ��Ч���е�ַ
    }
    
    // ȷ�������ȡ����ҳ�߽������
    if (column_addr + len > W25N04_PAGE_SIZE)
    {
        len = W25N04_PAGE_SIZE - column_addr;  // ������������Ӧҳ�߽�
    }
    
    // ��1������ָ��ҳ���ݼ��ص�оƬ�ڲ�������
    result = w25n04_page_data_read(page_addr);
    
    // ���ҳ��ȡʧ�ܻ��в��ɾ�����ECC������ֱ�ӷ���
    if (result == 0 || result == 3)
    {
        return result;
    }
    
    // ��2�����ӻ�������ȡ����
    if (w25n04_read_data(column_addr, data, len) == 0)
    {
        return 0;  // �ӻ�������ȡ����ʧ��
    }
    
    // ����ҳ��ȡ�Ľ����1=�޴���2=�пɾ�����ECC����
    return result;
}

/**
 * @brief �������ݵ��ڲ�ҳ������
 * 
 * @param column_addr �е�ַ(0-2111)
 * @param data Ҫ���ص�����
 * @param len ���ݳ���
 * @return uint8 0:ʧ�ܣ�1:�ɹ�
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
    
    // ʹ��16λ����
    if (len >= 2)
    {
        uint16 word_count = len / 2;
        uint16 *word_data = (uint16 *)data;
        
        // ����16λ����
        for (uint16 i = 0; i < word_count; i++)
        {
            w25n04_send_word(word_data[i]);
        }
        
        // ����ʣ��ĵ��ֽ�
        if (len % 2)
        {
            w25n04_send_byte(data[len - 1]);
        }
    }
    else
    {
        // ���ڵ��ֽ����ݣ�ʹ��8λ����
        for (uint16 i = 0; i < len; i++)
        {
            w25n04_send_byte(data[i]);
        }
    }
    
    w25n04_cs(1);
    return 1;
}

/**
 * @brief ִ�б�̲��������ڲ�����������д�뵽ָ��ҳ
 * 
 * @param page_addr Ҫд���ҳ��ַ
 * @return uint8 0:ʧ�ܣ�1:�ɹ���2:���ʧ��(P_FAILλ��1)
 */
uint8 w25n04_program_execute(uint32 page_addr)
{
    // ҳ��ַ��Χ���
    if (page_addr >= W25N04_TOTAL_PAGES)
    {
        return 0;  // ��Ч��ҳ��ַ
    }
    
    // дʹ��
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_WRITE_ENABLE);
    w25n04_cs(1);
    
    // ���дʹ���Ƿ�ɹ�
    uint8 status = w25n04_read_status(W25N04_SR3_ADDR);
    if ((status & W25N04_SR3_WEL) == 0)
    {
        return 0;  // дʹ��ʧ��
    }
    
    // ���ͱ��ִ������
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_PROGRAM_EXECUTE);
    
    // ����ҳ��ַ��24λ��ַ��
    w25n04_send_byte((uint8)(page_addr >> 16));  // ��8λ
    w25n04_send_byte((uint8)(page_addr >> 8));   // ��8λ
    w25n04_send_byte((uint8)(page_addr));        // ��8λ
    
    w25n04_cs(1);
    
    // �ȴ�������
    if (w25n04_wait_busy(W25N04_TIMEOUT_PAGE_PROG) == 0)
    {
        return 0;  // �ȴ���ʱ
    }
    
    // ������Ƿ�ɹ�
    status = w25n04_read_status(W25N04_SR3_ADDR);
    if (status & W25N04_SR3_P_FAIL)
    {
        // ������ʧ�ܱ�־λ
        uint8 sr3 = status & ~W25N04_SR3_P_FAIL;
        
        // д���µ�״̬�Ĵ���ֵ
        w25n04_cs(0);
        w25n04_send_byte(W25N04_CMD_SET_FEATURES);
        w25n04_send_byte(W25N04_SR3_ADDR);
        w25n04_send_byte(sr3);
        w25n04_cs(1);
        
        return 2;  // ���ʧ��
    }
    
    return 1;  // ��̳ɹ�
}

/**
 * @brief д�����ݵ�ָ��ҳ
 * 
 * @param page_addr ҳ��ַ
 * @param column_addr �е�ַ
 * @param data Ҫд������ݻ�����
 * @param len ���ݳ���
 * @return uint8 0:ʧ�ܣ�1:�ɹ���2:����ִ��ʧ��
 */
uint8 w25n04_write_page(uint32 page_addr, uint16 column_addr, uint8 *data, uint16 len)
{
    uint8 result;
    
    // ҳ��ַ��Χ���
    if (page_addr >= W25N04_TOTAL_PAGES)
    {
        return 0;  // ��Ч��ҳ��ַ
    }
    
    // �е�ַ��Χ���
    if (column_addr >= W25N04_PAGE_SIZE)
    {
        return 0;  // ��Ч���е�ַ
    }
    
    // ȷ������д�볬��ҳ�߽������
    if (column_addr + len > W25N04_PAGE_SIZE)
    {
        len = W25N04_PAGE_SIZE - column_addr;  // ������������Ӧҳ�߽�
    }
    
    // ��1���������ݼ��ص�оƬ�ڲ�������
    if (w25n04_program_data_load(column_addr, data, len) == 0)
    {
        return 0;  // ���ݼ���ʧ��
    }
    
    // ��2����ִ�б�̲���������������д�뵽Flash
    result = w25n04_program_execute(page_addr);
    
    return result;  // ���ر�̽��
}

/**
 * @brief ����������ݵ��ڲ�ҳ������(�����û�����)
 * 
 * @param column_addr �е�ַ(0-2111)
 * @param data Ҫ���ص�����
 * @param len ���ݳ���
 * @return uint8 0:ʧ�ܣ�1:�ɹ�
 */
uint8 w25n04_random_program_data_load(uint16 column_addr, uint8 *data, uint16 len)
{
    // �е�ַ��Χ���
    if (column_addr >= W25N04_PAGE_SIZE)
    {
        return 0;  // ��Ч���е�ַ
    }
    
    // ȷ��������س���ҳ�߽������
    if (column_addr + len > W25N04_PAGE_SIZE)
    {
        len = W25N04_PAGE_SIZE - column_addr;  // ������������Ӧҳ�߽�
    }
    
    // дʹ��
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_WRITE_ENABLE);
    w25n04_cs(1);
    
    // ���дʹ���Ƿ�ɹ�
    uint8 status = w25n04_read_status(W25N04_SR3_ADDR);
    if ((status & W25N04_SR3_WEL) == 0)
    {
        return 0;  // дʹ��ʧ��
    }
    
    // ������س�����������(�����û�����)
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_LOAD_RANDOM_PROGRAM_DATA);
    
    // �����е�ַ��16λ��ַ��
    w25n04_send_byte((uint8)(column_addr >> 8));  // ��8λ
    w25n04_send_byte((uint8)(column_addr));       // ��8λ
    
    // ������������
    for (uint16 i = 0; i < len; i++)
    {
        w25n04_send_byte(data[i]);
    }
    
    w25n04_cs(1);
    
    return 1;  // ���ݼ��سɹ�
}

/**
 * @brief дʹ��
 * 
 * @return uint8 0:ʧ�ܣ�1:�ɹ�
 */
uint8 w25n04_write_enable(void)
{
    // дʹ��
    w25n04_cs(0);
    w25n04_send_byte(W25N04_CMD_WRITE_ENABLE);
    w25n04_cs(1);
    
    // ���дʹ���Ƿ�ɹ�
    uint8 status = w25n04_read_status(W25N04_SR3_ADDR);
    if ((status & W25N04_SR3_WEL) == 0)
    {
        return 0;  // дʹ��ʧ��
    }
    
    return 1;  // дʹ�ܳɹ�
}
