#include "parameter_flash.h"
#include "zf_device_ips200.h"
#include "key.h"
#include <string.h>

//===================================================�Զ�����ֵ��ʽ��===================================================

// �ضϸ�������ʽ��������������
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

//===================================================�����洢�͹���===================================================

// �����洢����
float parameter[MAX_PARAMETERS];

// �������ñ�
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

// ��̬����
static uint8 s_curr_param = 0;
static ParameterState s_param_state = PARAM_STATE_VIEW;
static uint8 s_param_initialized = 0;
static float s_backup_params[MAX_PARAMETERS];

static uint8 s_storage_curr_item = 0;
static StorageMenuState s_storage_state = STORAGE_MENU_MAIN;
static uint8 s_storage_initialized = 0;

// ��λ����
static char slot_names[STORAGE_SLOT_COUNT][16] = {
    "Default", "Config 1", "Config 2", "Config 3", "Config 4"
};

// ��Ӳ���ֵ����
static float s_last_param_values[MAX_PARAMETERS];  // ��һ����ʾ�Ĳ���ֵ
static uint8 s_param_values_initialized = 0;
static uint8 s_update_pending = 0;  // �Ƿ��д����µ���ʾ
static uint32 s_last_update_time = 0;  // �ϴθ���ʱ��
#define UPDATE_INTERVAL_MS 50  // ���¼�������룩

// ��Ӳ����޸ı��
static uint8 s_params_modified = 0;  // �Ƿ��в������޸Ĺ�

//===================================================������������===================================================

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
        // Ӧ�ò�����Ŀ�����
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
    
    // ���Ƚ���ǰ����ֵͬ����parameter����
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

//===================================================����������===================================================

// ��̬����
static ParameterScheme schemes[MAX_SCHEMES];
static uint8 s_scheme_initialized = 0;
static uint8 s_scheme_curr_item = 0;
static SchemeMenuState s_scheme_state = SCHEME_MENU_MAIN;
static uint8 s_scheme_list_loaded = 0;

// ��̬���� for draw_scheme_menu optimization
static uint8 s_dsm_last_curr_item = 0xFF;
static SchemeMenuState s_dsm_last_state = (SchemeMenuState)0xFF; // Use an invalid enum cast
static uint8 s_dsm_last_valid_count = 0xFF;
static uint8 s_dsm_needs_full_redraw = 1;

// �����˵����ֲ���
#define SCHEME_TITLE_Y       10      // ����Y����
#define SCHEME_ITEM_START_Y  40      // �˵�����ʼY����
#define SCHEME_ITEM_HEIGHT   20      // �˵���߶�
#define SCHEME_LEFT_MARGIN   15      // ��߾�
#define SCHEME_LINES_PER_PAGE 10     // ÿҳ��ʾ�ķ�����

// ���ط����б�
void load_scheme_list(void)
{
    if(s_scheme_list_loaded) return;
    
    uint32 total_data_to_read = sizeof(schemes);

    if (total_data_to_read > sizeof(flash_union_buffer)) {
        for(uint8 i = 0; i < MAX_SCHEMES; i++) {
            schemes[i].valid = 0;
        }
        s_scheme_list_loaded = 1; // ���Ϊ�Ѽ��أ���Ȼ�ǿ�/����״̬��
        return;
    }

    flash_read_page_to_buffer(0, SCHEME_FLASH_BASE_INDEX);
    
    memcpy(schemes, flash_union_buffer, total_data_to_read);
    
    s_scheme_list_loaded = 1;
}

// ���淽���б�
void save_scheme_list(void)
{
    uint32 total_data_to_write = sizeof(schemes);

    if (total_data_to_write > sizeof(flash_union_buffer)) {
        return; // �޷����棬��Ϊ����̫��
    }

    memset(flash_union_buffer, 0xFF, sizeof(flash_union_buffer));
    
    memcpy(flash_union_buffer, schemes, total_data_to_write);
    
    flash_erase_page(0, SCHEME_FLASH_BASE_INDEX);
    
    flash_write_page_from_buffer(0, SCHEME_FLASH_BASE_INDEX);
}

// ��ȡ��������
uint8 get_scheme_count(void)
{
    uint8 count = 0;
    for(uint8 i = 0; i < MAX_SCHEMES; i++)
    {
        if(schemes[i].valid) count++;
    }
    return count;
}

