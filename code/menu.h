#ifndef __MENU_H__
#define __MENU_H__

#include "zf_common_headfile.h"
#include "key.h"

// 菜单项最大数量
#define MAX_MENU_ITEMS 10
// 菜单名称最大长度
#define MAX_MENU_NAME_LEN 20

// 菜单项结构体
typedef struct MenuItem {
    char name[MAX_MENU_NAME_LEN];          // 菜单项名称
    void (*function)(void);                // 关联的函数指针
    struct Menu* subMenu;                  // 指向子菜单的指针
} MenuItem;

// 菜单结构体
typedef struct Menu {
    char title[MAX_MENU_NAME_LEN];         // 菜单标题
    MenuItem items[MAX_MENU_ITEMS];        // 菜单项数组
    uint8 itemCount;                       // 菜单项数量
    uint8 currentSelection;                // 当前选中项的索引
    struct Menu* parentMenu;               // 指向父菜单的指针
} Menu;

// 初始化菜单系统
void menu_init(void);

// 显示菜单
void menu_display(Menu* menu);

// 处理菜单逻辑（通常在主循环中调用）
Menu* menu_process(Menu* currentMenu);

// 更新菜单选中项的显示（例如高亮）
void menu_update_selection(Menu* menu, uint8 old_selection, uint8 new_selection);

// 处理按键事件
void menu_key_handler(Menu* menu, uint8 key);

// 创建菜单项
void menu_create_item(Menu* menu, const char* name, void (*function)(void), Menu* subMenu);

// 设置当前菜单
void menu_set_current(Menu* menu);

// 获取当前菜单
Menu* menu_get_current(void);

// 进入子菜单
Menu* menu_enter_submenu(Menu* currentMenu);

// 返回父菜单
Menu* menu_return_parent(Menu* currentMenu);

// 执行当前选中项的函数
void menu_execute_function(Menu* menu);

// 退出菜单
void menu_exit(void);

// 重置退出标志
void menu_reset_exit_flag(void);

extern Menu* g_currentMenu;  // 当前活动菜单指针
extern uint8 g_menu_exit_flag;  // 菜单退出标志，1表示退出

#endif /* __MENU_H__ */