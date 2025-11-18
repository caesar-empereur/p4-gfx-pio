#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "displays_config.h"


Arduino_ESP32DSIPanel *dsipanel = new Arduino_ESP32DSIPanel(
                                                            display_cfg.hsync_pulse_width,
                                                            display_cfg.hsync_back_porch,
                                                            display_cfg.hsync_front_porch,
                                                            display_cfg.vsync_pulse_width,
                                                            display_cfg.vsync_back_porch,
                                                            display_cfg.vsync_front_porch,
                                                            display_cfg.prefer_speed,
                                                            display_cfg.lane_bit_rate);

Arduino_DSI_Display *gfx = new Arduino_DSI_Display(
                                                    display_cfg.width,
                                                    display_cfg.height,
                                                    dsipanel,
                                                    0,
                                                    true,
                                                    display_cfg.lcd_rst,
                                                    display_cfg.init_cmds,
                                                    display_cfg.init_cmds_size);


void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    gfx->begin();
    gfx->fillScreen(GREEN);
}

void loop() {
  // put your main code here, to run repeatedly:
    delay(1000);
    Serial.println("esp32 p4");

    gfx->fillScreen(RED);
    delay(1000);
    gfx->fillScreen(CYAN);
    delay(1000);
    gfx->fillScreen(ORANGE);
}