// ���ɷ�������
void generate_scheme_name(char* name_buffer, float speed_value)
{
    // ����ͬ�ٶ�ֵ�ķ�������
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
        // ��һ�����ٶȵķ�����ֱ��ʹ���ٶ�ֵ
        format_float_truncate(name_buffer, speed_value, 8, 2);
        // ȥ��β��ո�
        uint8 len = strlen(name_buffer);
        while(len > 0 && name_buffer[len-1] == ' ')
        {
            name_buffer[len-1] = '\0';
            len--;
        }
    }
    else
    {
        // �Ѵ��ڸ��ٶȵķ�������Ӱ汾��
        char base_name[10];
        format_float_truncate(base_name, speed_value, 8, 2);
        // ȥ��β��ո�
        uint8 len = strlen(base_name);
        while(len > 0 && base_name[len-1] == ' ')
        {
            base_name[len-1] = '\0';
            len--;
        }
        sprintf(name_buffer, "%s.%d", base_name, version_count + 1);
    }
}

// ���浱ǰ����Ϊ����
void save_current_parameters_as_scheme(void)
{
    // ǿ�����¼���scheme�б�ȷ�����������µ�
    s_scheme_list_loaded = 0;
    load_scheme_list();
    
    // ���Ƚ���ǰ����ֵͬ����parameter����
    for(uint8 i = 0; i < parameter_count; i++)
    {
        if(param_config[i].target_var != NULL)
        {
            parameter[param_config[i].index] = *(param_config[i].target_var);
        }
    }
    
    // ���ҿ��в�λ
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
    
    // ��ȡ�ٶ�ֵ������11��Ӧspeed��
    float speed_value = parameter[param_config[11].index];  // Speed������������11
    
    // ���ɷ�������
    generate_scheme_name(schemes[free_slot].name, speed_value);
    
    // ���淽������
    schemes[free_slot].valid = 1;
    schemes[free_slot].speed_value = speed_value;
    for(uint8 i = 0; i < parameter_count; i++)
    {
        schemes[free_slot].parameters[param_config[i].index] = parameter[param_config[i].index];
    }
    
    // ���浽flash
    save_scheme_list();
    
    // ��ʾ����ɹ���Ϣ
    ips200_clear();
    ips200_set_color(RGB565_GREEN, RGB565_BLACK);
    char msg[32];
    sprintf(msg, "Scheme %s Saved", schemes[free_slot].name);
    ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2 - 10, msg);
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    system_delay_ms(1500);
}

// ɾ������
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

// ���ط�������
void load_scheme_parameters(uint8 index)
{
    // ȷ��scheme�б������µ�
    s_scheme_list_loaded = 0;
    load_scheme_list();
    
    if(index >= MAX_SCHEMES || !schemes[index].valid) return;
    
    // ȷ�ϼ���
    ips200_clear();
    ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
    ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2 - 20, "Params will be overwritten!");
    ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2, "Confirm load? (3=Yes/4=No)");
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    
    // �ȴ��û�ȷ��
    while(1)
    {
        if(key_flag)
        {
            uint8 key = key_flag;
            key_flag = 0;
            
            if(key == Button_3)  // ȷ��
            {
                // ���ز���
                for(uint8 i = 0; i < parameter_count; i++)
                {
                    uint8 idx = param_config[i].index;
                    parameter[idx] = schemes[index].parameters[idx];
                    if(param_config[i].target_var != NULL)
                        *(param_config[i].target_var) = parameter[idx];
                }
                
                // ���浽Ĭ�ϲ�λ
                Parameter_flash_save();
                
                ips200_clear();
                ips200_set_color(RGB565_GREEN, RGB565_BLACK);
                ips200_show_string(SCHEME_LEFT_MARGIN, ips200_height_max / 2 - 10, "Params Applied");
                ips200_set_color(RGB565_WHITE, RGB565_BLACK);
                system_delay_ms(1000);
                break;
            }
            else if(key == Button_4)  // ȡ��
            {
                break;
            }
        }
        system_delay_ms(10);
    }
}

// ���Ʒ�������˵�
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

