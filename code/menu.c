#include "menu.h"
#include "zf_device_ips200.h"

// ????????
Menu* g_currentMenu = NULL;
// ?????????
uint8 g_menu_exit_flag = 0;

// ??????????
#define MENU_START_X       10
#define MENU_START_Y       40
#define MENU_ITEM_HEIGHT   20
#define MENU_ITEM_WIDTH    200
#define MENU_TITLE_Y       10

// ?????????
#define MENU_BG_COLOR      RGB565_BLACK
#define MENU_TEXT_COLOR    RGB565_WHITE
#define MENU_SELECT_COLOR  RGB565_BLUE
#define MENU_TITLE_COLOR   RGB565_YELLOW

// ??????????
void menu_init(void)
{
    // ??????
    ips200_clear();

    // ????????????
    ips200_set_dir(IPS200_PORTAIT);
    ips200_set_font(IPS200_8X16_FONT);
    ips200_set_color(MENU_TEXT_COLOR, MENU_BG_COLOR);
}

// ??????
void menu_display(Menu* menu)
{
    uint8 i;

    if (menu == NULL)
        return;

    // ??????
    ips200_clear();

    // ??????????
    ips200_set_color(MENU_TITLE_COLOR, MENU_BG_COLOR);
    ips200_show_string(MENU_START_X, MENU_TITLE_Y, menu->title);

    // ????????
    for (i = 0; i < menu->itemCount; i++)
    {
        // ?????????????
        if (i == menu->currentSelection)
        {
            ips200_set_color(MENU_SELECT_COLOR, MENU_BG_COLOR);
            // ???????????
            ips200_draw_line(MENU_START_X - 5, MENU_START_Y + i * MENU_ITEM_HEIGHT,
                            MENU_START_X + MENU_ITEM_WIDTH, MENU_START_Y + i * MENU_ITEM_HEIGHT, MENU_SELECT_COLOR);
            ips200_draw_line(MENU_START_X - 5, MENU_START_Y + (i+1) * MENU_ITEM_HEIGHT,
                            MENU_START_X + MENU_ITEM_WIDTH, MENU_START_Y + (i+1) * MENU_ITEM_HEIGHT, MENU_SELECT_COLOR);
            ips200_draw_line(MENU_START_X - 5, MENU_START_Y + i * MENU_ITEM_HEIGHT,
                            MENU_START_X - 5, MENU_START_Y + (i+1) * MENU_ITEM_HEIGHT, MENU_SELECT_COLOR);
            ips200_draw_line(MENU_START_X + MENU_ITEM_WIDTH, MENU_START_Y + i * MENU_ITEM_HEIGHT,
                            MENU_START_X + MENU_ITEM_WIDTH, MENU_START_Y + (i+1) * MENU_ITEM_HEIGHT, MENU_SELECT_COLOR);
        }
        else
        {
            ips200_set_color(MENU_TEXT_COLOR, MENU_BG_COLOR);
        }

        // ????????????
        ips200_show_string(MENU_START_X, MENU_START_Y + i * MENU_ITEM_HEIGHT, menu->items[i].name);
    }

    // ?????????
    ips200_set_color(MENU_TEXT_COLOR, MENU_BG_COLOR);
}

// ?????????????????????????
Menu* menu_process(Menu* currentMenu)
{
    Menu* menu = currentMenu;

    // ?????????????????
    if (g_menu_exit_flag)
    {
        return NULL;  // ????NULL???????????
    }

    // ???????§Ñ???
    if (key_flag != 0)
    {
        // ????????
        menu_key_handler(menu, key_flag);

        // ??????????
        key_flag = 0;
    }

    return g_currentMenu;
}

// ???????????????
// ??????????????????????????????
void menu_update_selection(Menu* menu, uint8 old_selection, uint8 new_selection)
{
    if (menu == NULL)
        return;

    // ????????????????
    ips200_set_color(MENU_TEXT_COLOR, MENU_BG_COLOR);
    // ?????????????
    ips200_draw_line(MENU_START_X - 5, MENU_START_Y + old_selection * MENU_ITEM_HEIGHT,
                    MENU_START_X + MENU_ITEM_WIDTH, MENU_START_Y + old_selection * MENU_ITEM_HEIGHT, MENU_BG_COLOR);
    ips200_draw_line(MENU_START_X - 5, MENU_START_Y + (old_selection+1) * MENU_ITEM_HEIGHT,
                    MENU_START_X + MENU_ITEM_WIDTH, MENU_START_Y + (old_selection+1) * MENU_ITEM_HEIGHT, MENU_BG_COLOR);
    ips200_draw_line(MENU_START_X - 5, MENU_START_Y + old_selection * MENU_ITEM_HEIGHT,
                    MENU_START_X - 5, MENU_START_Y + (old_selection+1) * MENU_ITEM_HEIGHT, MENU_BG_COLOR);
    ips200_draw_line(MENU_START_X + MENU_ITEM_WIDTH, MENU_START_Y + old_selection * MENU_ITEM_HEIGHT,
                    MENU_START_X + MENU_ITEM_WIDTH, MENU_START_Y + (old_selection+1) * MENU_ITEM_HEIGHT, MENU_BG_COLOR);

    // ????????????????
    ips200_show_string(MENU_START_X, MENU_START_Y + old_selection * MENU_ITEM_HEIGHT, menu->items[old_selection].name);

    // ????????????????
    ips200_set_color(MENU_SELECT_COLOR, MENU_BG_COLOR);
    // ?????????????
    ips200_draw_line(MENU_START_X - 5, MENU_START_Y + new_selection * MENU_ITEM_HEIGHT,
                    MENU_START_X + MENU_ITEM_WIDTH, MENU_START_Y + new_selection * MENU_ITEM_HEIGHT, MENU_SELECT_COLOR);
    ips200_draw_line(MENU_START_X - 5, MENU_START_Y + (new_selection+1) * MENU_ITEM_HEIGHT,
                    MENU_START_X + MENU_ITEM_WIDTH, MENU_START_Y + (new_selection+1) * MENU_ITEM_HEIGHT, MENU_SELECT_COLOR);
    ips200_draw_line(MENU_START_X - 5, MENU_START_Y + new_selection * MENU_ITEM_HEIGHT,
                    MENU_START_X - 5, MENU_START_Y + (new_selection+1) * MENU_ITEM_HEIGHT, MENU_SELECT_COLOR);
    ips200_draw_line(MENU_START_X + MENU_ITEM_WIDTH, MENU_START_Y + new_selection * MENU_ITEM_HEIGHT,
                    MENU_START_X + MENU_ITEM_WIDTH, MENU_START_Y + (new_selection+1) * MENU_ITEM_HEIGHT, MENU_SELECT_COLOR);

    // ??????????????
    ips200_show_string(MENU_START_X, MENU_START_Y + new_selection * MENU_ITEM_HEIGHT, menu->items[new_selection].name);

    // ?????????
    ips200_set_color(MENU_TEXT_COLOR, MENU_BG_COLOR);
}

