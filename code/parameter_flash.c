#include "parameter_flash.h"
#include "zf_device_ips200.h"
#include "key.h"
#include <string.h>

//===================================================自定义数值格式化===================================================

// 截断浮点数格式化，不四舍五入
void format_float_truncate(char* buffer, float value, uint8 width, uint8 precision)
{
    uint8 is_negative = 0;
    if(value < 0.0f)
    {
        is_negative = 1;
        value = -value;
    }
    uint32 int_part = (uint32)value;
    float frac_part = value - (float)int_part;
    uint32 frac_digits = 0;
    if(precision > 0)
    {
        uint32 multiplier = 1;
        for(uint8 i = 0; i < precision; i++)
            multiplier *= 10;
        frac_digits = (uint32)(frac_part * multiplier);
    }
    char temp[32];
    if(precision == 0)
    {
        if(is_negative)
            sprintf(temp, "-%lu", (unsigned long)int_part);
        else
            sprintf(temp, "%lu", (unsigned long)int_part);
    }
    else if(precision == 1)
    {
        if(is_negative)
            sprintf(temp, "-%lu.%01lu", (unsigned long)int_part, frac_digits);
        else
            sprintf(temp, "%lu.%01lu", (unsigned long)int_part, frac_digits);
    }
    else if(precision == 2)
    {
        if(is_negative)
            sprintf(temp, "-%lu.%02lu", (unsigned long)int_part, frac_digits);
        else
            sprintf(temp, "%lu.%02lu", (unsigned long)int_part, frac_digits);
    }
    else if(precision == 3)
    {
        if(is_negative)
            sprintf(temp, "-%lu.%03lu", (unsigned long)int_part, frac_digits);
        else
            sprintf(temp, "%lu.%03lu", (unsigned long)int_part, frac_digits);
    }
    else
    {
        char fmt[20];
        if(is_negative)
        {
            sprintf(fmt, "-%%lu.%%0%dlu", precision);
            sprintf(temp, fmt, (unsigned long)int_part, frac_digits);
        }
        else
        {
            sprintf(fmt, "%%lu.%%0%dlu", precision);
            sprintf(temp, fmt, (unsigned long)int_part, frac_digits);
        }
    }
    sprintf(buffer, "%*s ", width, temp);
}

//===================================================参数存储和管理===================================================

// 参数存储数组
float parameter[MAX_PARAMETERS];

// 参数配置表
ParameterConfig param_config[] = {
    {"Speed LADRC:-Wc",    0,   &speed_ladrc.wc,  0.0f,    0.0f,    1000.0f,    1.0f},
    {"     -W0",           1,   &speed_ladrc.w0,  0.0f,   0.0f,    1000.0f,   4.0f},
    {"     -b",            2,   &speed_ladrc.b,   0.0f,    0.0f,    10.0f,     0.1f},
    {"     -Limit",        3,   &speed_ladrc.limit, 0.0f, 0.0f,  10000.0f, 100.0f},
    {"MID PID:-P",         4,   &MID_PID.KP,      0.0f,    0.0f,    100.0f,    1.0f},
    {"   -D",              5,   &MID_PID.KD,      0.0f,    0.0f,    100.0f,     0.1f},
    {"   -Limit",          6,   &MID_PID.limit,   0.0f, 0.0f,    1000.0f, 10.0f},
    {"Gyroz LADRC:-Wc",    7,   &gyroz_ladrc.wc,  0.0f,   0.0f,    1000.0f,   1.0f},
    {"     -W0",           8,   &gyroz_ladrc.w0,  0.0f,   0.0f,    1000.0f,   4.0f},
    {"     -b",            9,   &gyroz_ladrc.b,   0.0f,    0.0f,    10.0f,    0.10f},
    {"     -Limit",        10,  &gyroz_ladrc.limit, 0.0f, 0.0f,  10000.0f, 100.0f},
    {"Speed",              11,  &speed,           0.0f,    0.0f,    10.0f,    0.25f},
    {"Add Speed",          13,  &add_speed,       0.0f,    0.0f,    10.0f,    0.05},
    {"FOC_PWM",            12,  &FOC_PWM,         0.0f,    0.0f,    1000.0f,    50.0f},
    
};

uint8 parameter_count = sizeof(param_config) / sizeof(ParameterConfig);

// 静态变量
static uint8 s_curr_param = 0;
static ParameterState s_param_state = PARAM_STATE_VIEW;
static uint8 s_param_initialized = 0;
static float s_backup_params[MAX_PARAMETERS];

static uint8 s_storage_curr_item = 0;
static StorageMenuState s_storage_state = STORAGE_MENU_MAIN;
static uint8 s_storage_initialized = 0;

// 槽位名称
static char slot_names[STORAGE_SLOT_COUNT][16] = {
    "Default", "Config 1", "Config 2", "Config 3", "Config 4"
};

// 添加参数值缓存
static float s_last_param_values[MAX_PARAMETERS];  // 上一次显示的参数值
static uint8 s_param_values_initialized = 0;
static uint8 s_update_pending = 0;  // 是否有待更新的显示
static uint32 s_last_update_time = 0;  // 上次更新时间
#define UPDATE_INTERVAL_MS 50  // 更新间隔（毫秒）

// 添加参数修改标记
static uint8 s_params_modified = 0;  // 是否有参数被修改过

//===================================================基础参数操作===================================================

void Parameter_flash_load(void)
{
    Parameter_flash_load_slot(STORAGE_SLOT_DEFAULT);
}

void Parameter_flash_load_slot(uint8 slot)
{
    if(slot >= STORAGE_SLOT_COUNT) return;
    
    flash_read_page_to_buffer(0, PARAMETER_INDEX + slot);

    uint8 valid_data = 1;
    for(uint8 i = 0; i < parameter_count; i++)
    {
        uint8 idx = param_config[i].index;
        float value = flash_union_buffer[idx].float_type;
        if(value != value || value > 1e10 || value < -1e10)
        {
            valid_data = 0;
            break;
        }
        parameter[idx] = value;
    }

    if(!valid_data)
    {
        Parameter_reset_to_default();
    }
    else
    {
        // 应用参数到目标变量
        for(uint8 i = 0; i < parameter_count; i++)
        {
            if(param_config[i].target_var != NULL)
                *(param_config[i].target_var) = parameter[param_config[i].index];
        }
    }
}

void Parameter_flash_save(void)
{
    Parameter_flash_save_slot(STORAGE_SLOT_DEFAULT);
}

void Parameter_flash_save_slot(uint8 slot)
{
    if(slot >= STORAGE_SLOT_COUNT) return;
    
    // 首先将当前变量值同步到parameter数组
    for(uint8 i = 0; i < parameter_count; i++)
    {
        if(param_config[i].target_var != NULL)
        {
            parameter[param_config[i].index] = *(param_config[i].target_var);
        }
    }
    
    flash_erase_page(0, PARAMETER_INDEX + slot);

    for(uint8 i = 0; i < parameter_count; i++)
    {
        uint8 idx = param_config[i].index;
        flash_union_buffer[idx].float_type = parameter[idx];
    }

    flash_write_page_from_buffer(0, PARAMETER_INDEX + slot);
}

void Parameter_reset_to_default(void)
{
    for(uint8 i = 0; i < parameter_count; i++)
    {
        uint8 idx = param_config[i].index;
        parameter[idx] = param_config[i].default_value;
        if(param_config[i].target_var != NULL)
            *(param_config[i].target_var) = parameter[idx];
    }
}

