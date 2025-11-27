

/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief ESP LCD touch: CST3530
 */

#pragma once

#include "touch.h"
#include "i2c.h"
#include "displays_config.h"

#define EXAMPLE_PIN_NUM_TOUCH_RST       (GPIO_NUM_23)            // Reset pin for the touch controller (set to -1 if not used)
#define EXAMPLE_PIN_NUM_TOUCH_INT       (GPIO_NUM_NC)

/**
 * @brief Create a new CST3530 touch driver
 *
 * @note  The I2C communication should be initialized before use this function.
 *
 * @param io LCD panel IO handle, it should be created by `esp_lcd_new_panel_io_i2c()`
 * @param config Touch panel configuration
 * @param tp Touch panel handle
 * @return
 *      - ESP_OK: on success
 */
esp_err_t esp_lcd_touch_new_i2c_cst3530(const esp_lcd_panel_io_handle_t io, const esp_lcd_touch_config_t *config, esp_lcd_touch_handle_t *tp);

esp_lcd_touch_handle_t touch_cst3530_init(DEV_I2C_Port port);

/**
 * @brief I2C address of the CST3530 controller
 *
 */
#define ESP_LCD_TOUCH_IO_I2C_CST3530_ADDRESS    (0x58)

/**
 * @brief Touch IO configuration structure
 *
 */
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
#define ESP_LCD_TOUCH_IO_I2C_CST3530_CONFIG()             \
    {                                                     \
        .dev_addr = ESP_LCD_TOUCH_IO_I2C_CST3530_ADDRESS, \
        .on_color_trans_done = 0,                         \
        .user_ctx = 0,                                    \
        .control_phase_bytes = 1,                         \
        .dc_bit_offset = 0,                               \
        .lcd_cmd_bits = 8,                                \
        .lcd_param_bits = 0,                              \
        .flags =                                          \
        {                                                 \
            .dc_low_on_data = 0,                          \
            .disable_control_phase = 1,                   \
        }                                                 \
    }
#else
#define ESP_LCD_TOUCH_IO_I2C_CST3530_CONFIG()             \
    {                                                     \
        .dev_addr = ESP_LCD_TOUCH_IO_I2C_CST3530_ADDRESS, \
        .on_color_trans_done = 0,                         \
        .user_ctx = 0,                                    \
        .control_phase_bytes = 1,                         \
        .dc_bit_offset = 0,                               \
        .lcd_cmd_bits = 8,                                \
        .lcd_param_bits = 0,                              \
        .flags =                                          \
        {                                                 \
            .dc_low_on_data = 0,                          \
            .disable_control_phase = 1,                   \
        },                                                \
        .scl_speed_hz = 100000                            \
    }
#endif

