
#include "lvgl-display-init.h"

// 双击检测变量
static uint32_t last_tap_time = 0;
static uint32_t tap_count = 0;
static lv_point_t last_tap_point = {0, 0};
static const uint32_t DOUBLE_TAP_DELAY = 300; // 双击间隔300ms
static const uint32_t DOUBLE_TAP_DISTANCE = 20; // 双击最大距离




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


// Arduino_ESP32DSIPanel *dsipanel_lvgl = new Arduino_ESP32DSIPanel(
//   display_cfg.hsync_pulse_width,
//   display_cfg.hsync_back_porch,
//   display_cfg.hsync_front_porch,
//   display_cfg.vsync_pulse_width,
//   display_cfg.vsync_back_porch,
//   display_cfg.vsync_front_porch,
//   display_cfg.prefer_speed,
//   display_cfg.lane_bit_rate);

// Arduino_DSI_Display *gfx_lvgl = new Arduino_DSI_Display(
//   display_cfg.width,
//   display_cfg.height,
//   dsipanel_lvgl,
//   2,
//   true,
//   display_cfg.lcd_rst,
//   display_cfg.init_cmds,
//   display_cfg.init_cmds_size);

extern Arduino_DSI_Display *gfx;


static lv_obj_t * left_checkbox;
static lv_obj_t * right_checkbox;
static lv_obj_t * small_checkbox_container;
static lv_obj_t * small_checkbox[3];
static lv_obj_t * small_labels[3];

// 回调函数 - 当右边复选框状态改变时
static void right_checkbox_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = (lv_obj_t*)lv_event_get_target(e);
    
    if(code == LV_EVENT_VALUE_CHANGED) {
        bool checked = lv_obj_has_state(obj, LV_STATE_CHECKED);
        
        // 根据右边复选框的状态显示/隐藏小复选框容器
        if(checked) {
            lv_obj_clear_flag(small_checkbox_container, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(small_checkbox_container, LV_OBJ_FLAG_HIDDEN);
        }
    }
}



void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    // gfx_lvgl->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
    lv_display_flush_ready(disp);
}

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
    esp_lcd_touch_read_data(tp_handle);
    touch_pressed = esp_lcd_touch_get_coordinates(
      tp_handle, touch_x, touch_y, touch_strength, &touch_cnt, MAX_TOUCH_POINTS);

    if (touch_pressed && touch_cnt > 0) {
        //这里是触摸坐标反向的处理，不知道为什么触摸反向了
        data->point.x = display_cfg.width-touch_x[0];
        data->point.y = display_cfg.height-touch_y[0];

        //这里是原版正常的
        // data->point.x = touch_x[0];
        // data->point.y = touch_y[0];
        data->state = LV_INDEV_STATE_PRESSED;
        // last_pressed = true;
        Serial.println("x: " + String(data->point.x) + ", y: " +String(data->point.y));
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// void double_touch_read(lv_indev_t *indev, lv_indev_data_t *data) {
//     esp_lcd_touch_read_data(tp_handle);
//     touch_pressed = esp_lcd_touch_get_coordinates(
//       tp_handle, touch_x, touch_y, touch_strength, &touch_cnt, MAX_TOUCH_POINTS);

//     if (touch_pressed && touch_cnt > 0) {
//         //这里是触摸坐标反向的处理，不知道为什么触摸反向了
//         data->point.x = display_cfg.width-touch_x[0];
//         data->point.y = display_cfg.height-touch_y[0];


//         // 双击检测
//         uint32_t current_time = millis();
//         lv_point_t current_point = {data->point.x, data->point.y};
        
//         if(current_time - last_tap_time < DOUBLE_TAP_DELAY && 
//            abs(current_point.x - last_tap_point.x) < DOUBLE_TAP_DISTANCE &&
//            abs(current_point.y - last_tap_point.y) < DOUBLE_TAP_DISTANCE) {
//             tap_count++;
//             if(tap_count == 2) {
//                 // 检测到双击
//                 if(!ui_is_visible()) {
//                     create_ui();
//                 }
//                 tap_count = 0;
//             }
//         } else {
//             tap_count = 1;
//         }
        
//         last_tap_time = current_time;
//         last_tap_point = current_point;

//         //这里是原版正常的
//         // data->point.x = touch_x[0];
//         // data->point.y = touch_y[0];
//         data->state = LV_INDEV_STATE_PRESSED;
//         // last_pressed = true;
//         Serial.println("x: " + String(data->point.x) + ", y: " +String(data->point.y));
//     } else {
//         data->state = LV_INDEV_STATE_RELEASED;
//     }
// }

void lvglTick(void *param) {
    lv_tick_inc(LVGL_TICK_PERIOD);
}

void lvgl_display_init(){
    DEV_I2C_Port port = DEV_I2C_Init();

    tp_handle = touch_gt911_init(port);

    // tp_handle = touch_cst3530_init(port);

    if (!gfx->begin()) {
      Serial.println("gfx->begin() failed!");
    }

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
    // lv_indev_set_read_cb(indev_touchpad, double_touch_read);


    const esp_timer_create_args_t lvgl_timer_args = {
      .callback = &lvglTick,
      .name = "lvgl_timer"
    };
    esp_timer_handle_t lvgl_timer;
    esp_timer_create(&lvgl_timer_args, &lvgl_timer);
    esp_timer_start_periodic(lvgl_timer, LVGL_TICK_PERIOD * 1000);

    lv_display_set_dpi(lv_display, 150);
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);

    // lv_demo_widgets();
    // lv_demo_benchmark();  
    ui_init();
}