// ��������ҳ��
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
                case Button_1: // ��һ��
                    s_scheme_curr_item = (s_scheme_curr_item > 0) ? (s_scheme_curr_item - 1) : 4;
                    draw_scheme_menu(s_scheme_curr_item, s_scheme_state);
                    break;
                    
                case Button_2: // ��һ��
                    s_scheme_curr_item = (s_scheme_curr_item < 4) ? (s_scheme_curr_item + 1) : 0;
                    draw_scheme_menu(s_scheme_curr_item, s_scheme_state);
                    break;
                    
                case Button_3: // ѡ��
                    switch(s_scheme_curr_item)
                    {
                        case 0: // ���浱ǰ����
                            save_current_parameters_as_scheme();
                            s_scheme_list_loaded = 0;  // ǿ�����¼���
                            load_scheme_list();  // ���¼����б�, list content might change
                            s_dsm_needs_full_redraw = 1;
                            // s_scheme_state remains SCHEME_MENU_MAIN
                            // s_scheme_curr_item could be reset or stay, draw_scheme_menu will handle
                            break;
                        case 1: // ɾ������
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
                        case 2: // ���ط���
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
                        case 3: // �鿴����
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
                        case 4: // �˳�
                            s_scheme_initialized = 0; // This will trigger full redraw on next entry
                            return 1;
                    }
                    draw_scheme_menu(s_scheme_curr_item, s_scheme_state); // Redraw after action or state change
                    break;
                    
                case Button_4: // ���� (from main menu)
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
                case Button_1: // ��һ������
                    if(current_valid_count > 0)
                    {
                        s_scheme_curr_item = (s_scheme_curr_item > 0) ? (s_scheme_curr_item - 1) : (current_valid_count - 1);
                        draw_scheme_menu(s_scheme_curr_item, s_scheme_state);
                    }
                    break;
                    
                case Button_2: // ��һ������
                    if(current_valid_count > 0)
                    {
                        s_scheme_curr_item = (s_scheme_curr_item < current_valid_count - 1) ? (s_scheme_curr_item + 1) : 0;
                        draw_scheme_menu(s_scheme_curr_item, s_scheme_state);
                    }
                    break;
                    
                case Button_3: // ȷ�ϲ��� (Delete or Load)
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
                            // ���뷽���鿴ҳ��
                            uint8 view_exit = 0;
                            while(!view_exit)
                            {
                                view_exit = view_scheme_page(actual_scheme_index);
                                system_delay_ms(10);
                            }
                            // �Ӳ鿴ҳ�淵�غ󣬻ص����˵�
                            s_scheme_state = SCHEME_MENU_MAIN;
                            s_scheme_curr_item = 0;
                            s_dsm_needs_full_redraw = 1;
                        }
                    }
                    draw_scheme_menu(s_scheme_curr_item, s_scheme_state); // Redraw after action
                    break;
                    
                case Button_4: // �������˵� (from sub-menu)
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

// ����������ں���
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
    
    // ���÷��ذ�����־���ò˵�ϵͳ֪����Ҫ�����ϼ��˵�
    key_flag = Button_4;
}

//===================================================��������ҳ��===================================================

// ��Ӿ�̬������������ʾ״̬
static uint8 s_current_page = 0;  // ��ǰҳ��
static uint8 s_last_page = 255;  // ��һ�ε�ҳ��
static uint8 s_last_curr_param = 255;  // ��һ�εĵ�ǰ����
static ParameterState s_last_state = PARAM_STATE_VIEW;  // ��һ�ε�״̬

// ���ֲ�����������Ļ�ߴ綯̬���㣩
#define PARAM_LINE_HEIGHT   18      // ÿ�в����ĸ߶�
#define PARAM_LEFT_MARGIN   10      // ��߾�
#define PARAM_VALUE_X_OFFSET 130    // ����ֵ�������߾��ƫ��
#define PARAM_TITLE_Y       10      // ����Y����
#define PARAM_START_Y       35      // �����б���ʼY����
#define PARAM_LINES_PER_PAGE 12      // ÿҳ��ʾ�Ĳ�������

// ���ָ���е�����
void clear_param_line(uint8 line_index)
{
    uint16 y_pos = PARAM_START_Y + line_index * PARAM_LINE_HEIGHT;
    uint16 line_width = ips200_width_max - PARAM_LEFT_MARGIN - 10;  // ��10�����ұ߾�
    
    // ������У��ú�ɫ��䣩
    for(uint8 i = 0; i < PARAM_LINE_HEIGHT && (y_pos + i) < ips200_height_max; i++)
    {
        ips200_draw_line(PARAM_LEFT_MARGIN, y_pos + i, PARAM_LEFT_MARGIN + line_width, y_pos + i, RGB565_BLACK);
    }
}