uint8 storage_slot_exists(uint8 slot)
{
    if(slot >= STORAGE_SLOT_COUNT) return 0;
    
    flash_read_page_to_buffer(0, PARAMETER_INDEX + slot);
    
    for(uint8 i = 0; i < 3 && i < parameter_count; i++)
    {
        uint8 idx = param_config[i].index;
        float value = flash_union_buffer[idx].float_type;
        if(value == value && value > -1e10 && value < 1e10)
            return 1;
    }
    return 0;
}

//===================================================方案管理功能===================================================

// 静态变量
static ParameterScheme schemes[MAX_SCHEMES];
static uint8 s_scheme_initialized = 0;
static uint8 s_scheme_curr_item = 0;
static SchemeMenuState s_scheme_state = SCHEME_MENU_MAIN;
static uint8 s_scheme_list_loaded = 0;

// 静态变量 for draw_scheme_menu optimization
static uint8 s_dsm_last_curr_item = 0xFF;
static SchemeMenuState s_dsm_last_state = (SchemeMenuState)0xFF; // Use an invalid enum cast
static uint8 s_dsm_last_valid_count = 0xFF;
static uint8 s_dsm_needs_full_redraw = 1;

// 方案菜单布局参数
#define SCHEME_TITLE_Y       10      // 标题Y坐标
#define SCHEME_ITEM_START_Y  40      // 菜单项起始Y坐标
#define SCHEME_ITEM_HEIGHT   20      // 菜单项高度
#define SCHEME_LEFT_MARGIN   15      // 左边距
#define SCHEME_LINES_PER_PAGE 10     // 每页显示的方案数

// 加载方案列表
void load_scheme_list(void)
{
    if(s_scheme_list_loaded) return;
    
    uint32 total_data_to_read = sizeof(schemes);

    if (total_data_to_read > sizeof(flash_union_buffer)) {
        for(uint8 i = 0; i < MAX_SCHEMES; i++) {
            schemes[i].valid = 0;
        }
        s_scheme_list_loaded = 1; // 标记为已加载（虽然是空/错误状态）
        return;
    }

    flash_read_page_to_buffer(0, SCHEME_FLASH_BASE_INDEX);
    
    memcpy(schemes, flash_union_buffer, total_data_to_read);
    
    s_scheme_list_loaded = 1;
}

// 保存方案列表
void save_scheme_list(void)
{
    uint32 total_data_to_write = sizeof(schemes);

    if (total_data_to_write > sizeof(flash_union_buffer)) {
        return; // 无法保存，因为数据太大
    }

    memset(flash_union_buffer, 0xFF, sizeof(flash_union_buffer));
    
    memcpy(flash_union_buffer, schemes, total_data_to_write);
    
    flash_erase_page(0, SCHEME_FLASH_BASE_INDEX);
    
    flash_write_page_from_buffer(0, SCHEME_FLASH_BASE_INDEX);
}

// 获取方案数量
uint8 get_scheme_count(void)
{
    uint8 count = 0;
    for(uint8 i = 0; i < MAX_SCHEMES; i++)
    {
        if(schemes[i].valid) count++;
    }
    return count;
}

// 生成方案名称
void generate_scheme_name(char* name_buffer, float speed_value)
{
    // 查找同速度值的方案数量
    uint8 version_count = 0;
    for(uint8 i = 0; i < MAX_SCHEMES; i++)
    {
        if(schemes[i].valid && schemes[i].speed_value == speed_value)
        {
            version_count++;
        }
    }
    
    if(version_count == 0)
    {
        // 第一个该速度的方案，直接使用速度值
        format_float_truncate(name_buffer, speed_value, 8, 2);
        // 去掉尾随空格
        uint8 len = strlen(name_buffer);
        while(len > 0 && name_buffer[len-1] == ' ')
        {
            name_buffer[len-1] = '\0';
            len--;
        }
    }
    else
    {
        // 已存在该速度的方案，添加版本号
        char base_name[10];
        format_float_truncate(base_name, speed_value, 8, 2);
        // 去掉尾随空格
        uint8 len = strlen(base_name);
        while(len > 0 && base_name[len-1] == ' ')
        {
            base_name[len-1] = '\0';
            len--;
        }
        sprintf(name_buffer, "%s.%d", base_name, version_count + 1);
    }
}

// 保存当前参数为方案
void save_current_parameters_as_scheme(void)
{
    // 强制重新加载scheme列表，确保数据是最新的
    s_scheme_list_loaded = 0;
    load_scheme_list();
    
    // 首先将当前变量值同步到parameter数组
    for(uint8 i = 0; i < parameter_count; i++)
    {
        if(param_config[i].target_var != NULL)
        {
            parameter[param_config[i].index] = *(param_config[i].target_var);
        }
    }
    
    // 查找空闲槽位
    uint8 free_slot = MAX_SCHEMES;
    for(uint8 i = 0; i < MAX_SCHEMES; i++)
    {
        if(!schemes[i].valid)
        {
            free_slot = i;
            break;
        }
    }
    
    if(free_slot >= MAX_SCHEMES)
    {
        ips200_clear();
        ips200_set_color(RGB565_RED, RGB565_BLACK);
        ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2 - 10, "Schemes Full!");
        ips200_set_color(RGB565_WHITE, RGB565_BLACK);
        system_delay_ms(1500);
        return;
    }
    
    // 获取速度值（参数11对应speed）
    float speed_value = parameter[param_config[11].index];  // Speed参数的索引是11
    
    // 生成方案名称
    generate_scheme_name(schemes[free_slot].name, speed_value);
    
    // 保存方案数据
    schemes[free_slot].valid = 1;
    schemes[free_slot].speed_value = speed_value;
    for(uint8 i = 0; i < parameter_count; i++)
    {
        schemes[free_slot].parameters[param_config[i].index] = parameter[param_config[i].index];
    }
    
    // 保存到flash
    save_scheme_list();
    
    // 显示保存成功信息
    ips200_clear();
    ips200_set_color(RGB565_GREEN, RGB565_BLACK);
    char msg[32];
    sprintf(msg, "Scheme %s Saved", schemes[free_slot].name);
    ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2 - 10, msg);
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    system_delay_ms(1500);
}

// 删除方案
void delete_scheme(uint8 index)
{
    if(index >= MAX_SCHEMES || !schemes[index].valid) return;
    
    schemes[index].valid = 0;
    save_scheme_list();
    
    ips200_clear();
    ips200_set_color(RGB565_GREEN, RGB565_BLACK);
    ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2 - 10, "Scheme Deleted");
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    system_delay_ms(1000);
}

// 加载方案参数
void load_scheme_parameters(uint8 index)
{
    // 确保scheme列表是最新的
    s_scheme_list_loaded = 0;
    load_scheme_list();
    
    if(index >= MAX_SCHEMES || !schemes[index].valid) return;
    
    // 确认加载
    ips200_clear();
    ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
    ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2 - 20, "Params will be overwritten!");
    ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2, "Confirm load? (3=Yes/4=No)");
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    
    // 等待用户确认
    while(1)
    {
        if(key_flag)
        {
            uint8 key = key_flag;
            key_flag = 0;
            
            if(key == Button_3)  // 确认
            {
                // 加载参数
                for(uint8 i = 0; i < parameter_count; i++)
                {
                    uint8 idx = param_config[i].index;
                    parameter[idx] = schemes[index].parameters[idx];
                    if(param_config[i].target_var != NULL)
                        *(param_config[i].target_var) = parameter[idx];
                }
                
                // 保存到默认槽位
                Parameter_flash_save();
                
                ips200_clear();
                ips200_set_color(RGB565_GREEN, RGB565_BLACK);
                ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2 - 10, "Params Applied");
                ips200_set_color(RGB565_WHITE, RGB565_BLACK);
                system_delay_ms(1000);
                break;
            }
            else if(key == Button_4)  // 取消
            {
                break;
            }
        }
        system_delay_ms(10);
    }
}

