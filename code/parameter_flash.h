#ifndef __PARAMETER_FLASH_H__
#define __PARAMETER_FLASH_H__

#include "zf_common_headfile.h"

#define PARAMETER_INDEX 1
#define MAX_PARAMETERS 20

// 参数配置结构体
typedef struct {
    const char* name;       // 参数名称
    uint8 index;           // 参数在数组中的索引位置
    float* target_var;      // 目标变量指针
    float default_value;    // 默认值
    float min_value;        // 最小值
    float max_value;        // 最大值
    float step;            // 调整步长
} ParameterConfig;

// 参数调整状态
typedef enum {
    PARAM_STATE_VIEW,       // 查看状态
    PARAM_STATE_EDIT        // 编辑状态
} ParameterState;

// 存储菜单状态
typedef enum {
    STORAGE_MENU_MAIN,      // 主菜单
    STORAGE_MENU_SAVE,      // 保存槽位选择
    STORAGE_MENU_LOAD       // 加载槽位选择
} StorageMenuState;

// 方案管理状态
typedef enum {
    SCHEME_MENU_MAIN,       // 主菜单
    SCHEME_MENU_DELETE,     // 删除方案
    SCHEME_MENU_LOAD,       // 加载方案
    SCHEME_MENU_VIEW        // 查看方案详情
} SchemeMenuState;

// 方案数据结构
#define MAX_SCHEMES 10
#define SCHEME_NAME_LEN 16
#define SCHEME_FLASH_BASE_INDEX 3  // 方案在flash中的起始索引

typedef struct {
    char name[SCHEME_NAME_LEN];     // 方案名称
    float speed_value;              // 速度值（用于自动命名）
    float parameters[MAX_PARAMETERS]; // 参数值
    uint8 valid;                    // 方案是否有效
} ParameterScheme;

// 存储槽位定义
#define STORAGE_SLOT_COUNT 5
#define STORAGE_SLOT_DEFAULT 0

// 外部变量
extern float parameter[MAX_PARAMETERS];
extern ParameterConfig param_config[];
extern uint8 parameter_count;

// 基础参数操作
void Parameter_flash_load(void);
void Parameter_flash_save(void);
void Parameter_flash_load_slot(uint8 slot);
void Parameter_flash_save_slot(uint8 slot);
void Parameter_reset_to_default(void);

// 参数调整页面
uint8 parameter_adjust_page(void);

// 存储菜单
uint8 storage_menu_page(void);

// 主菜单入口
void parameter_menu_entry(void);
void storage_menu_entry(void);

// 槽位管理
uint8 storage_slot_exists(uint8 slot);

// 方案管理功能
void scheme_management_entry(void);
uint8 scheme_management_page(void);
void save_current_parameters_as_scheme(void);
void load_scheme_list(void);
void save_scheme_list(void);
uint8 get_scheme_count(void);
void delete_scheme(uint8 index);
void load_scheme_parameters(uint8 index);
void generate_scheme_name(char* name_buffer, float speed_value);

// 方案查看功能
uint8 view_scheme_page(uint8 scheme_index);
void view_scheme_details(uint8 scheme_index);
uint8 get_scheme_by_index(uint8 index, ParameterScheme* scheme);

#endif