// ���Ƶ���������
void draw_param_line(uint8 param_index, uint8 line_index, uint8 curr_param, ParameterState state)
{
    if(param_index >= parameter_count) return;
    
    uint16 y_pos = PARAM_START_Y + line_index * PARAM_LINE_HEIGHT;
    uint16 value_x = PARAM_LEFT_MARGIN + PARAM_VALUE_X_OFFSET;
    
    // ȷ����������Ļ�ײ�
    if(y_pos + PARAM_LINE_HEIGHT > ips200_height_max) return;
    
    // ������ɫ
    if(param_index == curr_param)
    {
        ips200_set_color(state == PARAM_STATE_EDIT ? RGB565_GREEN : RGB565_BLUE, RGB565_BLACK);
    }
    else
    {
        ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    }
    
    // ��ʾ��������ֵ
    ips200_show_string(PARAM_LEFT_MARGIN, y_pos, (char*)param_config[param_index].name);
    
    // ʹ���Զ����ʽ��������ʾ��ʵ��ֵ
    char value_str[16];
    format_float_truncate(value_str, parameter[param_config[param_index].index], 7, 2);
    ips200_show_string(value_x, y_pos, value_str);
}

// ���²�����ʾ
void update_operation_hint(ParameterState state)
{
    uint16 hint_y = ips200_height_max - 40;  // ��ʾ��Ϣ����ײ�40����
    uint16 hint_width = ips200_width_max - PARAM_LEFT_MARGIN * 2;
    
    // �����ʾ����
    for(uint8 i = 0; i < 16 && (hint_y + i) < ips200_height_max; i++)
    {
        ips200_draw_line(PARAM_LEFT_MARGIN, hint_y + i, PARAM_LEFT_MARGIN + hint_width, hint_y + i, RGB565_BLACK);
    }
    
    // ��ʾ�µ���ʾ
    ips200_set_color(RGB565_GRAY, RGB565_BLACK);
    if(state == PARAM_STATE_VIEW)
        ips200_show_string(PARAM_LEFT_MARGIN, hint_y, "1:Up 2:Down 3:Edit 4:Menu");
    else
        ips200_show_string(PARAM_LEFT_MARGIN, hint_y, "1:+ 2:- 3:Save 4:Cancel");
}

// ����ҳ����ʾ
void update_page_indicator(uint8 current_page, uint8 total_pages)
{
    uint16 page_x = ips200_width_max - 80;  // ҳ����ʾ�����Ͻ�
    uint16 page_width = 70;
    
    // ���ҳ������
    for(uint8 i = 0; i < 16; i++)
    {
        ips200_draw_line(page_x, PARAM_TITLE_Y + i, page_x + page_width, PARAM_TITLE_Y + i, RGB565_BLACK);
    }
    
    // ��ʾҳ��
    ips200_set_color(RGB565_CYAN, RGB565_BLACK);
    char page_str[20];
    sprintf(page_str, "Page %d/%d", current_page + 1, total_pages);
    ips200_show_string(page_x, PARAM_TITLE_Y, page_str);
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
}

// ����������ڵ�ҳ��
uint8 get_param_page(uint8 param_index)
{
    return param_index / PARAM_LINES_PER_PAGE;
}

// ��ȡ��ҳ��
uint8 get_total_pages(void)
{
    return (parameter_count + PARAM_LINES_PER_PAGE - 1) / PARAM_LINES_PER_PAGE;  // ����ȡ��
}

// ��������ҳ�棨����ҳ���л���
void draw_full_page(uint8 page, uint8 curr_param, ParameterState state)
{
    uint8 start_idx = page * PARAM_LINES_PER_PAGE;
    uint8 end_idx = start_idx + PARAM_LINES_PER_PAGE;
    if(end_idx > parameter_count) end_idx = parameter_count;
    
    // ֱ�ӻ��Ƶ�ǰҳ�����в���������Ҫ�������Ϊ�Ѿ������ˣ�
    for(uint8 i = start_idx; i < end_idx; i++)
    {
        draw_param_line(i, i - start_idx, curr_param, state);
    }
    
    // ����ҳ����ʾ
    update_page_indicator(page, get_total_pages());
}

// ���µ��������У����ڹ���ƶ���
void update_single_param_line(uint8 old_param, uint8 new_param, ParameterState state)
{
    uint8 old_page = get_param_page(old_param);
    uint8 new_page = get_param_page(new_param);
    
    // �����ͬһҳ��
    if(old_page == new_page && old_page == s_current_page)
    {
        // ���¾ɲ�����
        uint8 old_line = old_param % PARAM_LINES_PER_PAGE;
        clear_param_line(old_line);
        draw_param_line(old_param, old_line, new_param, state);
        
        // �����²�����
        uint8 new_line = new_param % PARAM_LINES_PER_PAGE;
        clear_param_line(new_line);
        draw_param_line(new_param, new_line, new_param, state);
    }
}

