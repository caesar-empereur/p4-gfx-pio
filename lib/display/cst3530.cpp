
/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "cst3530.h"

/* CST3530 registers */
#define ESP_LCD_TOUCH_CST3530_READ_KEY_REG    (0x8050)  // 按键状态寄存器
#define ESP_LCD_TOUCH_CST3530_READ_XY_REG     (0x814E)  // 触摸坐标数据寄存器
#define ESP_LCD_TOUCH_CST3530_CONFIG_REG      (0x8047)  // 配置寄存器
#define ESP_LCD_TOUCH_CST3530_PRODUCT_ID_REG  (0x8140)  // 产品ID寄存器
#define ESP_LCD_TOUCH_CST3530_ENTER_SLEEP     (0x8040)  // 进入睡眠模式寄存器
#define ESP_LCD_TOUCH_CST3530_STATUS_REG      (0x814E)  // 状态寄存器（与READ_XY_REG相同）
#define ESP_LCD_TOUCH_CST3530_GESTURE_REG     (0x814C)  // 手势识别寄存器
#define ESP_LCD_TOUCH_CST3530_FW_VERSION_REG  (0x8144)  // 固件版本寄存器
#define ESP_LCD_TOUCH_CST3530_CHIP_ID_REG     (0x8140)  // 芯片ID寄存器
#define ESP_LCD_TOUCH_CST3530_VENDOR_ID_REG   (0x814A)  // 厂商ID寄存器

/* CST3530 support key num */
#define ESP_CST3530_TOUCH_MAX_BUTTONS         (0)  // CST3530通常不支持物理按键

#define POINT_NUM_MAX (1)

#define DATA_START_REG (0x00)
#define CHIP_ID_REG (0xA7)
#define NomalWorkModeRegister 0xD0070000
#define ReportCoordinates 0xD00002AB
static const char *TAG = "CST3530";

#define ESP_GT911_TOUCH_MAX_BUTTONS         (4)

esp_lcd_touch_handle_t tp_handle = NULL;

static esp_err_t esp_lcd_touch_cst3530_read_data(esp_lcd_touch_handle_t tp);
static bool esp_lcd_touch_cst3530_get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num);
static esp_err_t esp_lcd_touch_cst3530_del(esp_lcd_touch_handle_t tp);

static esp_err_t touch_cst3530_i2c_read(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len);
static esp_err_t touch_cst3530_i2c_write(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t data);
static esp_err_t touch_cst3530_reset(esp_lcd_touch_handle_t tp);
static esp_err_t touch_cst3530_read_cfg(esp_lcd_touch_handle_t tp);

esp_err_t esp_lcd_touch_new_i2c_cst3530(const esp_lcd_panel_io_handle_t io, 
                    const esp_lcd_touch_config_t *config, esp_lcd_touch_handle_t *tp){
    // ESP_RETURN_ON_FALSE(io, ESP_ERR_INVALID_ARG, TAG, "无效的接口");
    // ESP_RETURN_ON_FALSE(config, ESP_ERR_INVALID_ARG, TAG, "无效的配置");
    // ESP_RETURN_ON_FALSE(tp, ESP_ERR_INVALID_ARG, TAG, "无效的触摸句柄");

    esp_err_t ret = ESP_OK;
    esp_lcd_touch_handle_t esp_lcd_touch_cst3530 = (esp_lcd_touch_handle_t)calloc(1, sizeof(esp_lcd_touch_t));
    ESP_GOTO_ON_FALSE(esp_lcd_touch_cst3530, ESP_ERR_NO_MEM, err, TAG, "触摸句柄分配内存失败");

    esp_lcd_touch_cst3530->io = io;
    esp_lcd_touch_cst3530->read_data = esp_lcd_touch_cst3530_read_data;
    esp_lcd_touch_cst3530->get_xy = esp_lcd_touch_cst3530_get_xy;
    esp_lcd_touch_cst3530->del = esp_lcd_touch_cst3530_del;
    esp_lcd_touch_cst3530->data.lock.owner = portMUX_FREE_VAL;
    memcpy(&esp_lcd_touch_cst3530->config, config, sizeof(esp_lcd_touch_config_t));

    // if (esp_lcd_touch_cst3530->config.int_gpio_num != GPIO_NUM_NC){
    //     gpio_config_t int_gpio_config = {
    //         .mode = GPIO_MODE_INPUT,
    //         .intr_type = (esp_lcd_touch_cst3530->config.levels.interrupt ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE),
    //         .pin_bit_mask = BIT64(config->int_gpio_num)};
    //     ESP_GOTO_ON_ERROR(gpio_config(&int_gpio_config), err, TAG, "GPIO配置失败");

    //     if (esp_lcd_touch_cst3530->config.interrupt_callback){
    //         esp_lcd_touch_register_interrupt_callback(esp_lcd_touch_cst3530, esp_lcd_touch_cst3530->config.interrupt_callback);
    //     }
    // }

    // if (esp_lcd_touch_cst3530->config.rst_gpio_num != GPIO_NUM_NC){
    //     const gpio_config_t rst_gpio_config = {
    //         .mode = GPIO_MODE_OUTPUT,
    //         .pin_bit_mask = BIT64(config->rst_gpio_num)};
    //     ESP_GOTO_ON_ERROR(gpio_config(&rst_gpio_config), err, TAG, "GPIO复位配置失败");
    // }
    if (config->rst_gpio_num != GPIO_NUM_NC) {
        gpio_config_t rst_gpio_config = {
            .pin_bit_mask = BIT64(config->rst_gpio_num),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ret = gpio_config(&rst_gpio_config);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to configure reset GPIO");
            heap_caps_free(esp_lcd_touch_cst3530);  // Free allocated memory
            return ret;
        }
    }

    // Configure the interrupt GPIO pin if specified
    if (config->int_gpio_num != GPIO_NUM_NC) {
        gpio_config_t int_gpio_config = {
            .pin_bit_mask = BIT64(config->int_gpio_num),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = config->levels.interrupt ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE,
        };
        ret = gpio_config(&int_gpio_config);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to configure interrupt GPIO");
            heap_caps_free(esp_lcd_touch_cst3530);  // Free allocated memory
            return ret;
        }
    }

    ESP_GOTO_ON_ERROR(touch_cst3530_reset(esp_lcd_touch_cst3530), err, TAG, "复位失败");

    // ESP_GOTO_ON_ERROR(touch_cst3530_read_cfg(cst3530), err, TAG, "读取寄存器失败");
    *tp = esp_lcd_touch_cst3530;

    return ESP_OK;