// 绘制方案管理菜单
void draw_scheme_menu(uint8 current_item, SchemeMenuState state)
{
    uint8 local_needs_full_redraw = s_dsm_needs_full_redraw;
    uint8 current_valid_count = 0;
    uint8 valid_schemes[MAX_SCHEMES]; // Temporary for this function call, populated if in list state

    if (state == SCHEME_MENU_DELETE || state == SCHEME_MENU_LOAD || state == SCHEME_MENU_VIEW) {
        for(uint8 i = 0; i < MAX_SCHEMES; i++) {
            if(schemes[i].valid) {
                valid_schemes[current_valid_count++] = i;
            }
        }
        if (current_valid_count != s_dsm_last_valid_count) {
            local_needs_full_redraw = 1;
        }
    }

    if (state != s_dsm_last_state) {
        local_needs_full_redraw = 1;
    }

    if (local_needs_full_redraw) {
        s_dsm_needs_full_redraw = 0; // Reset global flag after handling
        ips200_clear();

        // Draw Title
        ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
        if(state == SCHEME_MENU_MAIN) {
            ips200_show_string(SCHEME_LEFT_MARGIN, SCHEME_TITLE_Y, "Scheme Management");
        } else if (state == SCHEME_MENU_DELETE) {
            ips200_show_string(SCHEME_LEFT_MARGIN, SCHEME_TITLE_Y, "Select Scheme to Delete");
        } else if (state == SCHEME_MENU_LOAD) {
            ips200_show_string(SCHEME_LEFT_MARGIN, SCHEME_TITLE_Y, "Select Scheme to Load");
        } else if (state == SCHEME_MENU_VIEW) {
            ips200_show_string(SCHEME_LEFT_MARGIN, SCHEME_TITLE_Y, "Select Scheme to View");
        }

        // Draw Items (this is the main body)
        if(state == SCHEME_MENU_MAIN) {
            const char* items[] = {"Save Current Params", "Delete Scheme", "Load Scheme", "View Scheme", "Exit"};
            for(uint8 i = 0; i < 5; i++) {
                uint16 y_pos = SCHEME_ITEM_START_Y + i * 25;
                 if(y_pos + 20 > ips200_height_max - 50) break;
                ips200_set_color(i == current_item ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
                ips200_show_string(SCHEME_LEFT_MARGIN, y_pos, (char*)items[i]);
            }
        } else { // SCHEME_MENU_DELETE or SCHEME_MENU_LOAD or SCHEME_MENU_VIEW
            if(current_valid_count == 0) {
                ips200_set_color(RGB565_GRAY, RGB565_BLACK);
                ips200_show_string(SCHEME_LEFT_MARGIN, SCHEME_ITEM_START_Y, "No schemes available");
            } else {
                uint8 display_count = (current_valid_count < SCHEME_LINES_PER_PAGE) ? current_valid_count : SCHEME_LINES_PER_PAGE;
                for(uint8 i = 0; i < display_count; i++) {
                    uint16 y_pos = SCHEME_ITEM_START_Y + i * SCHEME_ITEM_HEIGHT;
                    if(y_pos + SCHEME_ITEM_HEIGHT > ips200_height_max - 50) break;
                    
                    ips200_set_color(i == current_item ? RGB565_GREEN : RGB565_CYAN, RGB565_BLACK);
                    char display_str[32];
                    // current_item is the index in the displayed list, valid_schemes[i] gives the actual scheme index
                    sprintf(display_str, "%d. %s", i + 1, schemes[valid_schemes[i]].name);
                    ips200_show_string(SCHEME_LEFT_MARGIN, y_pos, display_str);
                }
            }
        }

        // Draw Hints (always drawn on full redraw)
        uint16 hint_y = ips200_height_max - 35;
        ips200_set_color(RGB565_GRAY, RGB565_BLACK);
        if(state == SCHEME_MENU_MAIN)
            ips200_show_string(SCHEME_LEFT_MARGIN, hint_y, "1:Up 2:Down 3:Select 4:Back");
        else if(state == SCHEME_MENU_DELETE)
            ips200_show_string(SCHEME_LEFT_MARGIN, hint_y, "1:Up 2:Down 3:Delete 4:Back");
        else if(state == SCHEME_MENU_LOAD)
            ips200_show_string(SCHEME_LEFT_MARGIN, hint_y, "1:Up 2:Down 3:Load 4:Back");
        else // SCHEME_MENU_VIEW
            ips200_show_string(SCHEME_LEFT_MARGIN, hint_y, "1:Up 2:Down 3:View 4:Back");

    } else if (current_item != s_dsm_last_curr_item) { // Partial redraw: only current_item changed
        if(state == SCHEME_MENU_MAIN) {
            const char* items[] = {"Save Current Params", "Delete Scheme", "Load Scheme", "View Scheme", "Exit"};
            uint16 y_pos_old = SCHEME_ITEM_START_Y + s_dsm_last_curr_item * 25;
            uint16 y_pos_new = SCHEME_ITEM_START_Y + current_item * 25;

            // Redraw old item with normal color
            if (s_dsm_last_curr_item < 5) { // Check bounds
                 ips200_set_color(RGB565_WHITE, RGB565_BLACK);
                 ips200_show_string(SCHEME_LEFT_MARGIN, y_pos_old, (char*)items[s_dsm_last_curr_item]);
            }

            // Redraw new item with selected color
            if (current_item < 5) { // Check bounds
                ips200_set_color(RGB565_GREEN, RGB565_BLACK);
                ips200_show_string(SCHEME_LEFT_MARGIN, y_pos_new, (char*)items[current_item]);
            }
        } else { // SCHEME_MENU_DELETE or SCHEME_MENU_LOAD or SCHEME_MENU_VIEW
            // current_valid_count and valid_schemes are populated from the beginning of the function
            // s_dsm_last_curr_item and current_item are indices for the *displayed* list
            uint16 y_pos_old = SCHEME_ITEM_START_Y + s_dsm_last_curr_item * SCHEME_ITEM_HEIGHT;
            uint16 y_pos_new = SCHEME_ITEM_START_Y + current_item * SCHEME_ITEM_HEIGHT;

            // Redraw old item with normal color
            if (s_dsm_last_curr_item < current_valid_count) { // Check bounds for displayed list
                 ips200_set_color(RGB565_CYAN, RGB565_BLACK);
                 char display_str_old[32];
                 sprintf(display_str_old, "%d. %s", s_dsm_last_curr_item + 1, schemes[valid_schemes[s_dsm_last_curr_item]].name);
                 ips200_show_string(SCHEME_LEFT_MARGIN, y_pos_old, display_str_old);
            }

            // Redraw new item with selected color
            if (current_item < current_valid_count) { // Check bounds for displayed list
                ips200_set_color(RGB565_GREEN, RGB565_BLACK);
                char display_str_new[32];
                sprintf(display_str_new, "%d. %s", current_item + 1, schemes[valid_schemes[current_item]].name);
                ips200_show_string(SCHEME_LEFT_MARGIN, y_pos_new, display_str_new);
            }
        }
    }
    // If no redraw was needed (local_needs_full_redraw is false AND current_item == s_dsm_last_curr_item), do nothing.

    ips200_set_color(RGB565_WHITE, RGB565_BLACK); // Reset default color just in case

    s_dsm_last_curr_item = current_item;
    s_dsm_last_state = state;
    if (state == SCHEME_MENU_DELETE || state == SCHEME_MENU_LOAD || state == SCHEME_MENU_VIEW) {
        s_dsm_last_valid_count = current_valid_count;
    }
}

// 方案管理页面
uint8 scheme_management_page(void)
{
    if(!s_scheme_initialized)
    {
        load_scheme_list(); // Ensure list is fresh on first entry or re-entry
        s_scheme_curr_item = 0;
        s_scheme_state = SCHEME_MENU_MAIN;
        s_scheme_initialized = 1;
        s_dsm_needs_full_redraw = 1; // Force full redraw for the first time or re-entry
        draw_scheme_menu(s_scheme_curr_item, s_scheme_state);
        return 0;
    }
    
    if(key_flag)
    {
        uint8 key = key_flag;
        key_flag = 0;
        
        if(s_scheme_state == SCHEME_MENU_MAIN)
        {
            switch(key)
            {
                case Button_1: // 上一项
                    s_scheme_curr_item = (s_scheme_curr_item > 0) ? (s_scheme_curr_item - 1) : 4;
                    draw_scheme_menu(s_scheme_curr_item, s_scheme_state);
                    break;
                    
                case Button_2: // 下一项
                    s_scheme_curr_item = (s_scheme_curr_item < 4) ? (s_scheme_curr_item + 1) : 0;
                    draw_scheme_menu(s_scheme_curr_item, s_scheme_state);
                    break;
                    
                case Button_3: // 选择
                    switch(s_scheme_curr_item)
                    {
                        case 0: // 保存当前参数
                            save_current_parameters_as_scheme();
                            s_scheme_list_loaded = 0;  // 强制重新加载
                            load_scheme_list();  // 重新加载列表, list content might change
                            s_dsm_needs_full_redraw = 1;
                            // s_scheme_state remains SCHEME_MENU_MAIN
                            // s_scheme_curr_item could be reset or stay, draw_scheme_menu will handle
                            break;
                        case 1: // 删除方案
                            if(get_scheme_count() > 0)
                            {
                                s_scheme_state = SCHEME_MENU_DELETE;
                                s_scheme_curr_item = 0; // Reset for the new list
                                s_dsm_needs_full_redraw = 1; // State change
                            }
                            else
                            {
                                ips200_clear();
                                ips200_set_color(RGB565_GRAY, RGB565_BLACK);
                                ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2 - 10, "No schemes to delete");
                                ips200_set_color(RGB565_WHITE, RGB565_BLACK);
                                system_delay_ms(1000);
                                s_dsm_needs_full_redraw = 1; // To redraw main menu after message
                            }
                            break;
                        case 2: // 加载方案
                            if(get_scheme_count() > 0)
                            {
                                s_scheme_state = SCHEME_MENU_LOAD;
                                s_scheme_curr_item = 0; // Reset for the new list
                                s_dsm_needs_full_redraw = 1; // State change
                            }
                            else
                            {
                                ips200_clear();
                                ips200_set_color(RGB565_GRAY, RGB565_BLACK);
                                ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2 - 10, "No schemes to load");
                                ips200_set_color(RGB565_WHITE, RGB565_BLACK);
                                system_delay_ms(1000);
                                s_dsm_needs_full_redraw = 1; // To redraw main menu after message
                            }
                            break;
                        case 3: // 查看方案
                            if(get_scheme_count() > 0)
                            {
                                s_scheme_state = SCHEME_MENU_VIEW;
                                s_scheme_curr_item = 0; // Reset for the new list
                                s_dsm_needs_full_redraw = 1; // State change
                            }
                            else
                            {
                                ips200_clear();
                                ips200_set_color(RGB565_GRAY, RGB565_BLACK);
                                ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2 - 10, "No schemes to view");
                                ips200_set_color(RGB565_WHITE, RGB565_BLACK);
                                system_delay_ms(1000);
                                s_dsm_needs_full_redraw = 1; // To redraw main menu after message
                            }
                            break;
                        case 4: // 退出
                            s_scheme_initialized = 0; // This will trigger full redraw on next entry
                            return 1;
                    }
                    draw_scheme_menu(s_scheme_curr_item, s_scheme_state); // Redraw after action or state change
                    break;
                    
                case Button_4: // 返回 (from main menu)
                    s_scheme_initialized = 0; // This will trigger full redraw on next entry
                    return 1;
            }
        }
        else // SCHEME_MENU_DELETE or SCHEME_MENU_LOAD or SCHEME_MENU_VIEW (scheme selection state)
        {
            // Get current valid_count for navigation logic
            uint8 current_valid_count = 0;
            for(uint8 i = 0; i < MAX_SCHEMES; i++) {
                if(schemes[i].valid) current_valid_count++;
            }
            
            switch(key)
            {
                case Button_1: // 上一个方案
                    if(current_valid_count > 0)
                    {
                        s_scheme_curr_item = (s_scheme_curr_item > 0) ? (s_scheme_curr_item - 1) : (current_valid_count - 1);
                        draw_scheme_menu(s_scheme_curr_item, s_scheme_state);
                    }
                    break;
                    
                case Button_2: // 下一个方案
                    if(current_valid_count > 0)
                    {
                        s_scheme_curr_item = (s_scheme_curr_item < current_valid_count - 1) ? (s_scheme_curr_item + 1) : 0;
                        draw_scheme_menu(s_scheme_curr_item, s_scheme_state);
                    }
                    break;
                    
                case Button_3: // 确认操作 (Delete or Load)
                    if(current_valid_count > 0 && s_scheme_curr_item < current_valid_count)
                    {
                        // Need to map s_scheme_curr_item (list index) to actual scheme index
                        uint8 actual_scheme_index = 0;
                        uint8 count = 0;
                        for(uint8 i=0; i<MAX_SCHEMES; ++i) {
                            if(schemes[i].valid) {
                                if(count == s_scheme_curr_item) {
                                    actual_scheme_index = i;
                                    break;
                                }
                                count++;
                            }
                        }

                        if(s_scheme_state == SCHEME_MENU_DELETE)
                        {
                            ips200_clear(); // Clear for confirmation message
                            ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
                            char confirm_msg[32];
                            sprintf(confirm_msg, "Delete %s?", schemes[actual_scheme_index].name);
                            ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2 - 20, confirm_msg);
                            ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2, "(3=Yes/4=No)");
                            ips200_set_color(RGB565_WHITE, RGB565_BLACK);
                            
                            while(1) { // Wait for confirmation
                                if(key_flag) {
                                    uint8 confirm_key = key_flag;
                                    key_flag = 0;
                                    if(confirm_key == Button_3) {
                                        delete_scheme(actual_scheme_index);
                                        load_scheme_list();  // Reload, list content changed
                                        s_scheme_state = SCHEME_MENU_MAIN;
                                        s_scheme_curr_item = 0;
                                        s_dsm_needs_full_redraw = 1;
                                        break;
                                    } else if(confirm_key == Button_4) {
                                        s_dsm_needs_full_redraw = 1; // Need to redraw previous sub-menu
                                        break;
                                    }
                                }
                                system_delay_ms(10);
                            }
                        }
                        else if(s_scheme_state == SCHEME_MENU_LOAD)
                        {
                            load_scheme_parameters(actual_scheme_index); // This function has its own confirm and messages
                            // After load_scheme_parameters, it returns to this flow.
                            s_scheme_state = SCHEME_MENU_MAIN;
                            s_scheme_curr_item = 0;
                            s_dsm_needs_full_redraw = 1;
                        }
                        else if(s_scheme_state == SCHEME_MENU_VIEW)
                        {
                            // 进入方案查看页面
                            uint8 view_exit = 0;
                            while(!view_exit)
                            {
                                view_exit = view_scheme_page(actual_scheme_index);
                                system_delay_ms(10);
                            }
                            // 从查看页面返回后，回到主菜单
                            s_scheme_state = SCHEME_MENU_MAIN;
                            s_scheme_curr_item = 0;
                            s_dsm_needs_full_redraw = 1;
                        }
                    }
                    draw_scheme_menu(s_scheme_curr_item, s_scheme_state); // Redraw after action
                    break;
                    
                case Button_4: // 返回主菜单 (from sub-menu)
                    s_scheme_state = SCHEME_MENU_MAIN;
                    s_scheme_curr_item = 0;
                    s_dsm_needs_full_redraw = 1; // State change
                    draw_scheme_menu(s_scheme_curr_item, s_scheme_state);
                    break;
            }
        }
    }
    
    return 0;
}

