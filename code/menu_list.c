#include "menu.h"
#include "zf_common_headfile.h"
#include "parameter_flash.h"
#include "PTS_Deal.h"

// 绘制进度条函数
void draw_progress_bar(uint16 x, uint16 y, uint16 width, uint16 height, uint8 progress)
{
    uint16 i;
    uint16 filled_width;
    
    // 屏幕边界检查
    if (x + width >= ips200_width_max)
        width = ips200_width_max - x - 5;  // 留出5像素安全边距
    
    if (y + height >= ips200_height_max)
        height = ips200_height_max - y - 5;  // 留出5像素安全边距
    
    // 确保progress在0-100范围内
    if (progress > 100)
        progress = 100;
    
    // 计算已填充宽度
    filled_width = (uint16)((width * progress) / 100);
    
    // 确保填充宽度在合理范围内
    if (filled_width > width - 2)
        filled_width = width - 2;
    
    // 绘制外框（白色矩形）
    ips200_draw_line(x, y, x + width, y, RGB565_WHITE);
    ips200_draw_line(x, y + height, x + width, y + height, RGB565_WHITE);
    ips200_draw_line(x, y, x, y + height, RGB565_WHITE);
    ips200_draw_line(x + width, y, x + width, y + height, RGB565_WHITE);
    
    // 绘制已完成部分（颜色根据进度变化）
    for (i = 1; i < height && i < 19; i++)  // 限制最大高度
    {
        // 根据进度不同使用不同颜色
        uint16 color;
        if (progress < 30)
            color = RGB565_RED;
        else if (progress < 70)
            color = RGB565_YELLOW;
        else
            color = RGB565_GREEN;
        
        // 确保绘制线条时的坐标安全
        if (x + 1 < ips200_width_max && x + filled_width - 1 < ips200_width_max && filled_width > 1)
            ips200_draw_line(x + 1, y + i, x + filled_width - 1, y + i, color);
    }
    
    // 清除剩余部分（如果进度减少了）
    for (i = 1; i < height && i < 19; i++)  // 限制最大高度
    {
        // 确保绘制线条时的坐标安全
        if (x + filled_width < ips200_width_max && x + width - 1 < ips200_width_max && filled_width < width - 1)
            ips200_draw_line(x + filled_width, y + i, x + width - 1, y + i, RGB565_BLACK);
    }
    
    // 显示进度百分比（确保文本显示在屏幕内）
    if (x + width + 30 < ips200_width_max && y + height/2 < ips200_height_max)
    {
        ips200_show_string(x + width + 10, y + height/2 - 8, "%");
        ips200_show_uint(x + width + 5, y + height/2 - 8, progress, 3);
    }
}

// 主菜单和子菜单
Menu mainMenu;