err:
    if (esp_lcd_touch_cst3530){
        esp_lcd_touch_cst3530_del(esp_lcd_touch_cst3530);
    }
    ESP_LOGE(TAG, "初始化失败!");
    return ret;
}

static esp_err_t esp_lcd_touch_cst3530_read_data(esp_lcd_touch_handle_t tp){
    typedef struct {
        uint8_t checksum_l;
        uint8_t checksum_h;
        uint8_t type;
        uint8_t num;
        uint8_t x_l;
        uint8_t y_l;
        uint8_t press_value;
        uint8_t xy_h;
        uint8_t id;
    } data_t;

    data_t point;
    // uint8_t buf[2]={0};
    // esp_lcd_panel_io_tx_param(tp->io, 0xd007, &buf, 2);
    uint8_t buf[3] = {0};
    buf[0] = (uint8_t)((NomalWorkModeRegister & 0x00ff0000) >> 16);
    buf[1] = (uint8_t)((NomalWorkModeRegister & 0x0000ff00) >> 8);
    buf[2] = (uint8_t)(NomalWorkModeRegister & 0x000000ff);
    //  i2c_read_bytes(tp, i, (uint8_t *)&buf, sizeof(buf));
    esp_lcd_panel_io_tx_param(tp->io, (uint8_t)(NomalWorkModeRegister >> 24), &buf, 3); // write stare
    uint8_t buf2[30] = {0};
    // ESP_RETURN_ON_ERROR(i2c_read_bytes(tp, DATA_START_REG, (uint8_t *)&point, sizeof(data_t)), TAG, "I2C读取失败！");
    ESP_RETURN_ON_ERROR(touch_cst3530_i2c_read(tp, 0x00, buf2, sizeof(buf2)), TAG, "I2C read failed");

    uint8_t buf3[3] = {0};
    buf3[0] = (uint8_t)((ReportCoordinates & 0x00ff0000) >> 16);
    buf3[1] = (uint8_t)((ReportCoordinates & 0x0000ff00) >> 8);
    buf3[2] = (uint8_t)(ReportCoordinates & 0x000000ff);
    //  i2c_read_bytes(tp, i, (uint8_t *)&buf, sizeof(buf));
    esp_lcd_panel_io_tx_param(tp->io, ReportCoordinates >> 24, &buf3, 3); // write  end

    point.num = buf2[3] & 0x0f;
    portENTER_CRITICAL(&tp->data.lock);
    point.num = (point.num > POINT_NUM_MAX ? POINT_NUM_MAX : point.num);
    tp->data.points = point.num;
    for (int i = 0; i < point.num; i++)
    {
        tp->data.coords[i].x = buf2[4] + ((uint16_t)(buf2[7]&0x0F) <<8);
        tp->data.coords[i].y = buf2[5] + ((uint16_t)(buf2[7]&0xf0) <<4);
        tp->data.coords[i].strength = buf2[6];
    }
    portEXIT_CRITICAL(&tp->data.lock);
    return ESP_OK;
}

