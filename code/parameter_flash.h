#ifndef __PARAMETER_FLASH_H__
#define __PARAMETER_FLASH_H__

#include "zf_common_headfile.h"

#define PARAMETER_INDEX 1
#define MAX_PARAMETERS 20

// �������ýṹ��
typedef struct {
    const char* name;       // ��������
    uint8 index;           // �����������е�����λ��
    float* target_var;      // Ŀ�����ָ��
    float default_value;    // Ĭ��ֵ
    float min_value;        // ��Сֵ
    float max_value;        // ���ֵ
    float step;            // ��������
} ParameterConfig;

// ��������״̬
typedef enum {
    PARAM_STATE_VIEW,       // �鿴״̬
    PARAM_STATE_EDIT        // �༭״̬
} ParameterState;

// �洢�˵�״̬
typedef enum {
    STORAGE_MENU_MAIN,      // ���˵�
    STORAGE_MENU_SAVE,      // �����λѡ��
    STORAGE_MENU_LOAD       // ���ز�λѡ��
} StorageMenuState;

// ��������״̬
typedef enum {
    SCHEME_MENU_MAIN,       // ���˵�
    SCHEME_MENU_DELETE,     // ɾ������
    SCHEME_MENU_LOAD,       // ���ط���
    SCHEME_MENU_VIEW        // �鿴��������
} SchemeMenuState;

// �������ݽṹ
#define MAX_SCHEMES 10
#define SCHEME_NAME_LEN 16
#define SCHEME_FLASH_BASE_INDEX 3  // ������flash�е���ʼ����

typedef struct {
    char name[SCHEME_NAME_LEN];     // ��������
    float speed_value;              // �ٶ�ֵ�������Զ�������
    float parameters[MAX_PARAMETERS]; // ����ֵ
    uint8 valid;                    // �����Ƿ���Ч
} ParameterScheme;

// �洢��λ����
#define STORAGE_SLOT_COUNT 5
#define STORAGE_SLOT_DEFAULT 0

// �ⲿ����
extern float parameter[MAX_PARAMETERS];
extern ParameterConfig param_config[];
extern uint8 parameter_count;

// ������������
void Parameter_flash_load(void);
void Parameter_flash_save(void);
void Parameter_flash_load_slot(uint8 slot);
void Parameter_flash_save_slot(uint8 slot);
void Parameter_reset_to_default(void);

// ��������ҳ��
uint8 parameter_adjust_page(void);

// �洢�˵�
uint8 storage_menu_page(void);

// ���˵����
void parameter_menu_entry(void);
void storage_menu_entry(void);

// ��λ����
uint8 storage_slot_exists(uint8 slot);

// ����������
void scheme_management_entry(void);
uint8 scheme_management_page(void);
void save_current_parameters_as_scheme(void);
void load_scheme_list(void);
void save_scheme_list(void);
uint8 get_scheme_count(void);
void delete_scheme(uint8 index);
void load_scheme_parameters(uint8 index);
void generate_scheme_name(char* name_buffer, float speed_value);

// �����鿴����
uint8 view_scheme_page(uint8 scheme_index);
void view_scheme_details(uint8 scheme_index);
uint8 get_scheme_by_index(uint8 index, ParameterScheme* scheme);

#endif

