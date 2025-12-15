

#include "ui.h"
#include <Arduino.h>

// UI相关变量
static lv_obj_t* red_screen = NULL;
static lv_obj_t* ui_root_container = NULL;
static lv_obj_t* left_radio = NULL;
static lv_obj_t* right_radio = NULL;
static lv_obj_t* small_radio_container = NULL;
static lv_obj_t* small_radios[3];
static lv_obj_t* small_labels[3];
static lv_obj_t* back_btn = NULL;

static bool ui_visible = false;

// 主题颜色定义
#define GREEN_COLOR lv_color_hex(0x00FF00)
#define GREEN_DARK_COLOR lv_color_hex(0x008000)
#define BLACK_COLOR lv_color_hex(0x000000)

// 创建全屏红色背景
void create_red_screen() {
    if(red_screen != NULL) {
        lv_obj_del(red_screen);
    }
    
    red_screen = lv_obj_create(lv_scr_act());
    lv_obj_set_size(red_screen, 720, 720);
    lv_obj_set_style_bg_color(red_screen, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_border_width(red_screen, 0, 0);
    lv_obj_set_style_pad_all(red_screen, 0, 0);
    lv_obj_set_style_radius(red_screen, 0, 0);
    lv_obj_align(red_screen, LV_ALIGN_TOP_LEFT, 0, 0);
    
    // 添加提示文字
    lv_obj_t* hint_label = lv_label_create(red_screen);
    lv_label_set_text(hint_label, "双击屏幕显示菜单");
    lv_obj_set_style_text_color(hint_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(hint_label, &lv_font_montserrat_24, 0);
    lv_obj_align(hint_label, LV_ALIGN_CENTER, 0, 0);
    
    ui_visible = false;
}

// 返回按钮回调
static void back_btn_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_PRESSING) {
        // 长按时触发返回
        hide_ui();
    }
}