void menu_key_handler(Menu* menu, uint8 key)
{
    if (menu == NULL)
        return;

    uint8 old_selection = menu->currentSelection;

    switch (key)
    {
        case Button_1:  // ??
            if (menu->currentSelection > 0)
                menu->currentSelection--;
            else
                menu->currentSelection = menu->itemCount - 1;
            menu_update_selection(menu, old_selection, menu->currentSelection);
            break;

        case Button_2:  // ??
            if (menu->currentSelection < menu->itemCount - 1)
                menu->currentSelection++;
            else
                menu->currentSelection = 0;
            menu_update_selection(menu, old_selection, menu->currentSelection);
            break;

        case Button_3:  // ???
            menu_execute_function(menu);
            break;

        case Button_4:  // ????
            g_currentMenu = menu_return_parent(menu);
            break;

        default:
            break;
    }
}

// ?????????
void menu_create_item(Menu* menu, const char* name, void (*function)(void), Menu* subMenu)
{
    if (menu == NULL || menu->itemCount >= MAX_MENU_ITEMS)
        return;

    MenuItem* item = &menu->items[menu->itemCount];

    // ????????????
    strncpy(item->name, name, MAX_MENU_NAME_LEN-1);
    item->name[MAX_MENU_NAME_LEN-1] = '\0';  // ????????????

    // ???¨´????????
    item->function = function;
    item->subMenu = subMenu;

    // ????????????????Íè???
    if (subMenu != NULL)
    {
        subMenu->parentMenu = menu;
    }

    // ????????????
    menu->itemCount++;
}

// ???????????
void menu_set_current(Menu* menu)
{
    g_currentMenu = menu;
    menu_display(menu);
}

// ???????????
Menu* menu_get_current(void)
{
    return g_currentMenu;
}

// ????????
Menu* menu_enter_submenu(Menu* currentMenu)
{
    if (currentMenu == NULL)
        return NULL;

    MenuItem* item = &currentMenu->items[currentMenu->currentSelection];

    if (item->subMenu != NULL)
    {
        // ????????
        g_currentMenu = item->subMenu;
        g_currentMenu->currentSelection = 0;  // ???????????????
        menu_display(g_currentMenu);
        return g_currentMenu;
    }

    return currentMenu;
}

// ????????
Menu* menu_return_parent(Menu* currentMenu)
{
    if (currentMenu == NULL || currentMenu->parentMenu == NULL)
        return currentMenu;

    // ??????????
    g_currentMenu = currentMenu->parentMenu;
    menu_display(g_currentMenu);
    return g_currentMenu;
}

// ?????§Ö???????
void menu_execute_function(Menu* menu)
{
    if (menu == NULL)
        return;

    MenuItem* item = &menu->items[menu->currentSelection];

    // ?????????????
    if (item->subMenu != NULL)
    {
        // ????????
        g_currentMenu = menu_enter_submenu(menu);
    }
    else if (item->function != NULL)
    {
        // ????????????????
        ips200_clear();

        // ???????????????????
        ips200_set_color(MENU_TITLE_COLOR, MENU_BG_COLOR);
        ips200_show_string(20, 20, item->name);

        // ??????????
        ips200_set_color(MENU_TEXT_COLOR, MENU_BG_COLOR);
        ips200_show_string(20, 40, "Press BACK to return");

        // ??????????
        while (1)
        {
            // ??§Û??????
            item->function();

            // ??????????????????
            if (g_menu_exit_flag)
            {
                // ???????????????????????
                break;
            }

            // ????? - ??????¡¤????(Button_4)????????
            if (key_flag == Button_4)
            {
                // ??????????
                key_flag = 0;
                // ??????????????
                break;
            }

            // ?????????????CPU???
            system_delay_ms(50);
        }

        // ???????????????????????????????
        if (!g_menu_exit_flag)
        {
            // ??????????
            menu_display(menu);
        }
    }
}

// ????????
void menu_exit(void)
{
    // ??????????
    g_menu_exit_flag = 1;
}

// ???¨°????????
void menu_reset_exit_flag(void)
{
    g_menu_exit_flag = 0;
}