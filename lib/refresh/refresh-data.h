
#ifndef REFRESH_DATA_H
#define REFRESH_DATA_H

// #include "gesture-rgb.h"
// #include "receive-mpu.h"
// #include "receive-mavlink.h"

#ifdef __cplusplus
extern "C" {
#endif


// -------------------- By MiluoOffical 2026.02.02 --------------
void show_gesture();  //显示红屏, 并标记红屏状态为true, - 函数定义
// void lvgl_custom_handler();    //进入lvgl控制, 此函数检测红屏标记值, 如果不处于红屏状态, 调用 lv_timer_handler(); 
void return_to_lvgl(); //退出红屏状态, 并切换到menu屏幕
void update_gfx_screen();
void ges_show_handler();       //进入lvgl控制, 此函数检测红屏标记值, 如果不处于红屏状态, 调用 lv_timer_handler(); 
extern int ges_show_type;


#ifdef __cplusplus
} /*extern "C"*/
#endif

#define GFX_SCREENS    3   //屏幕数, 当前示例就是红屏 绿屏 蓝屏 三个
#define USE_LOOP_SLIDE 0   //是否允许连续滑屏 (屏幕3→屏幕1) 这样

void refresh_mavlink_data();
void refresh_mpu_data();

void refresh_p3d_data();

#endif