// static esp_err_t esp_lcd_touch_cst3530_read_data(esp_lcd_touch_handle_t tp)
// {
//     esp_err_t err;
//     uint8_t buf[41];
//     uint8_t touch_cnt = 0;
//     uint8_t clear = 0;
//     size_t i = 0;

//     assert(tp != NULL);

//     err = touch_cst3530_i2c_read(tp, ESP_LCD_TOUCH_CST3530_READ_XY_REG, buf, 1);
//     ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");

//     /* Any touch data? */
//     if ((buf[0] & 0x80) == 0x00) {
//         touch_cst3530_i2c_write(tp, ESP_LCD_TOUCH_CST3530_READ_XY_REG, clear);
// #if (ESP_LCD_TOUCH_MAX_BUTTONS > 0)
//     } else if ((buf[0] & 0x10) == 0x10) {
//         /* Read all keys */
//         uint8_t key_max = ((ESP_GT911_TOUCH_MAX_BUTTONS < ESP_LCD_TOUCH_MAX_BUTTONS) ? \
//                            (ESP_GT911_TOUCH_MAX_BUTTONS) : (ESP_LCD_TOUCH_MAX_BUTTONS));
//         err = touch_cst3530_i2c_read(tp, ESP_LCD_TOUCH_CST3530_READ_KEY_REG, &buf[0], key_max);
//         ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");

//         /* Clear all */
//         touch_cst3530_i2c_write(tp, ESP_LCD_TOUCH_CST3530_READ_XY_REG, clear);
//         ESP_RETURN_ON_ERROR(err, TAG, "I2C write error!");

//         portENTER_CRITICAL(&tp->data.lock);

//         /* Buttons count */
//         tp->data.buttons = key_max;
//         for (i = 0; i < key_max; i++) {
//             tp->data.button[i].status = buf[0] ? 1 : 0;
//         }

//         portEXIT_CRITICAL(&tp->data.lock);
// #endif
//     } else if ((buf[0] & 0x80) == 0x80) {
// #if (ESP_LCD_TOUCH_MAX_BUTTONS > 0)
//         portENTER_CRITICAL(&tp->data.lock);
//         for (i = 0; i < ESP_LCD_TOUCH_MAX_BUTTONS; i++) {
//             tp->data.button[i].status = 0;
//         }
//         portEXIT_CRITICAL(&tp->data.lock);
// #endif
//         /* Count of touched points */
//         touch_cnt = buf[0] & 0x0f;
//         if (touch_cnt > 5 || touch_cnt == 0) {
//             touch_cst3530_i2c_write(tp, ESP_LCD_TOUCH_CST3530_READ_XY_REG, clear);
//             return ESP_OK;
//         }

//         /* Read all points */
//         err = touch_cst3530_i2c_read(tp, ESP_LCD_TOUCH_CST3530_READ_XY_REG + 1, &buf[1], touch_cnt * 8);
//         ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");

//         /* Clear all */
//         err = touch_cst3530_i2c_write(tp, ESP_LCD_TOUCH_CST3530_READ_XY_REG, clear);
//         ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");

//         portENTER_CRITICAL(&tp->data.lock);

//         /* Number of touched points */
//         touch_cnt = (touch_cnt > ESP_LCD_TOUCH_MAX_POINTS ? ESP_LCD_TOUCH_MAX_POINTS : touch_cnt);
//         tp->data.points = touch_cnt;

//         /* Fill all coordinates */
//         for (i = 0; i < touch_cnt; i++) {
//             tp->data.coords[i].x = ((uint16_t)buf[(i * 8) + 3] << 8) + buf[(i * 8) + 2];
//             tp->data.coords[i].y = (((uint16_t)buf[(i * 8) + 5] << 8) + buf[(i * 8) + 4]);
//             tp->data.coords[i].strength = (((uint16_t)buf[(i * 8) + 7] << 8) + buf[(i * 8) + 6]);
//         }

//         portEXIT_CRITICAL(&tp->data.lock);
//     }

//     return ESP_OK;
// }

static bool esp_lcd_touch_cst3530_get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, 
    uint8_t *point_num, uint8_t max_point_num){
    portENTER_CRITICAL(&tp->data.lock);

    *point_num = (tp->data.points > max_point_num ? max_point_num : tp->data.points);
    for (size_t i = 0; i < *point_num; i++){
        x[i] = tp->data.coords[i].x;
        y[i] = tp->data.coords[i].y;

        if (strength){
            strength[i] = tp->data.coords[i].strength;
        }
    }

    tp->data.points = 0;
    portEXIT_CRITICAL(&tp->data.lock);

    return (*point_num > 0);
}

