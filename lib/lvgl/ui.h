
#ifndef UI_H
#define UI_H

#include <lvgl.h>

// UI管理函数
void create_red_screen();
void create_ui();
void hide_ui();
bool ui_is_visible();

// 外部可访问的UI对象
// extern lv_obj_t* ui_root_container;

#endif