// ���²���ֵ��ʾ���༭ʱֻ������ֵ��
void update_param_value_only(uint8 param_index)
{
    if(get_param_page(param_index) == s_current_page)
    {
        // ��ʼ������
        if(!s_param_values_initialized)
        {
            for(uint8 i = 0; i < parameter_count; i++)
                s_last_param_values[i] = -999999.0f;  // ��ʼ��Ϊ�����ܵ�ֵ
            s_param_values_initialized = 1;
        }
        
        // ����д����£�������������
        s_update_pending = param_index + 1;  // +1 ��Ϊ������0�Ų������޸���
    }
}

// ������ִ��ʵ�ʵ���ʾ����
void perform_param_value_update(void)
{
    static uint32 current_time = 0;
    
    if(s_update_pending == 0) return;
    
    // ��ȡ��ǰʱ�䣨������ϵͳʱ�亯����
    current_time = system_getval_ms();
    
    // ����Ƿ��˸���ʱ��
    if(current_time - s_last_update_time < UPDATE_INTERVAL_MS) return;
    
    uint8 param_index = s_update_pending - 1;
    
    // ֻ��ֵ�����ı�ʱ�Ÿ�����ʾ
    uint8 idx = param_config[param_index].index;
    if(parameter[idx] != s_last_param_values[param_index])
    {
        uint8 line_index = param_index % PARAM_LINES_PER_PAGE;
        uint16 y_pos = PARAM_START_Y + line_index * PARAM_LINE_HEIGHT;
        uint16 value_x = PARAM_LEFT_MARGIN + PARAM_VALUE_X_OFFSET;
        
        // ֱ����ԭλ����ʾ��ֵ
        ips200_set_color(RGB565_GREEN, RGB565_BLACK);
        
        // ʹ���Զ����ʽ��������ȷ�����Ǿ�ֵ�Ҳ���������
        char value_str[16];
        format_float_truncate(value_str, parameter[idx], 7, 2);
        ips200_show_string(value_x, y_pos, value_str);
        
        ips200_set_color(RGB565_WHITE, RGB565_BLACK);
        
        // ���»���
        s_last_param_values[param_index] = parameter[idx];
    }
    
    // ��������±��
    s_update_pending = 0;
    s_last_update_time = current_time;
}

// ��ʼ����ʾ
void init_param_display(uint8 curr_param, ParameterState state)
{
    ips200_clear();
    
    // ���ò���ֵ����
    s_param_values_initialized = 0;
    
    // ����
    ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
    ips200_show_string(PARAM_LEFT_MARGIN, PARAM_TITLE_Y, "Parameter Config");
    
    // ���㵱ǰҳ
    s_current_page = get_param_page(curr_param);
    
    // ���Ƶ�ǰҳ����
    draw_full_page(s_current_page, curr_param, state);
    
    // ������ʾ
    update_operation_hint(state);
    
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    
    // ���¼�¼
    s_last_page = s_current_page;
    s_last_curr_param = curr_param;
    s_last_state = state;
}

