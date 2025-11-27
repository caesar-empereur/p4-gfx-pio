
#include "lvgl-display-init.h"

static esp_lcd_touch_handle_t tp_handle = NULL;
#define MAX_TOUCH_POINTS 5

#define LVGL_TICK_PERIOD 5  // ms
#define DRAW_BUF_HEIGHT 50
static lv_display_t *lv_display;
static lv_indev_t *indev_touchpad;
static lv_color_t *lv_draw_buf1;
static lv_color_t *lv_draw_buf2;
static uint16_t touch_x[MAX_TOUCH_POINTS] = { 0 };
static uint16_t touch_y[MAX_TOUCH_POINTS] = { 0 };
static uint16_t touch_strength[MAX_TOUCH_POINTS] = { 0 };
static uint8_t touch_cnt = 0;
static bool touch_pressed = false;


Arduino_ESP32DSIPanel *dsipanel_lvgl = new Arduino_ESP32DSIPanel(
  display_cfg.hsync_pulse_width,
  display_cfg.hsync_back_porch,
  display_cfg.hsync_front_porch,
  display_cfg.vsync_pulse_width,
  display_cfg.vsync_back_porch,
  display_cfg.vsync_front_porch,
  display_cfg.prefer_speed,
  display_cfg.lane_bit_rate);

Arduino_DSI_Display *gfx_lvgl = new Arduino_DSI_Display(
  display_cfg.width,
  display_cfg.height,
  dsipanel_lvgl,
  2,
  true,
  display_cfg.lcd_rst,
  display_cfg.init_cmds,
  display_cfg.init_cmds_size);

// extern Arduino_DSI_Display *gfx;


static void right_radio_event_cb(lv_event_t * e);

// 创建主界面
// void create_main_ui(void)
// {
//     // 创建主容器
//     lv_obj_t * cont = lv_obj_create(lv_scr_act());
//     lv_obj_set_size(cont, 720, 720);
//     lv_obj_set_style_bg_color(cont, lv_color_hex(0xFFFFFF), 0);
//     lv_obj_center(cont);

//     // 创建左侧单选框容器
//     lv_obj_t * left_cont = lv_obj_create(cont);
//     lv_obj_set_size(left_cont, 180, 180);
//     lv_obj_set_pos(left_cont, 150, 150);  // 左侧位置
//     lv_obj_set_style_radius(left_cont, 20, 0);  // 圆角矩形
//     lv_obj_set_style_bg_color(left_cont, lv_color_hex(0xF0F0F0), 0);
//     lv_obj_set_style_border_color(left_cont, lv_color_hex(0xCCCCCC), 0);
//     lv_obj_set_style_border_width(left_cont, 2, 0);

//     // 左侧单选框
//     lv_obj_t * left_radio = lv_checkbox_create(left_cont);
//     lv_checkbox_set_text(left_radio, "A");
//     lv_obj_set_style_text_font(left_radio, &lv_font_montserrat_24, 0);
//     lv_obj_center(left_radio);

//     // 创建右侧单选框容器
//     lv_obj_t * right_cont = lv_obj_create(cont);
//     lv_obj_set_size(right_cont, 180, 180);
//     lv_obj_set_pos(right_cont, 390, 150);  // 右侧位置
//     lv_obj_set_style_radius(right_cont, 20, 0);  // 圆角矩形
//     lv_obj_set_style_bg_color(right_cont, lv_color_hex(0xF0F0F0), 0);
//     lv_obj_set_style_border_color(right_cont, lv_color_hex(0xCCCCCC), 0);
//     lv_obj_set_style_border_width(right_cont, 2, 0);

//     // 右侧单选框
//     lv_obj_t * right_radio = lv_checkbox_create(right_cont);
//     lv_checkbox_set_text(right_radio, "B");
//     lv_obj_set_style_text_font(right_radio, &lv_font_montserrat_24, 0);
//     lv_obj_center(right_radio);
    
//     // 为右侧单选框添加事件监听
//     lv_obj_add_event_cb(right_radio, right_radio_event_cb, LV_EVENT_VALUE_CHANGED, right_cont);

//     // 创建子选项容器（初始隐藏）
//     lv_obj_t * sub_cont = lv_obj_create(cont);
//     lv_obj_set_size(sub_cont, 400, 200);
//     lv_obj_set_pos(sub_cont, 260, 400);  // 位于右侧单选框下方
//     lv_obj_set_style_bg_color(sub_cont, lv_color_hex(0xF8F8F8), 0);
//     lv_obj_set_style_border_color(sub_cont, lv_color_hex(0xDDDDDD), 0);
//     lv_obj_set_style_border_width(sub_cont, 1, 0);
//     lv_obj_set_style_radius(sub_cont, 15, 0);
    
//     // 设置用户数据，用于存储子选项容器指针
//     lv_obj_set_user_data(right_cont, sub_cont);
    
//     // 初始隐藏子选项容器
//     lv_obj_add_flag(sub_cont, LV_OBJ_FLAG_HIDDEN);

