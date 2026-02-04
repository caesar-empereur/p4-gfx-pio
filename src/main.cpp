#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#define DCSBIOS_DEFAULT_SERIAL

#include "DcsBios.h"

// #include "radar-old.h"
#include "gesture-rgb.h"
#include "receive-mpu.h"
#include "receive-mavlink.h"
#include "rgb-init.h"
// #include "spi-init.h"
// #include "lvgl-display-init.h"
#include "lvgl-display-init.h"

// #include "gt911.h"
// #include <lvgl.h>
// #include "lv_conf.h"
// #include <demos/lv_demos.h>

#include "ui.h"
#include <esp_log.h>
extern Arduino_DSI_Display *gfx; //显示句柄
extern esp_lcd_touch_handle_t tp_handle; //触摸句柄


// #define PIN_NUM_LCD_CS     46
// #define PIN_NUM_LCD_PCLK   16
// #define PIN_NUM_LCD_DATA0  17
// #define PIN_NUM_LCD_DATA1  18
// #define PIN_NUM_LCD_DATA2  8
// #define PIN_NUM_LCD_DATA3  3
// #define PIN_NUM_LCD_RST    9
ges_data_t ges_data;


#define DCSBIOS_DEFAULT_SERIAL

// #include "DcsBios.h"

String inputString = "";
bool stringComplete = false;


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
    // MySerial.begin(115200, SERIAL_8N1, 37, 38);


    
    // Serial1.begin(115200);
    // Serial2.begin(115200);
    

    lvgl_display_init();
    ui_init();

    initDisplay();

    showRedScreen();
    gestureInit(0,0);

    

    // mpu_init();
    // gfx_b->begin();
    // gfx_b->fillRect(0,0,480, 240, BLUE);
    // gfx_b->fillRect(0,240,480, 240, GREEN);

    // DcsBios::setup();
    Serial.println("Setup complete");
    Serial.begin(115200);
}

 void loop() {
    /**
     * drawGestureByData  这里是用 Arduino gfx 库显示的
     * 这里 改为简单  的 gfx->fillScreen(RED);
     * 屏幕底部有上滑动作的时候，出现那个配置界面
     * 配置界面返回的时候, 又回到这个给  gfx->fillScreen(RED) 的界面
     */
    // drawGestureByData(2, ges_data, 0, 0);

    /**
     * drawGestureByData  这里是用 Arduino gfx 库显示的
     * 这里 改为简单  的 gfx->fillScreen(RED);
     * 屏幕底部有上滑动作的时候，出现那个配置界面
     * 配置界面返回的时候, 又回到这个给  gfx->fillScreen(RED) 的界面
     */
// -------------------- By MiluoOffical 2026.02.02 --------------

    //检测到红屏为false时, lvgl需要及时接管.
    if( isRedScreen() == true ){ //当前是红屏状态, 持续检测手势, 监测到上滑手势时标记 redScreen_status 为false, 下个循环将会自动进入LVGL部分
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
                clearRedScreen(); //退出红屏状态
                ESP_LOGI("TOUCH", "Red screen cleared.");
            }
            lastPressed = false;
        }
    }

    lvglHandler();
    delay(5);
}




String getStringBetween(String data, String startStr, String endStr) {
    int startIndex = data.indexOf(startStr);
    if (startIndex == -1) return "";  // 起始字符串未找到
    
    startIndex += startStr.length();  // 移动到起始字符串之后
    
    int endIndex = data.indexOf(endStr, startIndex);
    if (endIndex == -1) return "";    // 结束字符串未找到
    
    return data.substring(startIndex, endIndex);
}

void serial_read_p3d(){
   // while(Serial.available()>0) {
    //     uint8_t c = Serial.read();
    //     Serial2.write(c);
    // }
    while(Serial.available()>0) {
        char inChar = (char)Serial.read();
        // Serial2.print(inChar);
        if (inChar == '/') {
          stringComplete = true;
        } else {
          inputString += inChar;
        }
        
    }
    
    if(stringComplete){
        
        String altitude_str = getStringBetween(inputString, "altitude:", ",");
        String bank_str = getStringBetween(inputString, "bank:", ",");
        String pitch_str = getStringBetween(inputString, "pitch:", ",");
        String air_speed = getStringBetween(inputString, "air_speed:", ",");
        String heading_str = getStringBetween(inputString, "heading:", ",");

        ges_data.roll = bank_str.toInt();
        ges_data.pitch = pitch_str.toInt();
        ges_data.yaw = heading_str.toInt();
        ges_data.altitude = altitude_str.toInt();
        ges_data.air_speed = air_speed.toInt();
        // tft.fillScreen(TFT_BLACK);
        // tft.drawString("altitude:" + altitude_str, 0, 0, 4);
        // tft.drawString("bank:" + bank_str, 0, 50, 4);
        // tft.drawString("pitch:" + pitch_str, 0, 100, 4);
        // tft.drawString("heading:" + heading_str, 0, 150, 4);

        inputString = "";
    }
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
    serial_read_dcs();
}


void serialEvent2(){
    ges_data = receive_parse_mpu();
}

// void serialEvent1(){
//     ges_data = receive_parse_mpu();
// }