// �����ƺ���
void draw_param_page(uint8 curr_param, ParameterState state)
{
    // ����ǵ�һ����ʾ�����г�ʼ��
    if(s_last_curr_param == 255)
    {
        init_param_display(curr_param, state);
        return;
    }
    
    // ������ҳ��
    uint8 new_page = get_param_page(curr_param);
    
    // ���ҳ��ı䣬ֱ�������ػ�
    if(new_page != s_current_page)
    {
        s_current_page = new_page;
        // ֱ������
        ips200_clear();
        
        // ������ʾ����
        ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
        ips200_show_string(PARAM_LEFT_MARGIN, PARAM_TITLE_Y, "Parameter Config");
        
        // ���Ƶ�ǰҳ����
        draw_full_page(s_current_page, curr_param, state);
        
        // ������ʾ������ʾ
        update_operation_hint(state);
        
        ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    }
    // �����ͬһҳ��
    else
    {
        // ���ֻ�ǹ���ƶ�
        if(s_last_curr_param != curr_param && state == s_last_state)
        {
            update_single_param_line(s_last_curr_param, curr_param, state);
        }
        // ���״̬�ı䣨����/�˳��༭ģʽ��
        else if(state != s_last_state)
        {
            // ���µ�ǰ�����е���ɫ
            uint8 line = curr_param % PARAM_LINES_PER_PAGE;
            clear_param_line(line);
            draw_param_line(curr_param, line, curr_param, state);
            
            // ���²�����ʾ
            update_operation_hint(state);
        }
    }
    
    // ���¼�¼
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
        
        // ������ʾ��صľ�̬����
        s_current_page = 0;
        s_last_page = 255;
        s_last_curr_param = 255;
        s_last_state = PARAM_STATE_VIEW;
        s_update_pending = 0;  // ���ø��±��
        
        // ���ݲ���
        for(uint8 i = 0; i < parameter_count; i++)
            s_backup_params[i] = parameter[param_config[i].index];
            
        draw_param_page(s_curr_param, s_param_state);
        return 0;
    }
    
    // ���ڼ�鲢ִ����ʾ����
    perform_param_value_update();
    
    if(key_flag)
    {
        uint8 key = key_flag;
        key_flag = 0;
        
        if(s_param_state == PARAM_STATE_VIEW)
        {
            switch(key)
            {
                case Button_1: // ��һ��
                    s_curr_param = (s_curr_param > 0) ? (s_curr_param - 1) : (parameter_count - 1);
                    draw_param_page(s_curr_param, s_param_state);
                    break;
                    
                case Button_2: // ��һ��
                    s_curr_param = (s_curr_param < parameter_count - 1) ? (s_curr_param + 1) : 0;
                    draw_param_page(s_curr_param, s_param_state);
                    break;
                    
                case Button_3: // �༭
                    s_param_state = PARAM_STATE_EDIT;
                    draw_param_page(s_curr_param, s_param_state);
                    break;
                    
                case Button_4: // ����
                    s_param_initialized = 0;
                    // ������ʾ��صľ�̬����
                    s_last_curr_param = 255;
                    s_last_page = 255;
                    return 1;
            }
        }
        else // PARAM_STATE_EDIT
        {
            switch(key)
            {
                case Button_1: // ����
                {
                    uint8 idx = param_config[s_curr_param].index;
                    float new_val = parameter[idx] + param_config[s_curr_param].step;
                    if(new_val <= param_config[s_curr_param].max_value)
                    {
                        parameter[idx] = new_val;
                        if(param_config[s_curr_param].target_var != NULL)
                            *(param_config[s_curr_param].target_var) = new_val;
                        // ֻ������ֵ��ʾ
                        update_param_value_only(s_curr_param);
                        s_params_modified = 1;  // ��ǲ������޸�
                    }
                    break;
                }
                
                case Button_2: // ����
                {
                    uint8 idx = param_config[s_curr_param].index;
                    float new_val = parameter[idx] - param_config[s_curr_param].step;
                    if(new_val >= param_config[s_curr_param].min_value)
                    {
                        parameter[idx] = new_val;
                        if(param_config[s_curr_param].target_var != NULL)
                            *(param_config[s_curr_param].target_var) = new_val;
                        // ֻ������ֵ��ʾ
                        update_param_value_only(s_curr_param);
                        s_params_modified = 1;  // ��ǲ������޸�
                    }
                    break;
                }
                
                case Button_3: // ����
                    s_param_state = PARAM_STATE_VIEW;
                    Parameter_flash_save();
                    draw_param_page(s_curr_param, s_param_state);
                    break;
                    
                case Button_4: // ȡ��
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

//===================================================�洢�˵�===================================================

// �洢�˵����ֲ���
#define STORAGE_TITLE_Y     10      // ����Y����
#define STORAGE_ITEM_START_Y 40     // �˵�����ʼY����
#define STORAGE_ITEM_HEIGHT 25      // ���˵���߶�
#define STORAGE_SLOT_HEIGHT 20      // ��λѡ����߶�
#define STORAGE_LEFT_MARGIN 20      // ��߾�

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
            // ȷ����������Ļ
            if(y_pos + STORAGE_ITEM_HEIGHT > ips200_height_max) break;
            
            ips200_set_color(i == current_item ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
            ips200_show_string(STORAGE_LEFT_MARGIN, y_pos, (char*)items[i]);
        }
        
        // �ڵײ���ʾ������ʾ
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
            // ȷ����������Ļ
            if(y_pos + STORAGE_SLOT_HEIGHT > ips200_height_max - 50) break;  // ��50���ظ��ײ���ʾ
            
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
        
        // �ڵײ���ʾ������ʾ
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
                case Button_1: // ��һ��
                    s_storage_curr_item = (s_storage_curr_item > 0) ? (s_storage_curr_item - 1) : 3;
                    draw_storage_menu(s_storage_curr_item, s_storage_state);
                    break;
                    
                case Button_2: // ��һ��
                    s_storage_curr_item = (s_storage_curr_item < 3) ? (s_storage_curr_item + 1) : 0;
                    draw_storage_menu(s_storage_curr_item, s_storage_state);
                    break;
                    
                case Button_3: // ѡ��
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
                            // ������ʾ��ʾ��Ϣ
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
                    
                case Button_4: // ����
                    s_storage_initialized = 0;
                    return 1;
            }
        }
        else // ��λѡ��
        {
            switch(key)
            {
                case Button_1: // ��һ����λ
                    s_storage_curr_item = (s_storage_curr_item > 0) ? (s_storage_curr_item - 1) : (STORAGE_SLOT_COUNT - 1);
                    draw_storage_menu(s_storage_curr_item, s_storage_state);
                    break;
                    
                case Button_2: // ��һ����λ
                    s_storage_curr_item = (s_storage_curr_item < STORAGE_SLOT_COUNT - 1) ? (s_storage_curr_item + 1) : 0;
                    draw_storage_menu(s_storage_curr_item, s_storage_state);
                    break;
                    
                case Button_3: // ȷ��
                    ips200_clear();
                    // ������ʾ��Ϣ
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
                    
                case Button_4: // ����
                    s_storage_state = STORAGE_MENU_MAIN;
                    s_storage_curr_item = 0;
                    draw_storage_menu(s_storage_curr_item, s_storage_state);
                    break;
            }
        }
    }
    
    return 0;
}