// 方案管理入口函数
void scheme_management_entry(void)
{
    s_scheme_initialized = 0;
    uint8 exit_flag = 0;
    key_flag = 0;
    
    while(!exit_flag)
    {
        exit_flag = scheme_management_page();
        system_delay_ms(10);
    }
    
    // 设置返回按键标志，让菜单系统知道需要返回上级菜单
    key_flag = Button_4;
}

//===================================================参数调整页面===================================================

// 添加静态变量来跟踪显示状态
static uint8 s_current_page = 0;  // 当前页码
static uint8 s_last_page = 255;  // 上一次的页码
static uint8 s_last_curr_param = 255;  // 上一次的当前参数
static ParameterState s_last_state = PARAM_STATE_VIEW;  // 上一次的状态

// 布局参数（基于屏幕尺寸动态计算）
#define PARAM_LINE_HEIGHT   18      // 每行参数的高度
#define PARAM_LEFT_MARGIN   10      // 左边距
#define PARAM_VALUE_X_OFFSET 130    // 参数值相对于左边距的偏移
#define PARAM_TITLE_Y       10      // 标题Y坐标
#define PARAM_START_Y       35      // 参数列表起始Y坐标
#define PARAM_LINES_PER_PAGE 12      // 每页显示的参数行数

// 清除指定行的内容
void clear_param_line(uint8 line_index)
{
    uint16 y_pos = PARAM_START_Y + line_index * PARAM_LINE_HEIGHT;
    uint16 line_width = ips200_width_max - PARAM_LEFT_MARGIN - 10;  // 留10像素右边距
    
    // 清除整行（用黑色填充）
    for(uint8 i = 0; i < PARAM_LINE_HEIGHT && (y_pos + i) < ips200_height_max; i++)
    {
        ips200_draw_line(PARAM_LEFT_MARGIN, y_pos + i, PARAM_LEFT_MARGIN + line_width, y_pos + i, RGB565_BLACK);
    }
}