// 退出菜单系统的功能函数
void exit_menu_system(void)
{
    uint8 progress = 0;      // 进度值（0-100）
    char status_text[20];    // 状态文本
    uint8 i;                 // 循环变量
    uint8 selection = 0;     // 选择项，0-不启用，1-启用
    
    // 询问是否开启Send_state
    ips200_clear();
    ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
    ips200_show_string(20, 20, "OPEN STORAGE?");
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    
    // 显示选项
    ips200_show_string(40, 60, "0. CLOSE");
    ips200_show_string(40, 80, "1. OPEN");
    
    // 显示当前选择（默认为当前的Send_state值）
    selection = Send_state ? 1 : 0;
    ips200_set_color(RGB565_BLUE, RGB565_BLACK);
    ips200_show_string(20, selection ? 80 : 60, ">");
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    key_flag = 0;
    // 等待用户选择
    while (1)
    {
        if (key_flag != 0)
        {
            switch (key_flag)
            {
                case Button_1:  // 上
                case Button_2:  // 下
                    // 清除上一个选择标记
                    ips200_set_color(RGB565_BLACK, RGB565_BLACK);
                    ips200_show_string(20, selection ? 80 : 60, ">");
                    
                    // 切换选择
                    selection = selection ? 0 : 1;
                    
                    // 显示新的选择标记
                    ips200_set_color(RGB565_BLUE, RGB565_BLACK);
                    ips200_show_string(20, selection ? 80 : 60, ">");
                    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
                    break;
                    
                case Button_3:  // 确认
                    // 设置Send_state的值
                    Send_state = selection;
                    
                    // 显示确认消息
                    ips200_set_color(RGB565_GREEN, RGB565_BLACK);
                    ips200_show_string(20, 120, selection ? "STORAGE OPEN" : "STORAGE CLOSE");
                    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
                    system_delay_ms(1000);  // 显示1秒
                    
                    // 退出选择循环
                    key_flag = 0;
                    goto exit_selection;
                    
                default:
                    break;
            }
            
            // 重置按键标志
            key_flag = 0;
        }
        
        // 短暂延时
        system_delay_ms(10);
    }
    
exit_selection:
{   // 添加代码块开始
    // 获取屏幕尺寸
    uint16 screen_width = ips200_width_max;
    uint16 screen_height = ips200_height_max;
    
    // 进度条参数 - 根据屏幕尺寸调整
    uint16 bar_x = 20;       // 进度条x坐标
    uint16 bar_y = screen_height / 2;  // 进度条y坐标
    uint16 bar_width = screen_width - 80;  // 进度条总宽度
    uint16 bar_height = 20;  // 进度条高度
    
    // 确保参数在安全范围内
    if (bar_width > screen_width - 40)
        bar_width = screen_width - 40;
    
    if (bar_y + 50 > screen_height)
        bar_y = screen_height - 80;
    if(Send_state)
    {
        image_storage_init();
        erase_storage_block();
    }
    // 显示退出信息
    ips200_clear();
    ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
    ips200_show_string(20, 20, "Exiting menu system...");
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);

    // 进度条说明
    ips200_show_string(20, bar_y - 20, "Progress:");

    // 进度条动画
    while (progress <= 100)
    {
        // 根据进度显示不同状态文本
        if (progress < 30)
            strcpy(status_text, "Saving settings...");
        else if (progress < 60)
            strcpy(status_text, "Cleaning up...");
        else if (progress < 90)
            strcpy(status_text, "Finalizing...");
        else
            strcpy(status_text, "Complete!");

        // 清除上一次状态文本区域
        for (i = 0; i < 20 && bar_y + bar_height + 10 + i < screen_height; i++)
        {
            if (20 + 160 < screen_width)
                ips200_draw_line(20, bar_y + bar_height + 10 + i, 20 + 160, bar_y + bar_height + 10 + i, RGB565_BLACK);
        }
        
        // 显示当前状态文本
        if (bar_y + bar_height + 10 < screen_height - 20)
            ips200_show_string(20, bar_y + bar_height + 10, status_text);

        // 绘制进度条
        draw_progress_bar(bar_x, bar_y, bar_width, bar_height, progress);
        
        // 增加进度
        progress += 2;
        
        // 延时，控制动画速度
        system_delay_ms(10);
    }

    // 显示完成信息
    ips200_set_color(RGB565_GREEN, RGB565_BLACK);
    if (bar_y + bar_height + 40 < screen_height - 10)
        ips200_show_string(20, bar_y + bar_height + 40, "Exit completed!");
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    system_delay_ms(1000);

    // 设置退出标志，这将使菜单系统在下一次循环检查时退出
    g_menu_exit_flag = 1;

    // 模拟按下返回按钮，确保menu_execute_function的循环立即退出
    // 双重保障：即使按键模拟失败，退出标志也会使系统在下一次循环检查时退出
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

// 初始化测试菜单
void init_test_menu(void)
{
    // 初始化主菜单
    strncpy(mainMenu.title, "Main Menu", MAX_MENU_NAME_LEN-1);
    mainMenu.itemCount = 0;
    mainMenu.currentSelection = 0;
    mainMenu.parentMenu = NULL;
    
    // 创建主菜单项
    menu_create_item(&mainMenu, "Image Show", Pts_Test, NULL);
    menu_create_item(&mainMenu, "perspective", Get_inverse_matrix, NULL);
    menu_create_item(&mainMenu, "Adjust Parameters", parameter_menu_entry,NULL); 
    menu_create_item(&mainMenu, "Scheme Management", scheme_management_entry, NULL);  // 添加方案管理
    menu_create_item(&mainMenu, "Exposure Adjust", exposure_time_adjust, NULL);  // 添加相机参数调整选项
    menu_create_item(&mainMenu, "Video Playback", video_playback, NULL);  // 添加回放功能
    menu_create_item(&mainMenu, "Exit Menu", exit_menu_system, NULL);  // 添加退出选项
    
}

// 菜单系统测试函数
void menu_open(void)
{
    // 初始化菜单系统
    menu_init();
    
    // 重置菜单退出标志
    menu_reset_exit_flag();
    
    // 初始化测试菜单
    init_test_menu();
    
    // 设置当前菜单为主菜单
    menu_set_current(&mainMenu);
    
    // 菜单处理循环
    while (1)
    {
        // 处理菜单
        g_currentMenu = menu_process(g_currentMenu);
        if (g_currentMenu == NULL)
        {
            // 菜单系统已退出
            break;
        }   
        
        // 延时
        system_delay_ms(10);
    }
    
    // 菜单系统退出后清屏
    ips200_clear();
}