//===================================================���˵����===================================================

void parameter_menu_entry(void)
{
    ips200_clear();
    // ������ʾ������Ϣ
    uint16 msg_y = ips200_height_max / 2 - 10;
    ips200_show_string(STORAGE_LEFT_MARGIN, msg_y, "Loading parameters...");
    
    Parameter_flash_load();
    system_delay_ms(500);
    
    s_param_initialized = 0;
    s_params_modified = 0;  // �����޸ı��
    uint8 exit_flag = 0;
    key_flag = 0;
    
    // ���ݳ�ʼ����ֵ�����ڼ���Ƿ����޸�
    float initial_params[MAX_PARAMETERS];
    for(uint8 i = 0; i < parameter_count; i++)
        initial_params[i] = parameter[param_config[i].index];
    
    while(!exit_flag)
    {
        exit_flag = parameter_adjust_page();
        system_delay_ms(10);
    }
    
    // ����Ƿ��в������޸�
    for(uint8 i = 0; i < parameter_count; i++)
    {
        if(parameter[param_config[i].index] != initial_params[i])
        {
            s_params_modified = 1;
            break;
        }
    }
    
    // ����в������޸ģ���ʾ��ʾ
    if(s_params_modified)
    {
        ips200_clear();
        ips200_set_color(RGB565_GREEN, RGB565_BLACK);
        ips200_show_string(STORAGE_LEFT_MARGIN, msg_y, "Parameters Saved!");
        ips200_show_string(STORAGE_LEFT_MARGIN, msg_y + 20, "Returning to menu...");
        ips200_set_color(RGB565_WHITE, RGB565_BLACK);
        system_delay_ms(500);
    }
    
    // ���÷��ذ�����־���ò˵�ϵͳ֪����Ҫ�����ϼ��˵�
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
    
    // ���÷��ذ�����־���ò˵�ϵͳ֪����Ҫ�����ϼ��˵�
    key_flag = Button_4;
}

//===================================================�����������===================================================