// 绘制单个参数行
void draw_param_line(uint8 param_index, uint8 line_index, uint8 curr_param, ParameterState state)
{
    if(param_index >= parameter_count) return;
    
    uint16 y_pos = PARAM_START_Y + line_index * PARAM_LINE_HEIGHT;
    uint16 value_x = PARAM_LEFT_MARGIN + PARAM_VALUE_X_OFFSET;
    
    // 确保不超出屏幕底部
    if(y_pos + PARAM_LINE_HEIGHT > ips200_height_max) return;
    
    // 设置颜色
    if(param_index == curr_param)
    {
        ips200_set_color(state == PARAM_STATE_EDIT ? RGB565_GREEN : RGB565_BLUE, RGB565_BLACK);
    }
    else
    {
        ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    }
    
    // 显示参数名和值
    ips200_show_string(PARAM_LEFT_MARGIN, y_pos, (char*)param_config[param_index].name);
    
    // 使用自定义格式化函数显示真实数值
    char value_str[16];
    format_float_truncate(value_str, parameter[param_config[param_index].index], 7, 2);
    ips200_show_string(value_x, y_pos, value_str);
}

// 更新操作提示
void update_operation_hint(ParameterState state)
{
    uint16 hint_y = ips200_height_max - 40;  // 提示信息距离底部40像素
    uint16 hint_width = ips200_width_max - PARAM_LEFT_MARGIN * 2;
    
    // 清除提示区域
    for(uint8 i = 0; i < 16 && (hint_y + i) < ips200_height_max; i++)
    {
        ips200_draw_line(PARAM_LEFT_MARGIN, hint_y + i, PARAM_LEFT_MARGIN + hint_width, hint_y + i, RGB565_BLACK);
    }
    
    // 显示新的提示
    ips200_set_color(RGB565_GRAY, RGB565_BLACK);
    if(state == PARAM_STATE_VIEW)
        ips200_show_string(PARAM_LEFT_MARGIN, hint_y, "1:Up 2:Down 3:Edit 4:Menu");
    else
        ips200_show_string(PARAM_LEFT_MARGIN, hint_y, "1:+ 2:- 3:Save 4:Cancel");
}

// 更新页码显示
void update_page_indicator(uint8 current_page, uint8 total_pages)
{
    uint16 page_x = ips200_width_max - 80;  // 页码显示在右上角
    uint16 page_width = 70;
    
    // 清除页码区域
    for(uint8 i = 0; i < 16; i++)
    {
        ips200_draw_line(page_x, PARAM_TITLE_Y + i, page_x + page_width, PARAM_TITLE_Y + i, RGB565_BLACK);
    }
    
    // 显示页码
    ips200_set_color(RGB565_CYAN, RGB565_BLACK);
    char page_str[20];
    sprintf(page_str, "Page %d/%d", current_page + 1, total_pages);
    ips200_show_string(page_x, PARAM_TITLE_Y, page_str);
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
}

// 计算参数所在的页码
uint8 get_param_page(uint8 param_index)
{
    return param_index / PARAM_LINES_PER_PAGE;
}

// 获取总页数
uint8 get_total_pages(void)
{
    return (parameter_count + PARAM_LINES_PER_PAGE - 1) / PARAM_LINES_PER_PAGE;  // 向上取整
}

// 绘制完整页面（用于页面切换）
void draw_full_page(uint8 page, uint8 curr_param, ParameterState state)
{
    uint8 start_idx = page * PARAM_LINES_PER_PAGE;
    uint8 end_idx = start_idx + PARAM_LINES_PER_PAGE;
    if(end_idx > parameter_count) end_idx = parameter_count;
    
    // 直接绘制当前页的所有参数（不需要清除，因为已经清屏了）
    for(uint8 i = start_idx; i < end_idx; i++)
    {
        draw_param_line(i, i - start_idx, curr_param, state);
    }
    
    // 更新页码显示
    update_page_indicator(page, get_total_pages());
}

// 更新单个参数行（用于光标移动）
void update_single_param_line(uint8 old_param, uint8 new_param, ParameterState state)
{
    uint8 old_page = get_param_page(old_param);
    uint8 new_page = get_param_page(new_param);
    
    // 如果在同一页内
    if(old_page == new_page && old_page == s_current_page)
    {
        // 更新旧参数行
        uint8 old_line = old_param % PARAM_LINES_PER_PAGE;
        clear_param_line(old_line);
        draw_param_line(old_param, old_line, new_param, state);
        
        // 更新新参数行
        uint8 new_line = new_param % PARAM_LINES_PER_PAGE;
        clear_param_line(new_line);
        draw_param_line(new_param, new_line, new_param, state);
    }
}

