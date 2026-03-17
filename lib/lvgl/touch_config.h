

#ifndef _TOUCH_CONFIG_H_FILE
#define _TOUCH_CONFIG_H_FILE


#define  TOUCH_SLIDE_COOLDOWN_TIME 50   // 3/13 MiluoOffical. : 标记100ms触摸冷却期的变量. 当检测到触摸信号后, 立刻进入冷却禁止刷屏, 避免刷屏信号干扰触摸检测和手势检测
#define  GESTURE_DISTANCE_PIXEL    80   // 3/13 MiluoOffical. : 滑动手势检测: 滑动超过该像素数的距离之后才被视为滑动操作, 太小的话更容易误触. 原始值为 100 像素.

// #define  USE_DcsBios                1 //使用DcsBios的话 , 取消注释这里

#endif