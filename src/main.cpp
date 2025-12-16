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
    // initDisplay();
    // gestureInit(0,0);

    lvgl_display_init();

    // mpu_init();
    // gfx_b->begin();
    // gfx_b->fillRect(0,0,480, 240, BLUE);
    // gfx_b->fillRect(0,240,480, 240, GREEN);

    // DcsBios::setup();
    Serial.println("Setup complete");
    Serial.begin(115200);
}

void loop() {
    // DcsBios::loop();
  // put your main code here, to run repeatedly:
    // drawGestureByData(2, ges_data, 0, 0);

    lv_timer_handler();
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


// void serialEvent2(){
//     ges_data = receive_parse_mpu();
// }

// void serialEvent1(){
//     ges_data = receive_parse_mpu();
// }