// 更新参数值显示（编辑时只更新数值）
void update_param_value_only(uint8 param_index)
{
    if(get_param_page(param_index) == s_current_page)
    {
        // 初始化缓存
        if(!s_param_values_initialized)
        {
            for(uint8 i = 0; i < parameter_count; i++)
                s_last_param_values[i] = -999999.0f;  // 初始化为不可能的值
            s_param_values_initialized = 1;
        }
        
        // 标记有待更新，但不立即更新
        s_update_pending = param_index + 1;  // +1 是为了区分0号参数和无更新
    }
}

// 新增：执行实际的显示更新
void perform_param_value_update(void)
{
    static uint32 current_time = 0;
    
    if(s_update_pending == 0) return;
    
    // 获取当前时间（假设有系统时间函数）
    current_time = system_getval_ms();
    
    // 检查是否到了更新时间
    if(current_time - s_last_update_time < UPDATE_INTERVAL_MS) return;
    
    uint8 param_index = s_update_pending - 1;
    
    // 只有值真正改变时才更新显示
    uint8 idx = param_config[param_index].index;
    if(parameter[idx] != s_last_param_values[param_index])
    {
        uint8 line_index = param_index % PARAM_LINES_PER_PAGE;
        uint16 y_pos = PARAM_START_Y + line_index * PARAM_LINE_HEIGHT;
        uint16 value_x = PARAM_LEFT_MARGIN + PARAM_VALUE_X_OFFSET;
        
        // 直接在原位置显示新值
        ips200_set_color(RGB565_GREEN, RGB565_BLACK);
        
        // 使用自定义格式化函数，确保覆盖旧值且不四舍五入
        char value_str[16];
        format_float_truncate(value_str, parameter[idx], 7, 2);
        ips200_show_string(value_x, y_pos, value_str);
        
        ips200_set_color(RGB565_WHITE, RGB565_BLACK);
        
        // 更新缓存
        s_last_param_values[param_index] = parameter[idx];
    }
    
    // 清除待更新标记
    s_update_pending = 0;
    s_last_update_time = current_time;
}

// 初始化显示
void init_param_display(uint8 curr_param, ParameterState state)
{
    ips200_clear();
    
    // 重置参数值缓存
    s_param_values_initialized = 0;
    
    // 标题
    ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
    ips200_show_string(PARAM_LEFT_MARGIN, PARAM_TITLE_Y, "Parameter Config");
    
    // 计算当前页
    s_current_page = get_param_page(curr_param);
    
    // 绘制当前页内容
    draw_full_page(s_current_page, curr_param, state);
    
    // 操作提示
    update_operation_hint(state);
    
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    
    // 更新记录
    s_last_page = s_current_page;
    s_last_curr_param = curr_param;
    s_last_state = state;
}

// 主绘制函数
void draw_param_page(uint8 curr_param, ParameterState state)
{
    // 如果是第一次显示，进行初始化
    if(s_last_curr_param == 255)
    {
        init_param_display(curr_param, state);
        return;
    }
    
    // 计算新页码
    uint8 new_page = get_param_page(curr_param);
    
    // 如果页码改变，直接清屏重绘
    if(new_page != s_current_page)
    {
        s_current_page = new_page;
        // 直接清屏
        ips200_clear();
        
        // 重新显示标题
        ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
        ips200_show_string(PARAM_LEFT_MARGIN, PARAM_TITLE_Y, "Parameter Config");
        
        // 绘制当前页内容
        draw_full_page(s_current_page, curr_param, state);
        
        // 重新显示操作提示
        update_operation_hint(state);
        
        ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    }
    // 如果在同一页内
    else
    {
        // 如果只是光标移动
        if(s_last_curr_param != curr_param && state == s_last_state)
        {
            update_single_param_line(s_last_curr_param, curr_param, state);
        }
        // 如果状态改变（进入/退出编辑模式）
        else if(state != s_last_state)
        {
            // 更新当前参数行的颜色
            uint8 line = curr_param % PARAM_LINES_PER_PAGE;
            clear_param_line(line);
            draw_param_line(curr_param, line, curr_param, state);
            
            // 更新操作提示
            update_operation_hint(state);
        }
    }
    
    // 更新记录
    s_last_page = s_current_page;
    s_last_curr_param = curr_param;
    s_last_state = state;
}

uint8 parameter_adjust_page(void)
{
    if(!s_param_initialized)
    {
        s_curr_param = 0;
        s_param_state = PARAM_STATE_VIEW;
        s_param_initialized = 1;
        
        // 重置显示相关的静态变量
        s_current_page = 0;
        s_last_page = 255;
        s_last_curr_param = 255;
        s_last_state = PARAM_STATE_VIEW;
        s_update_pending = 0;  // 重置更新标记
        
        // 备份参数
        for(uint8 i = 0; i < parameter_count; i++)
            s_backup_params[i] = parameter[param_config[i].index];
            
        draw_param_page(s_curr_param, s_param_state);
        return 0;
    }
    
    // 定期检查并执行显示更新
    perform_param_value_update();
    
    if(key_flag)
    {
        uint8 key = key_flag;
        key_flag = 0;
        
        if(s_param_state == PARAM_STATE_VIEW)
        {
            switch(key)
            {
                case Button_1: // 上一个
                    s_curr_param = (s_curr_param > 0) ? (s_curr_param - 1) : (parameter_count - 1);
                    draw_param_page(s_curr_param, s_param_state);
                    break;
                    
                case Button_2: // 下一个
                    s_curr_param = (s_curr_param < parameter_count - 1) ? (s_curr_param + 1) : 0;
                    draw_param_page(s_curr_param, s_param_state);
                    break;
                    
                case Button_3: // 编辑
                    s_param_state = PARAM_STATE_EDIT;
                    draw_param_page(s_curr_param, s_param_state);
                    break;
                    
                case Button_4: // 返回
                    s_param_initialized = 0;
                    // 重置显示相关的静态变量
                    s_last_curr_param = 255;
                    s_last_page = 255;
                    return 1;
            }
        }
        else // PARAM_STATE_EDIT
        {
            switch(key)
            {
                case Button_1: // 增加
                {
                    uint8 idx = param_config[s_curr_param].index;
                    float new_val = parameter[idx] + param_config[s_curr_param].step;
                    if(new_val <= param_config[s_curr_param].max_value)
                    {
                        parameter[idx] = new_val;
                        if(param_config[s_curr_param].target_var != NULL)
                            *(param_config[s_curr_param].target_var) = new_val;
                        // 只更新数值显示
                        update_param_value_only(s_curr_param);
                        s_params_modified = 1;  // 标记参数已修改
                    }
                    break;
                }
                
                case Button_2: // 减少
                {
                    uint8 idx = param_config[s_curr_param].index;
                    float new_val = parameter[idx] - param_config[s_curr_param].step;
                    if(new_val >= param_config[s_curr_param].min_value)
                    {
                        parameter[idx] = new_val;
                        if(param_config[s_curr_param].target_var != NULL)
                            *(param_config[s_curr_param].target_var) = new_val;
                        // 只更新数值显示
                        update_param_value_only(s_curr_param);
                        s_params_modified = 1;  // 标记参数已修改
                    }
                    break;
                }
                
                case Button_3: // 保存
                    s_param_state = PARAM_STATE_VIEW;
                    Parameter_flash_save();
                    draw_param_page(s_curr_param, s_param_state);
                    break;
                    
                case Button_4: // 取消
                {
                    uint8 idx = param_config[s_curr_param].index;
                    parameter[idx] = s_backup_params[s_curr_param];
                    if(param_config[s_curr_param].target_var != NULL)
                        *(param_config[s_curr_param].target_var) = parameter[idx];
                    s_param_state = PARAM_STATE_VIEW;
                    draw_param_page(s_curr_param, s_param_state);
                    break;
                }
            }
        }
    }
    
    return 0;
}