//     // 创建子选项标题
//     lv_obj_t * sub_label = lv_label_create(sub_cont);
//     lv_label_set_text(sub_label, "子选项:");
//     lv_obj_set_style_text_font(sub_label, &lv_font_montserrat_20, 0);
//     lv_obj_set_pos(sub_label, 20, 15);

//     // 创建4个圆形单选框
//     const char * sub_options[] = {"a", "b", "c", "d"};
//     lv_obj_t * sub_radios[4];
    
//     for(int i = 0; i < 4; i++) {
//         // 创建单选框容器
//         lv_obj_t * radio_cont = lv_obj_create(sub_cont);
//         lv_obj_set_size(radio_cont, 80, 80);
//         lv_obj_set_pos(radio_cont, 20 + i * 90, 60);
//         lv_obj_set_style_radius(radio_cont, 40, 0);  // 圆形
//         lv_obj_set_style_bg_color(radio_cont, lv_color_hex(0xFFFFFF), 0);
//         lv_obj_set_style_border_color(radio_cont, lv_color_hex(0xCCCCCC), 0);
//         lv_obj_set_style_border_width(radio_cont, 1, 0);

//         // 创建单选框
//         sub_radios[i] = lv_checkbox_create(radio_cont);
//         lv_checkbox_set_text(sub_radios[i], sub_options[i]);
//         lv_obj_set_style_text_font(sub_radios[i], &lv_font_montserrat_18, 0);
//         lv_obj_center(sub_radios[i]);
        
//         // 设置单选框为单选模式
//         if(i > 0) {
//             lv_checkbox_set_group(sub_radios[i], lv_checkbox_get_group(sub_radios[0]));
//         }
//     }
// }

// // 右侧单选框事件回调函数
// static void right_radio_event_cb(lv_event_t * e)
// {
//     lv_obj_t * target = lv_event_get_target(e);
//     lv_obj_t * right_cont = lv_event_get_user_data(e);
//     lv_obj_t * sub_cont = lv_obj_get_user_data(right_cont);
    
//     if(lv_obj_has_state(target, LV_STATE_CHECKED)) {
//         // 显示子选项
//         lv_obj_clear_flag(sub_cont, LV_OBJ_FLAG_HIDDEN);
//     } else {
//         // 隐藏子选项
//         lv_obj_add_flag(sub_cont, LV_OBJ_FLAG_HIDDEN);
//     }
// }

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx_lvgl->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
    lv_display_flush_ready(disp);
}

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
    esp_lcd_touch_read_data(tp_handle);
    touch_pressed = esp_lcd_touch_get_coordinates(
      tp_handle, touch_x, touch_y, touch_strength, &touch_cnt, MAX_TOUCH_POINTS);

    if (touch_pressed && touch_cnt > 0) {
      data->point.x = touch_x[0];
      data->point.y = touch_y[0];
      data->state = LV_INDEV_STATE_PRESSED;
    } else {
      data->state = LV_INDEV_STATE_RELEASED;
    }
}

void lvglTick(void *param) {
    lv_tick_inc(LVGL_TICK_PERIOD);
}

void lvgl_display_init(){
    if (!gfx_lvgl->begin()) {
      Serial.println("gfx->begin() failed!");
    }


    DEV_I2C_Port port = DEV_I2C_Init();
    tp_handle = touch_cst3530_init(port);
    
    lv_init();
    size_t draw_buf_size = display_cfg.width * DRAW_BUF_HEIGHT;
    lv_draw_buf1 = (lv_color_t *)heap_caps_malloc(draw_buf_size * sizeof(lv_color_t), MALLOC_CAP_DMA);
    if (!lv_draw_buf1) {
      Serial.println("LVGL draw buffer 1 allocation failed!");
    }
    lv_draw_buf2 = (lv_color_t *)heap_caps_malloc(draw_buf_size * sizeof(lv_color_t), MALLOC_CAP_DMA);
    if (!lv_draw_buf2) {
      Serial.println("LVGL draw buffer 2 allocation failed!");
      heap_caps_free(lv_draw_buf1);
    }
    lv_display = lv_display_create(display_cfg.width, display_cfg.height);
    lv_display_set_flush_cb(lv_display, my_disp_flush);
    lv_display_set_buffers(lv_display, lv_draw_buf1, lv_draw_buf2, draw_buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    indev_touchpad = lv_indev_create();
    lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touchpad, my_touchpad_read);

    const esp_timer_create_args_t lvgl_timer_args = {
      .callback = &lvglTick,
      .name = "lvgl_timer"
    };
    esp_timer_handle_t lvgl_timer;
    esp_timer_create(&lvgl_timer_args, &lvgl_timer);
    esp_timer_start_periodic(lvgl_timer, LVGL_TICK_PERIOD * 1000);

    lv_display_set_dpi(lv_display, 150);
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);

    lv_demo_widgets();
    // lv_demo_benchmark();  
}