// 右边单选框回调
static void right_radio_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);
    
    if(code == LV_EVENT_VALUE_CHANGED) {
        bool checked = lv_obj_has_state(obj, LV_STATE_CHECKED);
        
        // 根据右边单选框的状态显示/隐藏小单选框容器
        if(checked) {
            lv_obj_clear_flag(small_radio_container, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(small_radio_container, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

// 创建UI界面
void create_ui() {
    if(ui_visible) return;
    
    // 隐藏红色背景
    if(red_screen != NULL) {
        lv_obj_add_flag(red_screen, LV_OBJ_FLAG_HIDDEN);
    }
    
    // 创建主容器（黑色背景）
    ui_root_container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_root_container, 720, 720);
    lv_obj_set_style_bg_color(ui_root_container, BLACK_COLOR, 0);
    lv_obj_set_style_border_width(ui_root_container, 0, 0);
    lv_obj_set_style_pad_all(ui_root_container, 0, 0);
    lv_obj_align(ui_root_container, LV_ALIGN_TOP_LEFT, 0, 0);
    
    // 创建返回按钮
    back_btn = lv_btn_create(ui_root_container);
    lv_obj_set_size(back_btn, 80, 80);
    lv_obj_set_pos(back_btn, 20, 20);
    lv_obj_set_style_bg_color(back_btn, GREEN_COLOR, 0);
    lv_obj_set_style_bg_color(back_btn, GREEN_DARK_COLOR, LV_STATE_PRESSED);
    lv_obj_set_style_radius(back_btn, 10, 0);
    
    // 返回按钮图标
    lv_obj_t* back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_color(back_label, BLACK_COLOR, 0);
    lv_obj_set_style_text_font(back_label, &lv_font_montserrat_32, 0);
    lv_obj_center(back_label);
    
    // 添加返回按钮事件
    lv_obj_add_event_cb(back_btn, back_btn_cb, LV_EVENT_ALL, NULL);
    
    // 创建返回文字提示
    lv_obj_t* back_text = lv_label_create(ui_root_container);
    lv_label_set_text(back_text, "长按返回");
    lv_obj_set_style_text_color(back_text, GREEN_COLOR, 0);
    lv_obj_set_style_text_font(back_text, &lv_font_montserrat_16, 0);
    lv_obj_align_to(back_text, back_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    
    // 创建左边大单选框
    left_radio = lv_checkbox_create(ui_root_container);
    lv_obj_set_size(left_radio, 240, 120);
    lv_obj_align(left_radio, LV_ALIGN_LEFT_MID, 40, -60);
    
    // 设置单选框样式
    lv_obj_set_style_bg_color(left_radio, GREEN_COLOR, 0);
    lv_obj_set_style_bg_color(left_radio, GREEN_DARK_COLOR, LV_STATE_CHECKED);
    lv_obj_set_style_radius(left_radio, 15, 0);
    lv_obj_set_style_border_width(left_radio, 3, 0);
    lv_obj_set_style_border_color(left_radio, GREEN_COLOR, 0);
    lv_obj_set_style_border_color(left_radio, GREEN_DARK_COLOR, LV_STATE_CHECKED);
    lv_obj_set_style_pad_all(left_radio, 15, 0);
    
    // 设置单选框文字
    lv_checkbox_set_text(left_radio, "P3D");
    
    // 调整文字样式
    lv_obj_t* text_label = lv_obj_get_child(left_radio, 1);
    if(text_label) {
        lv_obj_set_style_text_color(text_label, BLACK_COLOR, 0);
        lv_obj_set_style_text_font(text_label, &lv_font_montserrat_28, 0);
        lv_obj_set_style_text_align(text_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(text_label, LV_ALIGN_CENTER, 0, 0);
    }
    
    // 调整勾选框（居中显示）
    lv_obj_t* radio_box = lv_obj_get_child(left_radio, 0);
    if(radio_box) {
        lv_obj_set_size(radio_box, 40, 40);
        lv_obj_set_style_radius(radio_box, 20, 0); // 圆形
        lv_obj_set_style_bg_color(radio_box, BLACK_COLOR, 0);
        lv_obj_set_style_bg_color(radio_box, BLACK_COLOR, LV_STATE_CHECKED);
        lv_obj_set_style_border_width(radio_box, 2, 0);
        lv_obj_set_style_border_color(radio_box, BLACK_COLOR, 0);
        lv_obj_set_style_outline_width(radio_box, 0, 0);
        // 添加选中标志
        lv_obj_set_style_bg_img_src(radio_box, LV_SYMBOL_OK, LV_STATE_CHECKED);
        lv_obj_set_style_text_color(radio_box, GREEN_COLOR, LV_STATE_CHECKED);
        lv_obj_set_style_text_font(radio_box, &lv_font_montserrat_24, LV_STATE_CHECKED);
        lv_obj_align(radio_box, LV_ALIGN_CENTER, 0, 0);
    }
    
    // 创建右边大单选框
    right_radio = lv_checkbox_create(ui_root_container);
    lv_obj_set_size(right_radio, 240, 120);
    lv_obj_align(right_radio, LV_ALIGN_RIGHT_MID, -40, -60);
    
    // 设置单选框样式（与左边相同）
    lv_obj_set_style_bg_color(right_radio, GREEN_COLOR, 0);
    lv_obj_set_style_bg_color(right_radio, GREEN_DARK_COLOR, LV_STATE_CHECKED);
    lv_obj_set_style_radius(right_radio, 15, 0);
    lv_obj_set_style_border_width(right_radio, 3, 0);
    lv_obj_set_style_border_color(right_radio, GREEN_COLOR, 0);
    lv_obj_set_style_border_color(right_radio, GREEN_DARK_COLOR, LV_STATE_CHECKED);
    lv_obj_set_style_pad_all(right_radio, 15, 0);
    
    // 设置单选框文字
    lv_checkbox_set_text(right_radio, "DCS");
    
    // 调整文字样式
    text_label = lv_obj_get_child(right_radio, 1);
    if(text_label) {
        lv_obj_set_style_text_color(text_label, BLACK_COLOR, 0);
        lv_obj_set_style_text_font(text_label, &lv_font_montserrat_28, 0);
        lv_obj_set_style_text_align(text_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(text_label, LV_ALIGN_CENTER, 0, 0);
    }
    
    // 调整勾选框（居中显示）
    radio_box = lv_obj_get_child(right_radio, 0);
    if(radio_box) {
        lv_obj_set_size(radio_box, 40, 40);
        lv_obj_set_style_radius(radio_box, 20, 0);
        lv_obj_set_style_bg_color(radio_box, BLACK_COLOR, 0);
        lv_obj_set_style_bg_color(radio_box, BLACK_COLOR, LV_STATE_CHECKED);
        lv_obj_set_style_border_width(radio_box, 2, 0);
        lv_obj_set_style_border_color(radio_box, BLACK_COLOR, 0);
        lv_obj_set_style_outline_width(radio_box, 0, 0);
        lv_obj_set_style_bg_img_src(radio_box, LV_SYMBOL_OK, LV_STATE_CHECKED);
        lv_obj_set_style_text_color(radio_box, GREEN_COLOR, LV_STATE_CHECKED);
        lv_obj_set_style_text_font(radio_box, &lv_font_montserrat_24, LV_STATE_CHECKED);
        lv_obj_align(radio_box, LV_ALIGN_CENTER, 0, 0);
    }
    
    // 添加事件监听器到右边单选框
    lv_obj_add_event_cb(right_radio, right_radio_cb, LV_EVENT_ALL, NULL);
    
    // 创建小单选框容器（初始隐藏）
    small_radio_container = lv_obj_create(ui_root_container);
    lv_obj_set_size(small_radio_container, 400, 100);
    lv_obj_align_to(small_radio_container, right_radio, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);
    lv_obj_set_style_bg_opa(small_radio_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(small_radio_container, 0, 0);
    lv_obj_add_flag(small_radio_container, LV_OBJ_FLAG_HIDDEN);
    
    // 创建三个小单选框
    const char* small_labels_text[] = {"DCS1", "DCS2", "DCS3"};
    
    for(int i = 0; i < 3; i++) {
        // 创建小单选框
        small_radios[i] = lv_checkbox_create(small_radio_container);
        lv_obj_set_size(small_radios[i], 40, 40);
        lv_obj_set_pos(small_radios[i], 30 + i * 120, 0);
        
        // 设置小单选框样式
        lv_obj_set_style_bg_color(small_radios[i], GREEN_COLOR, 0);
        lv_obj_set_style_bg_color(small_radios[i], GREEN_DARK_COLOR, LV_STATE_CHECKED);
        lv_obj_set_style_radius(small_radios[i], 8, 0);
        lv_obj_set_style_border_width(small_radios[i], 2, 0);
        lv_obj_set_style_border_color(small_radios[i], GREEN_COLOR, 0);
        lv_obj_set_style_border_color(small_radios[i], GREEN_DARK_COLOR, LV_STATE_CHECKED);
        lv_obj_set_style_pad_all(small_radios[i], 8, 0);
        
        // 设置单选框文字
        lv_checkbox_set_text(small_radios[i], "");
        
        // 调整小单选框的勾选框
        radio_box = lv_obj_get_child(small_radios[i], 0);
        if(radio_box) {
            lv_obj_set_size(radio_box, 20, 20);
            lv_obj_set_style_radius(radio_box, 10, 0);
            lv_obj_set_style_bg_color(radio_box, BLACK_COLOR, 0);
            lv_obj_set_style_bg_color(radio_box, BLACK_COLOR, LV_STATE_CHECKED);
            lv_obj_set_style_border_width(radio_box, 1, 0);
            lv_obj_set_style_border_color(radio_box, BLACK_COLOR, 0);
            lv_obj_set_style_bg_img_src(radio_box, LV_SYMBOL_OK, LV_STATE_CHECKED);
            lv_obj_set_style_text_color(radio_box, GREEN_COLOR, LV_STATE_CHECKED);
            lv_obj_set_style_text_font(radio_box, &lv_font_montserrat_16, LV_STATE_CHECKED);
            lv_obj_align(radio_box, LV_ALIGN_CENTER, 0, 0);
        }
        
        // 创建小单选框下面的文字标签
        small_labels[i] = lv_label_create(small_radio_container);
        lv_label_set_text(small_labels[i], small_labels_text[i]);
        lv_obj_set_style_text_color(small_labels[i], GREEN_COLOR, 0);
        lv_obj_set_style_text_font(small_labels[i], &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_align(small_labels[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align_to(small_labels[i], small_radios[i], LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    }
    
    ui_visible = true;
}

// 隐藏UI，返回红色屏幕
void hide_ui() {
    if(!ui_visible) return;
    
    // 删除UI容器
    if(ui_root_container != NULL) {
        lv_obj_del(ui_root_container);
        ui_root_container = NULL;
    }
    
    // 显示红色背景
    if(red_screen != NULL) {
        lv_obj_clear_flag(red_screen, LV_OBJ_FLAG_HIDDEN);
    }
    create_red_screen();
    
    ui_visible = false;
}

// 检查UI是否可见
bool ui_is_visible() {
    return ui_visible;
}