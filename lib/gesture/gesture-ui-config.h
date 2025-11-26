#include "font-config.h"
// #include <Arduino_GFX_Library.h>

#ifndef GES_UI_CONFIG_H
#define GES_UI_CONFIG_H

// #define GES_SP_W 480
// #define GES_SP_H 480

#define GES_SP_W 480
#define GES_SP_H 480

//是否开启航向圆盘
#define YAW_SP_COMPASS_SHOW 1
#define LEFT_FLOAT_SP_SHOW 1
#define RIGHT_FLOAT_SP_SHOW 1

//整体颜色的 定义
/*
#define TWO_SP_SCALE_LINE_COLOR TFT_GREEN   //空速海拔控件的刻度线条颜色
#define TWO_SP_SCALE_TEXT_COLOR TFT_GREEN   //空速海拔控件的刻度文字颜色
#define TWO_SP_SCALE_LINE_BG_COLOR TFT_DARKGREY//空速海拔控件的背景颜色
#define TWO_SP_FLOAT_BG_COLOR TFT_DARKGREY      //空速海拔控件的数字显示背景颜色
#define TWO_SP_FLOAT_TEXT_COLOR TFT_GREEN    //空速海拔控件的数字显示文字颜色

#define HORI_SP_COLOR TFT_GREEN             //中间姿态地平线的全部颜色

#define YAW_SP_BG_COLOR TFT_DARKGREY           //航向圆盘控件的背景颜色
#define YAW_SP_SCALE_LINE_COLOR TFT_GREEN   //航向圆盘控件的刻度线条颜色
#define YAW_SP_SCALE_TEXT_COLOR TFT_GREEN   //航向圆盘控件的刻度文本颜色
#define YAW_SP_NUM_TEXT_COLOR TFT_GREEN     //航向圆盘控件的数字文本颜色
#define YAW_SP_NUM_TEXT_BG_COLOR TFT_DARKGREY     //航向圆盘控件的数字文本背景颜色
*/

#define TWO_SP_SCALE_LINE_COLOR GREEN   //空速海拔控件的刻度线条颜色
#define TWO_SP_SCALE_TEXT_COLOR GREEN   //空速海拔控件的刻度文字颜色
#define TWO_SP_SCALE_LINE_BG_COLOR DARKGREY//空速海拔控件的背景颜色
#define TWO_SP_FLOAT_BG_COLOR DARKGREY      //空速海拔控件的数字显示背景颜色
#define TWO_SP_FLOAT_TEXT_COLOR GREEN    //空速海拔控件的数字显示文字颜色

#define HORI_SP_COLOR GREEN             //中间姿态地平线的全部颜色

#define YAW_SP_BG_COLOR DARKGREY           //航向圆盘控件的背景颜色
#define YAW_SP_SCALE_LINE_COLOR GREEN   //航向圆盘控件的刻度线条颜色
#define YAW_SP_SCALE_TEXT_COLOR GREEN   //航向圆盘控件的刻度文本颜色
#define YAW_SP_NUM_TEXT_COLOR GREEN     //航向圆盘控件的数字文本颜色
#define YAW_SP_NUM_TEXT_BG_COLOR DARKGREY     //航向圆盘控件的数字文本背景颜色


//左边空速跟右边海拔的 ui 宽高配置
// uint32_t SKY_COLOR = TFT_DARKCYAN, GROUND_COLOR = TFT_ORANGE;
// #define TWO_SP_BIG_SCALE_GAP (GES_SP_H/8)
#define TWO_SP_BIG_SCALE_GAP ((GES_SP_H==460?480:GES_SP_H)/8) //2个垂直刻度的大刻度的间隔, 高度的一半 /2, 再 /4 刻度改成跟像素对应 高度 240, GAP=30, 高度320, GAP=40
#define TWO_SP_SMALL_SCALE_GAP (TWO_SP_BIG_SCALE_GAP/5) //2个垂直刻度的小刻度的间隔
// #define TWO_SP_WIDTH (GES_SP_W/6)
#define TWO_SP_WIDTH ((GES_SP_W==460?480:GES_SP_W)/6) //2个垂直刻度的 SP 宽度
// #define TWO_SP_HEIGHT ((GES_SP_H/2)+((TWO_SP_BEGIN_DRAW_Y)*2)) //2个垂直刻度的 SP 高度, 120+20, 160+20
#define TWO_SP_HEIGHT (((GES_SP_W==460?480:GES_SP_W)/2)+((TWO_SP_BEGIN_DRAW_Y)*2)) 
// #define TWO_SP_BIG_SCALE_WIDTH ((GES_SP_W)<=240?((TWO_SP_WIDTH)*0.35):(TWO_SP_WIDTH)*0.5) //2个垂直刻度的大刻度的宽度
// #define TWO_SP_SMALL_SCALE_WIDTH ((GES_SP_W)<=240?(TWO_SP_WIDTH/5):(TWO_SP_WIDTH/4)) //2个垂直刻度的小刻度的宽度
#define TWO_SP_BIG_SCALE_WIDTH ((TWO_SP_WIDTH)*0.35) //2个垂直刻度的大刻度的宽度
template<typename T>
int16_t TWO_SP_BIG_SCALE_LINE_WIDTH(T t){//2个垂直刻度的大刻度的线宽度
    int16_t w;
    if(GES_SP_H<240){
        w = 1;
    } else if(GES_SP_H>=240 && GES_SP_H<=480){
        w = 2;
    } else if(GES_SP_H>480){
        w = 3;
    }
    return w;


}                  
#define TWO_SP_SMALL_SCALE_WIDTH ((TWO_SP_WIDTH)/5) //2个垂直刻度的小刻度的宽度
#define TWO_SP_BEGIN_DRAW_Y 10      //2个垂直刻度的 SP 上下预留高度像素