static esp_err_t esp_lcd_touch_cst3530_del(esp_lcd_touch_handle_t tp){

    if (tp->config.int_gpio_num != GPIO_NUM_NC){
        gpio_reset_pin(tp->config.int_gpio_num);
        if (tp->config.interrupt_callback){
            gpio_isr_handler_remove(tp->config.int_gpio_num);
        }
    }
    if (tp->config.rst_gpio_num != GPIO_NUM_NC){
        gpio_reset_pin(tp->config.rst_gpio_num);
    }

    free(tp);

    return ESP_OK;
}

static esp_err_t touch_cst3530_reset(esp_lcd_touch_handle_t tp){
    if (tp->config.rst_gpio_num != GPIO_NUM_NC){
        ESP_RETURN_ON_ERROR(gpio_set_level(tp->config.rst_gpio_num, tp->config.levels.reset), TAG, "GPIO设置电平失败");
        vTaskDelay(pdMS_TO_TICKS(200));
        ESP_RETURN_ON_ERROR(gpio_set_level(tp->config.rst_gpio_num, !tp->config.levels.reset), TAG, "GPIO设置电平失败");
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    return ESP_OK;
}
static esp_err_t touch_cst3530_read_cfg(esp_lcd_touch_handle_t tp){
    uint8_t reg[26] = {0};
    ESP_RETURN_ON_ERROR(touch_cst3530_i2c_read(tp, 0XD000, reg, 26), TAG, "I2C读取失败");
    esp_lcd_panel_io_rx_param(tp->io, 0xD000, reg, 26);
    // ESP_LOGI(TAG, "触摸分辨率: X=%x,Y=%x", reg[0], reg[1]);

    return ESP_OK;
}

static esp_err_t touch_cst3530_i2c_read(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len){
    ESP_RETURN_ON_FALSE(data, ESP_ERR_INVALID_ARG, TAG, "无效的数据");

    return esp_lcd_panel_io_rx_param(tp->io, reg, data, len);
}

esp_lcd_touch_handle_t touch_cst3530_init(DEV_I2C_Port port){
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;  // Declare a handle for touch panel I/O
    // Configure the I2C communication settings for the GT911 touch controller
    const esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_CST3530_CONFIG();

    // Reset the touch screen before usage
    delay(10);
    // DEV_GPIO_Mode(EXAMPLE_PIN_NUM_TOUCH_INT, GPIO_MODE_INPUT_OUTPUT);  // Set GPIO pin mode for interrupt
    delay(200);  // Wait for 200ms to ensure the touch controller is ready

    ESP_LOGI(TAG, "Initialize I2C panel IO");  // Log I2C panel I/O initialization
    // Create a new I2C panel I/O handle for the touch controller
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(port.bus, &tp_io_config, &tp_io_handle));

    ESP_LOGI(TAG, "Initialize touch controller GT911");  // Log touch controller initialization
    // Configure the touch controller with necessary settings (coordinates, GPIO pins, etc.)
    const esp_lcd_touch_config_t tp_cfg = {
        // .x_max = display_cfg.width,  // Set the maximum X coordinate based on screen resolution
        // .y_max = display_cfg.height,  // Set the maximum Y coordinate based on screen resolution
        .x_max = display_cfg.width,  // Set the maximum X coordinate based on screen resolution
        .y_max = display_cfg.height,
        .rst_gpio_num = EXAMPLE_PIN_NUM_TOUCH_RST,  // GPIO number for reset
        .int_gpio_num = EXAMPLE_PIN_NUM_TOUCH_INT,  // GPIO number for interrupt
        .levels = {
            .reset = 0,  // Low level for reset
            .interrupt = 0,  // Low level for interrupt
        },
        .flags = {
            .swap_xy = 0,  // No swap of X and Y coordinates
            .mirror_x = 0,  // No mirroring of X axis
            .mirror_y = 0,  // No mirroring of Y axis
        },
    };

    // Create a new touch controller instance using the configured I2C and settings
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_cst3530(tp_io_handle, &tp_cfg, &tp_handle));

    return tp_handle;
}

static esp_err_t touch_cst3530_i2c_write(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t data)
{
    assert(tp != NULL);

    // *INDENT-OFF*
    /* Write data */
    // return esp_lcd_panel_io_tx_param(tp->io, reg, (uint8_t[]){data}, 1);
    uint8_t data_array[1] = {data};
    return esp_lcd_panel_io_tx_param(tp->io, reg, data_array, 1);

    // *INDENT-ON*
}