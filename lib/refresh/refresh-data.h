
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
void lvgl_custom_handler();    //进入lvgl控制, 此函数检测红屏标记值, 如果不处于红屏状态, 调用 lv_timer_handler(); 
void clear_gesture(); //退出红屏状态, 并切换到menu屏幕
bool is_show_gesture();    //返回是否处于红屏状态


#ifdef __cplusplus
} /*extern "C"*/
#endif

void refresh_mavlink_data();
void refresh_mpu_data();

void refresh_p3d_data();

#endif