//===================================================存储菜单===================================================

// 存储菜单布局参数
#define STORAGE_TITLE_Y     10      // 标题Y坐标
#define STORAGE_ITEM_START_Y 40     // 菜单项起始Y坐标
#define STORAGE_ITEM_HEIGHT 25      // 主菜单项高度
#define STORAGE_SLOT_HEIGHT 20      // 槽位选择项高度
#define STORAGE_LEFT_MARGIN 20      // 左边距

void draw_storage_menu(uint8 current_item, StorageMenuState state)
{
    ips200_clear();
    
    ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
    
    if(state == STORAGE_MENU_MAIN)
    {
        ips200_show_string(PARAM_LEFT_MARGIN, STORAGE_TITLE_Y, "Storage Menu");
        
        const char* items[] = {"Save to Slot", "Load from Slot", "Reset Default", "Return"};
        
        for(uint8 i = 0; i < 4; i++)
        {
            uint16 y_pos = STORAGE_ITEM_START_Y + i * STORAGE_ITEM_HEIGHT;
            // 确保不超出屏幕
            if(y_pos + STORAGE_ITEM_HEIGHT > ips200_height_max) break;
            
            ips200_set_color(i == current_item ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
            ips200_show_string(STORAGE_LEFT_MARGIN, y_pos, (char*)items[i]);
        }
        
        // 在底部显示操作提示
        uint16 hint_y = ips200_height_max - 40;
        ips200_set_color(RGB565_GRAY, RGB565_BLACK);
        ips200_show_string(PARAM_LEFT_MARGIN, hint_y, "1:Up 2:Down 3:Enter 4:Back");
    }
    else
    {
        const char* title = (state == STORAGE_MENU_SAVE) ? "Select Save Slot" : "Select Load Slot";
        ips200_show_string(PARAM_LEFT_MARGIN, STORAGE_TITLE_Y, (char*)title);
        
        for(uint8 i = 0; i < STORAGE_SLOT_COUNT; i++)
        {
            uint16 y_pos = STORAGE_ITEM_START_Y + i * STORAGE_SLOT_HEIGHT;
            // 确保不超出屏幕
            if(y_pos + STORAGE_SLOT_HEIGHT > ips200_height_max - 50) break;  // 留50像素给底部提示
            
            if(i == current_item)
                ips200_set_color(RGB565_GREEN, RGB565_BLACK);
            else if(storage_slot_exists(i))
                ips200_set_color(RGB565_CYAN, RGB565_BLACK);
            else
                ips200_set_color(RGB565_GRAY, RGB565_BLACK);
                
            char slot_info[32];
            sprintf(slot_info, "Slot %d: %s", i, slot_names[i]);
            ips200_show_string(STORAGE_LEFT_MARGIN, y_pos, slot_info);
        }
        
        // 在底部显示操作提示
        uint16 hint_y = ips200_height_max - 40;
        ips200_set_color(RGB565_GRAY, RGB565_BLACK);
        ips200_show_string(PARAM_LEFT_MARGIN, hint_y, "1:Up 2:Down 3:OK 4:Back");
    }
    
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
}

uint8 storage_menu_page(void)
{
    if(!s_storage_initialized)
    {
        s_storage_curr_item = 0;
        s_storage_state = STORAGE_MENU_MAIN;
        s_storage_initialized = 1;
        draw_storage_menu(s_storage_curr_item, s_storage_state);
        return 0;
    }
    
    if(key_flag)
    {
        uint8 key = key_flag;
        key_flag = 0;
        
        if(s_storage_state == STORAGE_MENU_MAIN)
        {
            switch(key)
            {
                case Button_1: // 上一项
                    s_storage_curr_item = (s_storage_curr_item > 0) ? (s_storage_curr_item - 1) : 3;
                    draw_storage_menu(s_storage_curr_item, s_storage_state);
                    break;
                    
                case Button_2: // 下一项
                    s_storage_curr_item = (s_storage_curr_item < 3) ? (s_storage_curr_item + 1) : 0;
                    draw_storage_menu(s_storage_curr_item, s_storage_state);
                    break;
                    
                case Button_3: // 选择
                    switch(s_storage_curr_item)
                    {
                        case 0: // Save
                            s_storage_state = STORAGE_MENU_SAVE;
                            s_storage_curr_item = 0;
                            break;
                        case 1: // Load
                            s_storage_state = STORAGE_MENU_LOAD;
                            s_storage_curr_item = 0;
                            break;
                        case 2: // Reset
                            ips200_clear();
                            // 居中显示提示信息
                            uint16 msg_y = ips200_height_max / 2 - 10;
                            ips200_show_string(STORAGE_LEFT_MARGIN, msg_y, "Resetting...");
                            Parameter_reset_to_default();
                            Parameter_flash_save();
                            system_delay_ms(1000);
                            break;
                        case 3: // Return
                            s_storage_initialized = 0;
                            return 1;
                    }
                    draw_storage_menu(s_storage_curr_item, s_storage_state);
                    break;
                    
                case Button_4: // 返回
                    s_storage_initialized = 0;
                    return 1;
            }
        }
        else // 槽位选择
        {
            switch(key)
            {
                case Button_1: // 上一个槽位
                    s_storage_curr_item = (s_storage_curr_item > 0) ? (s_storage_curr_item - 1) : (STORAGE_SLOT_COUNT - 1);
                    draw_storage_menu(s_storage_curr_item, s_storage_state);
                    break;
                    
                case Button_2: // 下一个槽位
                    s_storage_curr_item = (s_storage_curr_item < STORAGE_SLOT_COUNT - 1) ? (s_storage_curr_item + 1) : 0;
                    draw_storage_menu(s_storage_curr_item, s_storage_state);
                    break;
                    
                case Button_3: // 确认
                    ips200_clear();
                    // 居中显示消息
                    uint16 msg_y = ips200_height_max / 2 - 10;
                    if(s_storage_state == STORAGE_MENU_SAVE)
                    {
                        ips200_show_string(STORAGE_LEFT_MARGIN, msg_y, "Saving...");
                        Parameter_flash_save_slot(s_storage_curr_item);
                        ips200_show_string(STORAGE_LEFT_MARGIN, msg_y + 20, "Saved!");
                    }
                    else
                    {
                        if(storage_slot_exists(s_storage_curr_item))
                        {
                            ips200_show_string(STORAGE_LEFT_MARGIN, msg_y, "Loading...");
                            Parameter_flash_load_slot(s_storage_curr_item);
                            ips200_show_string(STORAGE_LEFT_MARGIN, msg_y + 20, "Loaded!");
                        }
                        else
                        {
                            ips200_show_string(STORAGE_LEFT_MARGIN, msg_y, "Slot Empty!");
                        }
                    }
                    system_delay_ms(1000);
                    s_storage_state = STORAGE_MENU_MAIN;
                    s_storage_curr_item = 0;
                    draw_storage_menu(s_storage_curr_item, s_storage_state);
                    break;
                    
                case Button_4: // 返回
                    s_storage_state = STORAGE_MENU_MAIN;
                    s_storage_curr_item = 0;
                    draw_storage_menu(s_storage_curr_item, s_storage_state);
                    break;
            }
        }
    }
    
    return 0;
}

//===================================================主菜单入口===================================================

void parameter_menu_entry(void)
{
    ips200_clear();
    // 居中显示加载信息
    uint16 msg_y = ips200_height_max / 2 - 10;
    ips200_show_string(STORAGE_LEFT_MARGIN, msg_y, "Loading parameters...");
    
    Parameter_flash_load();
    system_delay_ms(500);
    
    s_param_initialized = 0;
    s_params_modified = 0;  // 重置修改标记
    uint8 exit_flag = 0;
    key_flag = 0;
    
    // 备份初始参数值，用于检测是否有修改
    float initial_params[MAX_PARAMETERS];
    for(uint8 i = 0; i < parameter_count; i++)
        initial_params[i] = parameter[param_config[i].index];
    
    while(!exit_flag)
    {
        exit_flag = parameter_adjust_page();
        system_delay_ms(10);
    }
    
    // 检查是否有参数被修改
    for(uint8 i = 0; i < parameter_count; i++)
    {
        if(parameter[param_config[i].index] != initial_params[i])
        {
            s_params_modified = 1;
            break;
        }
    }
    
    // 如果有参数被修改，显示提示
    if(s_params_modified)
    {
        ips200_clear();
        ips200_set_color(RGB565_GREEN, RGB565_BLACK);
        ips200_show_string(STORAGE_LEFT_MARGIN, msg_y, "Parameters Saved!");
        ips200_show_string(STORAGE_LEFT_MARGIN, msg_y + 20, "Returning to menu...");
        ips200_set_color(RGB565_WHITE, RGB565_BLACK);
        system_delay_ms(500);
    }
    
    // 设置返回按键标志，让菜单系统知道需要返回上级菜单
    key_flag = Button_4;
}

void storage_menu_entry(void)
{
    s_storage_initialized = 0;
    uint8 exit_flag = 0;
    key_flag = 0;
    
    while(!exit_flag)
    {
        exit_flag = storage_menu_page();
        system_delay_ms(10);
    }
    
    // 设置返回按键标志，让菜单系统知道需要返回上级菜单
    key_flag = Button_4;
}

//===================================================参数索引检查===================================================

// 检查参数索引是否有冲突
uint8 check_parameter_index_conflicts(void)
{
    uint8 index_used[MAX_PARAMETERS] = {0};  // 标记数组，记录哪些索引被使用
    uint8 conflicts = 0;
    
    for(uint8 i = 0; i < parameter_count; i++)
    {
        uint8 idx = param_config[i].index;
        
        // 检查索引是否超出范围
        if(idx >= MAX_PARAMETERS)
        {
            conflicts++;
            continue;
        }
        
        // 检查索引是否重复使用
        if(index_used[idx])
        {
            conflicts++;
        }
        else
        {
            index_used[idx] = 1;
        }
    }
    
    return conflicts;
}

//===================================================方案查看功能===================================================

// 静态变量 for 方案查看
static uint8 s_view_curr_param = 0;
static uint8 s_view_initialized = 0;
static ParameterScheme s_current_viewing_scheme;

// 根据索引获取方案数据
uint8 get_scheme_by_index(uint8 index, ParameterScheme* scheme)
{
    if(index >= MAX_SCHEMES || scheme == NULL) return 0;
    
    load_scheme_list();  // 确保方案列表已加载
    
    if(!schemes[index].valid) return 0;
    
    // 复制方案数据
    memcpy(scheme, &schemes[index], sizeof(ParameterScheme));
    return 1;
}

// 显示方案详细信息
void view_scheme_details(uint8 scheme_index)
{
    if(!get_scheme_by_index(scheme_index, &s_current_viewing_scheme))
        return;
    
    ips200_clear();
    ips200_set_color(RGB565_CYAN, RGB565_BLACK);
    
    // 显示方案标题
    char title[32];
    sprintf(title, "Scheme: %.12s", s_current_viewing_scheme.name);
    ips200_show_string(5, 5, title);
    
    // 显示速度值
    char speed_str[32];
    format_float_truncate(speed_str, s_current_viewing_scheme.speed_value, 8, 2);
    sprintf(title, "Speed: %s", speed_str);
    ips200_show_string(5, 25, title);
    
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    
    // 显示分隔线
    ips200_show_string(5, 45, "Parameters:");
    
    s_view_initialized = 1;
}

// 绘制方案查看页面的参数
void draw_view_param_page(uint8 curr_param)
{
    if(!s_view_initialized) return;
    
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    
    // 计算当前页面
    uint8 params_per_page = 8;  // 每页显示8个参数
    uint8 current_page = curr_param / params_per_page;
    uint8 total_pages = (parameter_count + params_per_page - 1) / params_per_page;
    uint8 start_param = current_page * params_per_page;
    uint8 end_param = start_param + params_per_page;
    if(end_param > parameter_count) end_param = parameter_count;
    
    // 清除参数显示区域
    for(uint8 line = 0; line < params_per_page; line++)
    {
        uint16 y = 65 + line * 18;
        ips200_show_string(0, y, "                    ");  // 清除整行
    }
    
    // 显示当前页的参数
    for(uint8 i = start_param; i < end_param; i++)
    {
        uint8 line_index = i - start_param;
        uint16 y = 65 + line_index * 18;
        
        // 高亮当前选中的参数
        if(i == curr_param)
        {
            ips200_set_color(RGB565_BLACK, RGB565_WHITE);
        }
        else
        {
            ips200_set_color(RGB565_WHITE, RGB565_BLACK);
        }
        
        // 显示参数名称
        char name_str[20];
        sprintf(name_str, "%.18s", param_config[i].name);
        ips200_show_string(5, y, name_str);
        
        // 显示参数值（从方案中获取）
        char value_str[16];
        uint8 param_idx = param_config[i].index;
        if(param_idx < MAX_PARAMETERS)
        {
            format_float_truncate(value_str, s_current_viewing_scheme.parameters[param_idx], 8, 2);
            ips200_show_string(150, y, value_str);
        }
        else
        {
            ips200_show_string(150, y, "N/A");
        }
    }
    
    // 恢复正常颜色
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    
    // 显示页面指示器
    char page_info[32];
    sprintf(page_info, "Page %d/%d", current_page + 1, total_pages);
    ips200_show_string(5, 210, page_info);
    
    // 显示操作提示
    ips200_show_string(5, 225, "U/D:Select  B4:Back");
}

// 方案查看页面主函数
uint8 view_scheme_page(uint8 scheme_index)
{
    // 初始化
    if(!s_view_initialized)
    {
        if(!get_scheme_by_index(scheme_index, &s_current_viewing_scheme))
        {
            ips200_clear();
            ips200_show_string(50, 100, "Scheme not found!");
            system_delay_ms(1000);
            return 1;  // 返回上级菜单
        }
        
        view_scheme_details(scheme_index);
        s_view_curr_param = 0;
        draw_view_param_page(s_view_curr_param);
    }
    
    // 处理按键
    if(key_flag)
    {
        uint8 key = key_flag;
        key_flag = 0;
        
        switch(key)
        {
            case Button_1: // 上一个参数
                if(s_view_curr_param > 0)
                {
                    s_view_curr_param--;
                }
                else
                {
                    s_view_curr_param = parameter_count - 1;
                }
                draw_view_param_page(s_view_curr_param);
                break;
                
            case Button_2: // 下一个参数
                if(s_view_curr_param < parameter_count - 1)
                {
                    s_view_curr_param++;
                }
                else
                {
                    s_view_curr_param = 0;
                }
                draw_view_param_page(s_view_curr_param);
                break;
                
            case Button_3: // 暂时无功能，可以后续扩展
                break;
                
            case Button_4: // 返回
                s_view_initialized = 0;
                return 1;
        }
    }
    
    return 0;
} 