// �����������Ƿ��г�ͻ
uint8 check_parameter_index_conflicts(void)
{
    uint8 index_used[MAX_PARAMETERS] = {0};  // ������飬��¼��Щ������ʹ��
    uint8 conflicts = 0;
    
    for(uint8 i = 0; i < parameter_count; i++)
    {
        uint8 idx = param_config[i].index;
        
        // ��������Ƿ񳬳���Χ
        if(idx >= MAX_PARAMETERS)
        {
            conflicts++;
            continue;
        }
        
        // ��������Ƿ��ظ�ʹ��
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

//===================================================�����鿴����===================================================

// ��̬���� for �����鿴
static uint8 s_view_curr_param = 0;
static uint8 s_view_initialized = 0;
static ParameterScheme s_current_viewing_scheme;

// ����������ȡ��������
uint8 get_scheme_by_index(uint8 index, ParameterScheme* scheme)
{
    if(index >= MAX_SCHEMES || scheme == NULL) return 0;
    
    load_scheme_list();  // ȷ�������б��Ѽ���
    
    if(!schemes[index].valid) return 0;
    
    // ���Ʒ�������
    memcpy(scheme, &schemes[index], sizeof(ParameterScheme));
    return 1;
}

// ��ʾ������ϸ��Ϣ
void view_scheme_details(uint8 scheme_index)
{
    if(!get_scheme_by_index(scheme_index, &s_current_viewing_scheme))
        return;
    
    ips200_clear();
    ips200_set_color(RGB565_CYAN, RGB565_BLACK);
    
    // ��ʾ��������
    char title[32];
    sprintf(title, "Scheme: %.12s", s_current_viewing_scheme.name);
    ips200_show_string(5, 5, title);
    
    // ��ʾ�ٶ�ֵ
    char speed_str[32];
    format_float_truncate(speed_str, s_current_viewing_scheme.speed_value, 8, 2);
    sprintf(title, "Speed: %s", speed_str);
    ips200_show_string(5, 25, title);
    
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    
    // ��ʾ�ָ���
    ips200_show_string(5, 45, "Parameters:");
    
    s_view_initialized = 1;
}

// ���Ʒ����鿴ҳ��Ĳ���
void draw_view_param_page(uint8 curr_param)
{
    if(!s_view_initialized) return;
    
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    
    // ���㵱ǰҳ��
    uint8 params_per_page = 8;  // ÿҳ��ʾ8������
    uint8 current_page = curr_param / params_per_page;
    uint8 total_pages = (parameter_count + params_per_page - 1) / params_per_page;
    uint8 start_param = current_page * params_per_page;
    uint8 end_param = start_param + params_per_page;
    if(end_param > parameter_count) end_param = parameter_count;
    
    // ���������ʾ����
    for(uint8 line = 0; line < params_per_page; line++)
    {
        uint16 y = 65 + line * 18;
        ips200_show_string(0, y, "                    ");  // �������
    }
    
    // ��ʾ��ǰҳ�Ĳ���
    for(uint8 i = start_param; i < end_param; i++)
    {
        uint8 line_index = i - start_param;
        uint16 y = 65 + line_index * 18;
        
        // ������ǰѡ�еĲ���
        if(i == curr_param)
        {
            ips200_set_color(RGB565_BLACK, RGB565_WHITE);
        }
        else
        {
            ips200_set_color(RGB565_WHITE, RGB565_BLACK);
        }
        
        // ��ʾ��������
        char name_str[20];
        sprintf(name_str, "%.18s", param_config[i].name);
        ips200_show_string(5, y, name_str);
        
        // ��ʾ����ֵ���ӷ����л�ȡ��
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
    
    // �ָ�������ɫ
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    
    // ��ʾҳ��ָʾ��
    char page_info[32];
    sprintf(page_info, "Page %d/%d", current_page + 1, total_pages);
    ips200_show_string(5, 210, page_info);
    
    // ��ʾ������ʾ
    ips200_show_string(5, 225, "U/D:Select  B4:Back");
}

// �����鿴ҳ��������
uint8 view_scheme_page(uint8 scheme_index)
{
    // ��ʼ��
    if(!s_view_initialized)
    {
        if(!get_scheme_by_index(scheme_index, &s_current_viewing_scheme))
        {
            ips200_clear();
            ips200_show_string(50, 100, "Scheme not found!");
            system_delay_ms(1000);
            return 1;  // �����ϼ��˵�
        }
        
        view_scheme_details(scheme_index);
        s_view_curr_param = 0;
        draw_view_param_page(s_view_curr_param);
    }
    
    // ������
    if(key_flag)
    {
        uint8 key = key_flag;
        key_flag = 0;
        
        switch(key)
        {
            case Button_1: // ��һ������
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
                
            case Button_2: // ��һ������
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
                
            case Button_3: // ��ʱ�޹��ܣ����Ժ�����չ
                break;
                
            case Button_4: // ����
                s_view_initialized = 0;
                return 1;
        }
    }
    
    return 0;
} 
