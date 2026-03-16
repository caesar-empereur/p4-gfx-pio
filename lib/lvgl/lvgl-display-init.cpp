
#include "lvgl-display-init.h"
#if 1
extern Arduino_DSI_Display *gfx; //显示句柄
extern esp_lcd_touch_handle_t tp_handle; //触摸句柄

// 双击检测变量
static uint32_t last_tap_time = 0;
static uint32_t tap_count = 0;
static lv_point_t last_tap_point = {0, 0};
static const uint32_t DOUBLE_TAP_DELAY = 300; // 双击间隔300ms
static const uint32_t DOUBLE_TAP_DISTANCE = 20; // 双击最大距离



// -------------------- By MiluoOffical 2026.02.02 --------------
//static esp_lcd_touch_handle_t tp_handle = NULL;
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


void lvglTick(void *param) {
    lv_tick_inc(LVGL_TICK_PERIOD);
}

void lvgl_display_init(){
    DEV_I2C_Port port = DEV_I2C_Init();

    // tp_handle = touch_gt911_init(port);

    tp_handle = touch_cst3530_init(port);

    // if (!gfx->begin()) {
    //   Serial.println("gfx->begin() failed!");
    // }

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

}

#else

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


void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx_lvgl->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
    lv_display_flush_ready(disp);
}

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
    // static bool last_pressed = false;
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
        // last_pressed = false;
    }


}



void lvglTick(void *param) {
    lv_tick_inc(LVGL_TICK_PERIOD);
}

void lvgl_display_init(){
    DEV_I2C_Port port = DEV_I2C_Init();

    // tp_handle = touch_gt911_init(port);

    tp_handle = touch_cst3530_init(port);

    if (!gfx_lvgl->begin()) {
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

#endif

void lvgl_check_touch(){
    //检测到红屏为false时, lvgl需要及时接管.
    if(ges_show_type != 0 ){ //当前是 GFX 显示的状态, 持续检测手势, 监测到上滑手势时标记 redScreen_status 为false, 下个循环将会自动进入LVGL部分
        //Serial.println("ges_show_type != 0");

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
    

        //Serial.printf("pressed= %d\n" , pressed);
        static uint32_t refreshCoolDown = 0;  // 3/13 MiluoOffical. : 标记触摸冷却期的变量. 检测到触摸后, 立刻进入冷却禁止刷屏, 避免刷屏信号干扰触摸检测和手势检测
        if(pressed==true){
            if(lastPressed==false){ //手指刚放在屏幕上, 记录触摸点
                first_touch_x = touch_x;
                first_touch_y = touch_y;
                lastPressed = true;
            }
            last_touch_x = touch_x; //只要触摸没断触, 就持续记录触摸点
            last_touch_y = touch_y;

            refreshCoolDown = millis() + TOUCH_SLIDE_COOLDOWN_TIME; // 3/13 MiluoOffical. : 检测到触摸信号后, 新增100ms冷却期, 在此期间不刷屏
        }
        else if(millis() >= refreshCoolDown){ // 3/13 MiluoOffical. : 不处于冷却期, 而且在触摸空闲状态: 刷屏, 刷新方框内容
            update_gfx_screen();              // 3/13 MiluoOffical. : 刷屏. 实际的刷屏处理在 ges_show_handler 函数中. 这里设置好变量之后刷屏任务会自动检测刷屏
        }
        else {
            pressed = true; // 3/13 MiluoOffical. : 断触处理. 如果刚刚检测到触摸不久, 可能会有驱动层面的断触bug, 这里修复此bug来确保流畅的触屏体验.
        }
        //Serial.println("pressed= %d, lastPressed= %d\n" , pressed, lastPressed);
        if(pressed==false && lastPressed==true){ //如果移动方位角在-45°~45°之间, 那么视为向上滑动
            int32_t dx = ((int32_t)last_touch_x-(int32_t)first_touch_x);
            int32_t dy = ((int32_t)last_touch_y-(int32_t)first_touch_y);
            int32_t distance_sq = (dx*dx+dy*dy);
            float angle = atan2f(dx,dy) * 180 / 3.14f;
            Serial.printf("Detected gesture: Pix:%4d, Ang: %3d\n", int(sqrt(distance_sq)), int(angle));

            //参考阈值: 距离 10000 (100像素), 角度 -45~45
            if(distance_sq >= (GESTURE_DISTANCE_PIXEL*GESTURE_DISTANCE_PIXEL) && angle > -45 &&  angle < 45 ){ //满足上滑动作的阈值
                return_to_lvgl(); //退出红屏状态
                Serial.println("return_to_lvgl");
                
            }
            if(distance_sq >= (GESTURE_DISTANCE_PIXEL*GESTURE_DISTANCE_PIXEL) && angle > 45 &&  angle < 135 ){ //满足右滑动作的阈值
                if(ges_show_type < GFX_SCREENS) ges_show_type ++; 
                update_gfx_screen(); //调用该函数之后将在下一个 displayHandler() 调用时全屏刷新
                Serial.println("touch from right to left");
            }
            if(distance_sq >= (GESTURE_DISTANCE_PIXEL*GESTURE_DISTANCE_PIXEL) && angle > -135 &&  angle < -45 ){ //满足左滑动作的阈值
                if(ges_show_type > 1) ges_show_type --; 
                update_gfx_screen(); //调用该函数之后将在下一个 displayHandler() 调用时全屏刷新
                Serial.println("touch from left to right");
            }
            lastPressed = false;
        }
    }
    ges_show_handler(); //进入lvgl控制, 此函数检测红屏标记值, 如果不处于红屏状态, 调用 lv_timer_handler(); 
    delay(5);
}