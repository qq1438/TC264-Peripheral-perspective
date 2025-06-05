#include "menu.h"
#include "zf_common_headfile.h"
#include "parameter_flash.h"
#include "PTS_Deal.h"

// ���ƽ���������
void draw_progress_bar(uint16 x, uint16 y, uint16 width, uint16 height, uint8 progress)
{
    uint16 i;
    uint16 filled_width;
    
    // ��Ļ�߽���
    if (x + width >= ips200_width_max)
        width = ips200_width_max - x - 5;  // ����5���ذ�ȫ�߾�
    
    if (y + height >= ips200_height_max)
        height = ips200_height_max - y - 5;  // ����5���ذ�ȫ�߾�
    
    // ȷ��progress��0-100��Χ��
    if (progress > 100)
        progress = 100;
    
    // �����������
    filled_width = (uint16)((width * progress) / 100);
    
    // ȷ��������ں���Χ��
    if (filled_width > width - 2)
        filled_width = width - 2;
    
    // ������򣨰�ɫ���Σ�
    ips200_draw_line(x, y, x + width, y, RGB565_WHITE);
    ips200_draw_line(x, y + height, x + width, y + height, RGB565_WHITE);
    ips200_draw_line(x, y, x, y + height, RGB565_WHITE);
    ips200_draw_line(x + width, y, x + width, y + height, RGB565_WHITE);
    
    // ��������ɲ��֣���ɫ���ݽ��ȱ仯��
    for (i = 1; i < height && i < 19; i++)  // �������߶�
    {
        // ���ݽ��Ȳ�ͬʹ�ò�ͬ��ɫ
        uint16 color;
        if (progress < 30)
            color = RGB565_RED;
        else if (progress < 70)
            color = RGB565_YELLOW;
        else
            color = RGB565_GREEN;
        
        // ȷ����������ʱ�����갲ȫ
        if (x + 1 < ips200_width_max && x + filled_width - 1 < ips200_width_max && filled_width > 1)
            ips200_draw_line(x + 1, y + i, x + filled_width - 1, y + i, color);
    }
    
    // ���ʣ�ಿ�֣�������ȼ����ˣ�
    for (i = 1; i < height && i < 19; i++)  // �������߶�
    {
        // ȷ����������ʱ�����갲ȫ
        if (x + filled_width < ips200_width_max && x + width - 1 < ips200_width_max && filled_width < width - 1)
            ips200_draw_line(x + filled_width, y + i, x + width - 1, y + i, RGB565_BLACK);
    }
    
    // ��ʾ���Ȱٷֱȣ�ȷ���ı���ʾ����Ļ�ڣ�
    if (x + width + 30 < ips200_width_max && y + height/2 < ips200_height_max)
    {
        ips200_show_string(x + width + 10, y + height/2 - 8, "%");
        ips200_show_uint(x + width + 5, y + height/2 - 8, progress, 3);
    }
}

// ���˵����Ӳ˵�
Menu mainMenu;

// �˳��˵�ϵͳ�Ĺ��ܺ���
void exit_menu_system(void)
{
    uint8 progress = 0;      // ����ֵ��0-100��
    char status_text[20];    // ״̬�ı�
    uint8 i;                 // ѭ������
    uint8 selection = 0;     // ѡ���0-�����ã�1-����
    
    // ѯ���Ƿ���Send_state
    ips200_clear();
    ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
    ips200_show_string(20, 20, "OPEN STORAGE?");
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    
    // ��ʾѡ��
    ips200_show_string(40, 60, "0. CLOSE");
    ips200_show_string(40, 80, "1. OPEN");
    
    // ��ʾ��ǰѡ��Ĭ��Ϊ��ǰ��Send_stateֵ��
    selection = Send_state ? 1 : 0;
    ips200_set_color(RGB565_BLUE, RGB565_BLACK);
    ips200_show_string(20, selection ? 80 : 60, ">");
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    key_flag = 0;
    // �ȴ��û�ѡ��
    while (1)
    {
        if (key_flag != 0)
        {
            switch (key_flag)
            {
                case Button_1:  // ��
                case Button_2:  // ��
                    // �����һ��ѡ����
                    ips200_set_color(RGB565_BLACK, RGB565_BLACK);
                    ips200_show_string(20, selection ? 80 : 60, ">");
                    
                    // �л�ѡ��
                    selection = selection ? 0 : 1;
                    
                    // ��ʾ�µ�ѡ����
                    ips200_set_color(RGB565_BLUE, RGB565_BLACK);
                    ips200_show_string(20, selection ? 80 : 60, ">");
                    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
                    break;
                    
                case Button_3:  // ȷ��
                    // ����Send_state��ֵ
                    Send_state = selection;
                    
                    // ��ʾȷ����Ϣ
                    ips200_set_color(RGB565_GREEN, RGB565_BLACK);
                    ips200_show_string(20, 120, selection ? "STORAGE OPEN" : "STORAGE CLOSE");
                    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
                    system_delay_ms(1000);  // ��ʾ1��
                    
                    // �˳�ѡ��ѭ��
                    key_flag = 0;
                    goto exit_selection;
                    
                default:
                    break;
            }
            
            // ���ð�����־
            key_flag = 0;
        }
        
        // ������ʱ
        system_delay_ms(10);
    }
    
exit_selection:
{   // ��Ӵ���鿪ʼ
    // ��ȡ��Ļ�ߴ�
    uint16 screen_width = ips200_width_max;
    uint16 screen_height = ips200_height_max;
    
    // ���������� - ������Ļ�ߴ����
    uint16 bar_x = 20;       // ������x����
    uint16 bar_y = screen_height / 2;  // ������y����
    uint16 bar_width = screen_width - 80;  // �������ܿ��
    uint16 bar_height = 20;  // �������߶�
    
    // ȷ�������ڰ�ȫ��Χ��
    if (bar_width > screen_width - 40)
        bar_width = screen_width - 40;
    
    if (bar_y + 50 > screen_height)
        bar_y = screen_height - 80;
    if(Send_state)
    {
        image_storage_init();
        erase_storage_block();
    }
    // ��ʾ�˳���Ϣ
    ips200_clear();
    ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
    ips200_show_string(20, 20, "Exiting menu system...");
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);

    // ������˵��
    ips200_show_string(20, bar_y - 20, "Progress:");

    // ����������
    while (progress <= 100)
    {
        // ���ݽ�����ʾ��ͬ״̬�ı�
        if (progress < 30)
            strcpy(status_text, "Saving settings...");
        else if (progress < 60)
            strcpy(status_text, "Cleaning up...");
        else if (progress < 90)
            strcpy(status_text, "Finalizing...");
        else
            strcpy(status_text, "Complete!");

        // �����һ��״̬�ı�����
        for (i = 0; i < 20 && bar_y + bar_height + 10 + i < screen_height; i++)
        {
            if (20 + 160 < screen_width)
                ips200_draw_line(20, bar_y + bar_height + 10 + i, 20 + 160, bar_y + bar_height + 10 + i, RGB565_BLACK);
        }
        
        // ��ʾ��ǰ״̬�ı�
        if (bar_y + bar_height + 10 < screen_height - 20)
            ips200_show_string(20, bar_y + bar_height + 10, status_text);

        // ���ƽ�����
        draw_progress_bar(bar_x, bar_y, bar_width, bar_height, progress);
        
        // ���ӽ���
        progress += 2;
        
        // ��ʱ�����ƶ����ٶ�
        system_delay_ms(10);
    }

    // ��ʾ�����Ϣ
    ips200_set_color(RGB565_GREEN, RGB565_BLACK);
    if (bar_y + bar_height + 40 < screen_height - 10)
        ips200_show_string(20, bar_y + bar_height + 40, "Exit completed!");
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    system_delay_ms(1000);

    // �����˳���־���⽫ʹ�˵�ϵͳ����һ��ѭ�����ʱ�˳�
    g_menu_exit_flag = 1;

    // ģ�ⰴ�·��ذ�ť��ȷ��menu_execute_function��ѭ�������˳�
    // ˫�ر��ϣ���ʹ����ģ��ʧ�ܣ��˳���־Ҳ��ʹϵͳ����һ��ѭ�����ʱ�˳�
    key_flag = Button_4;
}
}

