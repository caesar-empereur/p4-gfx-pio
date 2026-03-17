#include <Arduino.h>
#include <Arduino_GFX_Library.h>


#define DCSBIOS_DEFAULT_SERIAL

#include "DcsBios.h"

#include "radar-new.h"
#include "compass.h"
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

extern uint16_t radar_scan_angle;



 void setup() {

    Serial.begin(115200);
    // Serial1.begin(115200);
    // Serial2.begin(115200);
    

    lvgl_display_init();
    ui_init();

    initDisplay();

    show_gesture();
    gestureInit(0,0);
    drawCompass();
    radarInitNew();

    // mpu_init();

    // DcsBios::setup();
}

 void loop() {

    lvgl_check_touch();

    
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

// void serialEvent(){
//     // refresh_p3d_data();
// }


// void serialEvent2(){
//     ges_data = receive_parse_mpu();
// }

// void serialEvent1(){
//     ges_data = receive_parse_mpu();
// }