// #define TWO_SP_MARGIN_TOP (GES_SP_H/4) //2个垂直刻度的 SP 的顶部外边距，就是到顶部屏幕边缘的距离
#define TWO_SP_MARGIN_TOP ((GES_SP_H==460?480:GES_SP_H)/4)
#define TWO_SP_FLOAT_W (TWO_SP_WIDTH)
template<typename T>
int16_t TWO_SP_FLOAT_H(T t) {
    // return TWO_SP_FLOAT_W * 0.5;
    int16_t h;
    int16_t font_size = GET_FONT_SIZE(GES_SP_W, GES_SP_H) + 1;
    if(font_size == 1){
        h = FONT1_H;
    }
    if(font_size == 2){
        h = FONT2_H;
    }
    if(font_size == 3){
        h = FONT3_H;
    }
    if(font_size == 4){
        h = FONT4_H;
    }
    h = h + h/4;
    return h;
}

//中间姿态仪 ui 的配置
#define HORI_SP_SCALE_NUM 5 //必须得是奇数
#define HORI_SP_SCROLL_BEGIN_Y 10

//中间水平姿态刻度的像素间隔
template<typename T>
int16_t HORI_SP_SCALE_PIX_INTERVAL(T t) {
    int16_t h;
    int16_t min_h = min(GES_SP_W, GES_SP_H);
    if(min_h<240){
        h = 20;
    } else if(min_h>=240 && min_h <= 300){
        h = 20;
    } else if(min_h>300 && min_h <= 400){
        h = 30;
    } else if(min_h>400 && min_h <= 500){
        h = 40;
    } else if(min_h>=720){
        h = 60;
    }
    return h;
}

#define HORI_SP_SCALE_VALUE_INTERVAL 5 //中间水平姿态 SP 的每个刻度的值间隔
#define HORI_SP_SCALE_PIX_RATE ((HORI_SP_SCALE_PIX_INTERVAL(0))/(HORI_SP_SCALE_VALUE_INTERVAL))//值刻度与像素的滚动比率
// #define HORI_SP_CROWN_H 30 //中间水平姿态 SP 顶部皇冠部分的高度
#define HORI_SP_CROWN_H (GES_SP_H/10)
// #define HORI_SP_CROWN_SCALE_LEN ((GES_SP_H)<240?(5):(10))// 皇冠部分的刻度长度
#define HORI_SP_CROWN_SCALE_LEN ((GES_SP_H)<240?(5):(GES_SP_H*0.04))
// #define HORI_SP_CROWN_RADIUS_RATE ((YAW_SP_COMPASS_SHOW)==1?((GES_SP_H)<=240?(0.9):(0.95)):(1))// 皇冠部分的刻度对半径的比率,不开启罗盘都是1，开启罗盘根据高度判断
template<typename T>
float_t HORI_SP_CROWN_RADIUS_RATE(T t) {
    float_t rate = 1;
    if(YAW_SP_COMPASS_SHOW == 1){
        if(GES_SP_H <=240){
            rate = 0.9;
        } else {
            rate = 0.95;
        }
    } else {
        rate = 1;
    }
    return rate;
}

#define HORI_SP_SCROLL_H ((HORI_SP_SCALE_PIX_INTERVAL(0))*(HORI_SP_SCALE_NUM-1)+(HORI_SP_SCROLL_BEGIN_Y)*2) //中间水平姿态 SP 滚动部分的高度
#define HORI_SP_WIDTH (HORI_SP_SCROLL_H) // 中间水平姿态 SP 的宽度
#define HORI_SP_HEIGHT ((HORI_SP_SCROLL_H)+(HORI_SP_CROWN_H)) // 中间水平姿态 SP 的高度

// #define HORI_SP_LINE_WIDTH ((GES_SP_H) <= 240?(1):(3))
template<typename T>
int16_t HORI_SP_LINE_WIDTH(T t){
    int16_t line;
    if(GES_SP_H<=240){
        line = 1;
    } else if(GES_SP_H>240 && GES_SP_H<=480){
        line = 2;
    } else if(GES_SP_H>480){
        line = 3;
    }
    return line;
}
// #define HORI_SP_SCALE_W ((HORI_SP_WIDTH) * 0.09)
#define HORI_SP_SCALE_W ((HORI_SP_WIDTH) * 0.12)   //中间水平姿态刻度的水平宽度
#define HORI_SP_SCALE_END_PAD (HORI_SP_WIDTH*0.35) //中间水平姿态 SP 的大小刻度的终点到左边的边距
#define HORI_SP_SCALE_L_BEGIN (HORI_SP_SCALE_END_PAD-HORI_SP_SCALE_W) //中间水平姿态 SP Z左边的刻度的水平开始位置
#define HORI_SP_SCALE_R_BEGIN (HORI_SP_WIDTH-HORI_SP_SCALE_END_PAD) //中间水平姿态 SP Z左边的刻度的水平开始位置
#define HORI_SP_TEXT_R_BEGIN (HORI_SP_SCALE_R_BEGIN+HORI_SP_SCALE_W) //中间水平姿态 SP Z左边的刻度的水平开始位置

