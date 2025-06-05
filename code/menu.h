#ifndef __MENU_H__
#define __MENU_H__

#include "zf_common_headfile.h"
#include "key.h"

// �˵����������
#define MAX_MENU_ITEMS 10
// �˵�������󳤶�
#define MAX_MENU_NAME_LEN 20

// �˵���ṹ��
typedef struct MenuItem {
    char name[MAX_MENU_NAME_LEN];          // �˵�������
    void (*function)(void);                // �����ĺ���ָ��
    struct Menu* subMenu;                  // ָ���Ӳ˵���ָ��
} MenuItem;

// �˵��ṹ��
typedef struct Menu {
    char title[MAX_MENU_NAME_LEN];         // �˵�����
    MenuItem items[MAX_MENU_ITEMS];        // �˵�������
    uint8 itemCount;                       // �˵�������
    uint8 currentSelection;                // ��ǰѡ���������
    struct Menu* parentMenu;               // ָ�򸸲˵���ָ��
} Menu;

// ��ʼ���˵�ϵͳ
void menu_init(void);

// ��ʾ�˵�
void menu_display(Menu* menu);

// ����˵��߼���ͨ������ѭ���е��ã�
Menu* menu_process(Menu* currentMenu);

// ���²˵�ѡ�������ʾ�����������
void menu_update_selection(Menu* menu, uint8 old_selection, uint8 new_selection);

// �������¼�
void menu_key_handler(Menu* menu, uint8 key);

// �����˵���
void menu_create_item(Menu* menu, const char* name, void (*function)(void), Menu* subMenu);

// ���õ�ǰ�˵�
void menu_set_current(Menu* menu);

// ��ȡ��ǰ�˵�
Menu* menu_get_current(void);

// �����Ӳ˵�
Menu* menu_enter_submenu(Menu* currentMenu);

// ���ظ��˵�
Menu* menu_return_parent(Menu* currentMenu);

// ִ�е�ǰѡ����ĺ���
void menu_execute_function(Menu* menu);

// �˳��˵�
void menu_exit(void);

// �����˳���־
void menu_reset_exit_flag(void);

extern Menu* g_currentMenu;  // ��ǰ��˵�ָ��
extern uint8 g_menu_exit_flag;  // �˵��˳���־��1��ʾ�˳�

#endif /* __MENU_H__ */