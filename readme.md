Arduino_ESP32RGBPanel.h
// int data_gpio_nums[SOC_LCD_RGB_DATA_WIDTH]; 
// esp_lcd_rgb_panel_frame_trans_done_cb_t on_frame_trans_done;


Arduino_ESP32SPI.h
285
// PERIPH_RCC_ACQUIRE_ATOMIC(PERIPH_GPSPI2_MODULE, ref_count)
// if (ref_count == 0)
// {
//   PERIPH_RCC_ATOMIC()
//   {
//     spi_ll_enable_bus_clock(SPI2_HOST, true);
//     spi_ll_reset_register(SPI2_HOST);
//     spi_ll_enable_clock(SPI2_HOST, true);
//   }
// }


// PERIPH_RCC_ACQUIRE_ATOMIC(PERIPH_GPSPI3_MODULE, ref_count)
    // {
      // if (ref_count == 0)
      // // {
      //   PERIPH_RCC_ATOMIC()
      //   {
      //     spi_ll_enable_bus_clock(SPI3_HOST, true);
      //     spi_ll_reset_register(SPI3_HOST);
      //     spi_ll_enable_clock(SPI3_HOST, true);
      //   }
      // }
    // }


编辑 lv_demo_widgets.h 文件，将第17行的引用改为：

c
// 将原来的
#include "../../src/draw/lv_draw.h"

// 改为
#include "lvgl.h"
// 或者
#include <lvgl.h>


## GFX 库 BROWN
-- #define RGB565_BROWN RGB565(150, 75, 0)
-- #define BROWN RGB565_BROWN