// #define HORI_SP_H_LINE_W 20//姿态仪中间的横线
#define HORI_SP_H_LINE_W (GES_SP_W*0.08)//姿态仪中间的横线

#define ELLIPSE_SP_W_GAP ((GES_SP_H)<(320)?((TWO_SP_WIDTH)/4):((TWO_SP_WIDTH)/4))
#define ELLIPSE_SP_W ((GES_SP_W)-(TWO_SP_WIDTH)*2-2*(ELLIPSE_SP_W_GAP))
template<typename T>
int16_t ELLIPSE_SP_H(T t) {
    int16_t h;
    if(YAW_SP_COMPASS_SHOW ==1){//只要开启航向罗盘, 高度则最高只能跟 2 边的空速一样高
        h = TWO_SP_HEIGHT;
    } else {
        if(GES_SP_H > GES_SP_W){
            h = TWO_SP_HEIGHT;//如果是高度大于宽度，纵向足够长，则跟2边的sp高度一样
        } else {
            h = TWO_SP_HEIGHT * 1.2;
        }
    }
    return h;
}

template<typename T>
float_t ELLIPSE_SP_RADIUS_RATE(T t) {
    float_t rate;
    if(GES_SP_H > GES_SP_W){
        rate = 1;
    } else {
        if(GES_SP_H <= 200){
            rate = 1.1;
        } else {
            if(GES_SP_H>=320){
                rate = 1;
            } else {
                rate = 1.1;
            }
        }
    }
    return rate;
}


//方向角度的 sp 高度, yaw 的浮动显示的宽度, 和高度
#define YAW_SP_BEGIN_X 12   //左右2边留下的像素空隙
#define YAW_SP_H (TWO_SP_FLOAT_H(0) * 0.8)
#define YAW_SP_SC_VALUE_INTV 30//显示的刻度间隔, 300, 330, 0, 30, 60 因此间隔是 30
#define YAW_SP_SC_PIXEL_INTV ((GES_SP_W-(YAW_SP_BEGIN_X)*2)/4)//显示的刻度间隔对应的像素间隔是 60


#define YAW_FLOAT_W (TWO_SP_FLOAT_W)
#define YAW_FLOAT_H (YAW_SP_H)

//底部的航向旋转控件
#define YAW_ROTATE_SP_BG_H (GES_SP_H/4-(TWO_SP_BEGIN_DRAW_Y))//背景的 sp
#define YAW_ROTATE_SP_BG_W (GES_SP_W)
// #define YAW_ROTATE_SP_A ((GES_SP_W-TWO_SP_WIDTH)>300?(300):(GES_SP_W-TWO_SP_WIDTH)) //横线
#define YAW_ROTATE_SP_A (GES_SP_W-2*TWO_SP_WIDTH) //横线
// #define YAW_ROTATE_SP_A (320) //横线
#define YAW_ROTATE_SP_B ((GES_SP_H/4)*(0.75)) //竖线
// #define YAW_ROTATE_SP_RADIUS (YAW_ROTATE_SP_BG_H)
#define YAW_ROTATE_SP_RADIUS (((pow(YAW_ROTATE_SP_A,2) + 4 * pow(YAW_ROTATE_SP_B, 2))) / (8 * (YAW_ROTATE_SP_B)))
#define YAW_ROTATE_SP_POVIT_Y (YAW_ROTATE_SP_RADIUS - YAW_ROTATE_SP_B) //前景的 sp 根据 y 坐标旋转
#define YAW_RORATE_SP_SCALE_BIG_LEN (YAW_ROTATE_SP_RADIUS * 0.09)
#define YAW_RORATE_SP_SCALE_SMALL_LEN (YAW_ROTATE_SP_RADIUS * 0.05)
#define YAW_RORATE_SP_SCALE_SMALL_GAP 6
#define YAW_RORATE_SP_SCALE_BIG_GAP 30
#define YAW_RORATE_SP_V_LINE_LEN (YAW_ROTATE_SP_RADIUS * 0.05)
#define YAW_RORATE_SP_V_LINE_Y (GES_SP_H-YAW_ROTATE_SP_B-YAW_RORATE_SP_V_LINE_LEN)


#define YAW_RORATE_TEXT_SP_W (YAW_ROTATE_SP_RADIUS * 0.2)
#define YAW_RORATE_TEXT_SP_H (YAW_ROTATE_SP_RADIUS * 0.15)


#endif