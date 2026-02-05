#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#define DCSBIOS_DEFAULT_SERIAL

#include "DcsBios.h"

// #include "radar-old.h"
#include "gesture-rgb.h"
#include "receive-mpu.h"
#include "receive-mavlink.h"
#include "rgb-init.h"
#include "lvgl-display-init.h"
#include "refresh-data.h"

#include "ui.h"
#include <esp_log.h>
extern Arduino_DSI_Display *gfx; //显示句柄
extern esp_lcd_touch_handle_t tp_handle; //触摸句柄


ges_data_t ges_data;


#define DCSBIOS_DEFAULT_SERIAL

// #include "DcsBios.h"




// HardwareSerial MySerial(1);

void F18_SAI_BANK(unsigned int newValue) {
    float fix_value = (newValue  / 65535.0f * 360) - 180;
    int roll = static_cast<int>(fix_value);
    ges_data.roll = roll;
}
DcsBios::IntegerBuffer F18_SAI_BANK_FUNC(FA_18C_hornet_SAI_BANK, F18_SAI_BANK);

void F18_SAI_PITCH(unsigned int newValue) {
    float fix_value = (newValue / 65535.0f * 180.0f) -90.0f;
    int pitch = static_cast<int>(fix_value);
    ges_data.pitch = pitch;
}
DcsBios::IntegerBuffer F18_SAI_PITCH_FUNC(FA_18C_hornet_SAI_PITCH, F18_SAI_PITCH);

void F18_SBY_COMPASS_HDG(unsigned int newValue) {

    float fix_value = (newValue * 360 / 65535.0f);
    int yaw = static_cast<int>(fix_value);
    if(abs(yaw)>=360){
      yaw = 0;
    }
    ges_data.yaw = yaw;
}
DcsBios::IntegerBuffer F18_SBY_COMPASS_HDG_FUNC(FA_18C_hornet_SBY_COMPASS_HDG, F18_SBY_COMPASS_HDG);

void F18_STBY_ASI_AIRSPEED(unsigned int newValue) {
  int fix_value = (newValue * 350 / 65535);
  ges_data.air_speed=abs(int(newValue/150));
}
DcsBios::IntegerBuffer F18_STBY_ASI_AIRSPEED_FUNC(FA_18C_hornet_STBY_ASI_AIRSPEED, F18_STBY_ASI_AIRSPEED);

void F18_PRESSURE_ALT(unsigned int newValue) {
  ges_data.altitude = newValue*2;
}
DcsBios::IntegerBuffer F18_PRESSURE_ALT_FUNC(FA_18C_hornet_PRESSURE_ALT, F18_PRESSURE_ALT);

// void F18_STBY_PRESS_ALT(unsigned int newValue) {
//   float fix_value = (newValue * 360 / 65535);
//   ges_data.altitude = newValue;
// }
// DcsBios::IntegerBuffer F18_STBY_PRESS_ALT_FUNC(FA_18C_hornet_STBY_PRESS_ALT, F18_STBY_PRESS_ALT);



 void setup() {

    
    // Serial1.begin(115200);
    // Serial2.begin(115200);
    

    lvgl_display_init();
    ui_init();

    initDisplay();

    show_gesture();
    gestureInit(0,0);

    

    // mpu_init();

    // DcsBios::setup();
    Serial.println("Setup complete");
    Serial.begin(115200);
}

 void loop() {


    //检测到红屏为false时, lvgl需要及时接管.
    if( ges_show_type != 0 ){ //当前是红屏状态, 持续检测手势, 监测到上滑手势时标记 redScreen_status 为false, 下个循环将会自动进入LVGL部分
    //检测到上滑动作时, 进入lvgl控制, 红屏状态值标记为false.
        esp_lcd_touch_read_data(tp_handle);
        static uint16_t first_touch_x = 0xffffu;
        static uint16_t first_touch_y = 0xffffu;
        static uint16_t last_touch_x = 0xffffu;
        static uint16_t last_touch_y = 0xffffu;
        static bool lastPressed = false;
        uint16_t touch_x;
        uint16_t touch_y;
        uint8_t touch_cnt;
        bool pressed = esp_lcd_touch_get_coordinates(
        tp_handle, &touch_x, &touch_y, NULL, &touch_cnt, (uint8_t )1);
    //检测方法: 在红屏状态下, 检测手指刚放在屏幕时的触摸点和手指离开的触摸点, 分析这两个点的坐标位置
        if(pressed==true){
            if(lastPressed==false){ //手指刚放在屏幕上, 记录触摸点
                first_touch_x = touch_x;
                first_touch_y = touch_y;
                ESP_LOGI("TOUCH", "Pressed at (%d, %d)", touch_x, touch_y);
                lastPressed = true;
            }
            last_touch_x = touch_x; //只要触摸没断触, 就持续记录触摸点
            last_touch_y = touch_y;
        }
        if(pressed==false && lastPressed==true){ //如果移动方位角在-45°~45°之间, 那么视为向上滑动
            int32_t dx = ((int32_t)last_touch_x-(int32_t)first_touch_x);
            int32_t dy = ((int32_t)last_touch_y-(int32_t)first_touch_y);
            int32_t distance_sq = (dx*dx+dy*dy);
            float angle = atan2f(dx,dy) * 180 / 3.14159265f;
            ESP_LOGI("TOUCH", "Released at (%" PRIu16 ", %" PRIu16 "), Distant is sqrt(%" PRId32 "), Angle is (%.3lf)",\
                last_touch_x, last_touch_y, distance_sq, angle);

            //参考阈值: 距离 10000 (100像素), 角度 -45~45
            if(distance_sq >= 10000 && angle > -45 &&  angle < 45 ){ //满足上滑动作的阈值
                clear_gesture(); //退出红屏状态
                ESP_LOGI("TOUCH", "Red screen cleared.");
            }
            if(distance_sq >= 10000 && angle > 45 &&  angle < 135 ){ //满足右滑动作的阈值
                if(ges_show_type < GFX_SCREENS) ges_show_type ++; //这两行代码表示需要切屏, 切到下一个屏幕
#if USE_LOOP_SLIDE
                else gfxScreenStatus = 1;
#endif
                clear_gfx_screen(); //调用该函数之后将在下一个 displayHandler() 调用时全屏刷新
                ESP_LOGI("TOUCH", "right slide.");
            }
            if(distance_sq >= 10000 && angle > -135 &&  angle < -45 ){ //满足左滑动作的阈值
                if(ges_show_type > 1) ges_show_type --; //这两行代码表示需要切屏, 切到上一个屏幕
#if USE_LOOP_SLIDE
                else ges_show_type = GFX_SCREENS;
#endif
                clear_gfx_screen(); //调用该函数之后将在下一个 displayHandler() 调用时全屏刷新
                ESP_LOGI("TOUCH", "left slide.");
            }
            lastPressed = false;
        }
    }
    ges_show_handler(); //进入lvgl控制, 此函数检测红屏标记值, 如果不处于红屏状态, 调用 lv_timer_handler(); 
    delay(5);
}






using namespace DcsBios;
ProtocolParser parser1;

void serial_read_dcs(){
    using namespace DcsBios;
    while (Serial.available()) {
      parser1.processChar(Serial.read());
    }
    PollingInput::pollInputs();
    ExportStreamListener::loopAll();
}

void serialEvent(){
    refresh_p3d_data();
}


// void serialEvent2(){
//     ges_data = receive_parse_mpu();
// }

// void serialEvent1(){
//     ges_data = receive_parse_mpu();
// }