void video_playback(void)
{
    uint8 key = 0;
    image_read_frame_count();
    uint32 frame_count = get_frame_count();
    // Check if there are stored frames
    if (frame_count == 0)
    {
        ips200_set_color(RGB565_RED, RGB565_BLACK);
        ips200_show_string(20, 80, "No stored images!");
        ips200_set_color(RGB565_WHITE, RGB565_BLACK);
        system_delay_ms(1000);
        return;
    }
    
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    
    // Read the first frame and display
    if (read_compressed_image(go) == STORAGE_OK)
    {
        image_data_analysis();
    }
    
    // Playback control loop
    while (1)
    {
        // Check for key press
        if (key_flag != 0)
        {
            key = key_flag;
            key_flag = 0;
            
            switch (key)
            {
                case Button_1:  // Previous frame
                    read_compressed_image(back);
                    break;
                    
                case Button_2:  // Next frame
                    read_compressed_image(go);
                    
                    break;

                case Button_3:  // Next 10 frames
                    read_compressed_image(go_go);
                    break;
                    
                case Button_4:  // Return
                    read_compressed_image(back_back);
                    break;
                    
                default:
                    break;
            }
        }
        image_data_analysis();
        WITE();
    }
}

// ��ʼ�����Բ˵�
void init_test_menu(void)
{
    // ��ʼ�����˵�
    strncpy(mainMenu.title, "Main Menu", MAX_MENU_NAME_LEN-1);
    mainMenu.itemCount = 0;
    mainMenu.currentSelection = 0;
    mainMenu.parentMenu = NULL;
    
    // �������˵���
    menu_create_item(&mainMenu, "Image Show", Pts_Test, NULL);
    menu_create_item(&mainMenu, "perspective", Get_inverse_matrix, NULL);
    menu_create_item(&mainMenu, "Adjust Parameters", parameter_menu_entry,NULL); 
    menu_create_item(&mainMenu, "Scheme Management", scheme_management_entry, NULL);  // ��ӷ�������
    menu_create_item(&mainMenu, "Exposure Adjust", exposure_time_adjust, NULL);  // ��������������ѡ��
    menu_create_item(&mainMenu, "Video Playback", video_playback, NULL);  // ��ӻطŹ���
    menu_create_item(&mainMenu, "Exit Menu", exit_menu_system, NULL);  // ����˳�ѡ��
    
}

// �˵�ϵͳ���Ժ���
void menu_open(void)
{
    // ��ʼ���˵�ϵͳ
    menu_init();
    
    // ���ò˵��˳���־
    menu_reset_exit_flag();
    
    // ��ʼ�����Բ˵�
    init_test_menu();
    
    // ���õ�ǰ�˵�Ϊ���˵�
    menu_set_current(&mainMenu);
    
    // �˵�����ѭ��
    while (1)
    {
        // ����˵�
        g_currentMenu = menu_process(g_currentMenu);
        if (g_currentMenu == NULL)
        {
            // �˵�ϵͳ���˳�
            break;
        }   
        
        // ��ʱ
        system_delay_ms(10);
    }
    
    // �˵�ϵͳ�˳�������
    ips200_clear();
}
