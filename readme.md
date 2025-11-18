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