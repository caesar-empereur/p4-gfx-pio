
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


extern Arduino_DSI_Display *gfx;

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
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

    DEV_I2C_Port port = DEV_I2C_Init();
    tp_handle = touch_gt911_init(port);
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