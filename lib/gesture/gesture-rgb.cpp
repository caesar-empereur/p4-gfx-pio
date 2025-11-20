#include "gesture-rgb.h"

int FONT_SIZE = 1;


extern Rgb_Sprite bgSprAllHori;

//椭圆的 sp
extern Rgb_Sprite bgSprEllipse;

extern Rgb_Sprite airSpeedSp;
extern Rgb_Sprite altitudeSp;
extern Rgb_Sprite airSpeedSpBg;
extern Rgb_Sprite altitudeSpBg;
extern Rgb_Sprite airSpeedFloatSp;
extern Rgb_Sprite altitudeFloatSp;

extern Rgb_Sprite midHoriSp;
extern Rgb_Sprite midHoriLineSp;//姿态仪中间的标记横线

extern Rgb_Sprite yawSp;
extern Rgb_Sprite yawSpBlackBg;
extern Rgb_Sprite yawFloatFixSp;

extern Rgb_Sprite yawRotateBgSp;
extern Rgb_Sprite yawRotateSp;
extern Rgb_Sprite yawRotateNoFillSp;
extern Rgb_Sprite yawRotateTextSp;
extern Rgb_Sprite yawRotateVLineSp;
// uint32_t SKY_COLOR = tft.alphaBlend(125, TFT_DARKCYAN, TFT_CYAN);

bool has_init_yaw_bg_1 = false;
bool has_init_yaw_bg_2 = false;
bool has_init_yaw_bg_3 = false;

long MOVE_DELAY = 5;

int16_t pos_y_air = TWO_SP_BEGIN_DRAW_Y;
int16_t pos_y_alti= TWO_SP_BEGIN_DRAW_Y;
int16_t pos_y_hori = HORI_SP_CROWN_H+HORI_SP_SCROLL_BEGIN_Y;
int16_t pos_x_yaw = YAW_SP_BEGIN_X;//记录 yaw 的 移动的横坐标

int16_t roll_prev = 0, 
        pitch_prev = 0, 
        yaw_prev = 0, 
    
        air_speed_prev = 0, 
        altitude_prev  = 0;


void rorateHorizon(Rgb_Sprite *bgSprP, int angle){
    midHoriSp.pushRotated(bgSprP, angle, TFT_TRANSPARENT);
    bgSprP->pushSprite(0, 0);
}


/*
画出圆上的直线，相当于圆上的刻度
radius 半径  angle 角度
lenght 刻度的长度，单位为像素
ratio 在半径多长的比例的圆形上面
*/
void drawDotLine(Rgb_Sprite *bgSprP, uint16_t pos_x_mid, uint16_t pos_y_mid, uint16_t radius, int angle, int lenght, float ratio, uint16_t color){
    float start_x = pos_x_mid + radius*ratio*sin(radians(angle));
    float start_y = pos_y_mid - radius*ratio*cos(radians(angle));

    //算出内圈点的结束点
    float end_x   = pos_x_mid + (radius*ratio - lenght) * sin(radians(angle));
    float end_y   = pos_y_mid - (radius*ratio - lenght) * cos(radians(angle));
    bgSprP->drawLine(start_x, start_y, end_x, end_y, color);
}

void drawYawRotateSp(int16_t x, int16_t y, bool fillYawSpBg){
    yawRotateBgSp.fillScreen(TFT_TRANSPARENT);

    if(fillYawSpBg){
        yawRotateSp.fillScreen(TFT_TRANSPARENT);
        yawRotateSp.fillCircle(YAW_ROTATE_SP_RADIUS, YAW_ROTATE_SP_RADIUS, YAW_ROTATE_SP_RADIUS, YAW_SP_BG_COLOR);
        yawRotateSp.drawCircle(YAW_ROTATE_SP_RADIUS, YAW_ROTATE_SP_RADIUS, YAW_ROTATE_SP_RADIUS, YAW_SP_SCALE_LINE_COLOR);
    } else {
        yawRotateNoFillSp.fillScreen(TFT_TRANSPARENT);
        yawRotateNoFillSp.drawCircle(YAW_ROTATE_SP_RADIUS, YAW_ROTATE_SP_RADIUS, YAW_ROTATE_SP_RADIUS, YAW_SP_SCALE_LINE_COLOR);
    }

    yawRotateTextSp.setTextSize(FONT_SIZE);

    uint16_t radius = YAW_ROTATE_SP_RADIUS;
    for(int i=0;i<360;i=i+YAW_RORATE_SP_SCALE_SMALL_GAP){
        if(i % YAW_RORATE_SP_SCALE_BIG_GAP ==0){
            drawDotLine(&yawRotateSp, radius,radius,radius,i,YAW_RORATE_SP_SCALE_BIG_LEN,1, YAW_SP_SCALE_LINE_COLOR);
            drawDotLine(&yawRotateNoFillSp, radius,radius,radius,i,YAW_RORATE_SP_SCALE_BIG_LEN,1, YAW_SP_SCALE_LINE_COLOR);
        } else {
            drawDotLine(&yawRotateSp, radius,radius,radius,i,YAW_RORATE_SP_SCALE_SMALL_LEN,1, YAW_SP_SCALE_LINE_COLOR);
            drawDotLine(&yawRotateNoFillSp, radius,radius,radius,i,YAW_RORATE_SP_SCALE_SMALL_LEN,1, YAW_SP_SCALE_LINE_COLOR);
        }
        
    }
    yawRotateTextSp.setTextSize(FONT_SIZE);


    int16_t font_size = GET_FONT_SIZE(GES_SP_W, GES_SP_H);
    for(int16_t i=0;i<360;i=i+30){
        float end_x   = YAW_ROTATE_SP_RADIUS + (YAW_ROTATE_SP_RADIUS - YAW_RORATE_SP_SCALE_BIG_LEN*1.5) * sin(radians(i));
        float end_y   = YAW_ROTATE_SP_RADIUS - (YAW_ROTATE_SP_RADIUS - YAW_RORATE_SP_SCALE_BIG_LEN*1.5) * cos(radians(i));
        yawRotateTextSp.setTextColor(HORI_SP_COLOR, TFT_TRANSPARENT);
        yawRotateTextSp.fillScreen(TFT_TRANSPARENT);
        int16_t l_x_start = GET_TEXT_L_START(static_cast<int16_t>(YAW_RORATE_TEXT_SP_W), font_size, i);
        int16_t y_down = YAW_RORATE_TEXT_SP_H/2-GET_TEXT_Y_HALF(font_size)+GET_TEXT_Y_DOWN(font_size);
        yawRotateTextSp.setCursor(l_x_start, y_down);
        yawRotateTextSp.println(String(i));

        if(fillYawSpBg){
            yawRotateSp.setPivot(end_x, end_y);
            yawRotateTextSp.pushRotated(&yawRotateSp, i, TFT_TRANSPARENT);
        } else {
            yawRotateNoFillSp.setPivot(end_x, end_y);
            yawRotateTextSp.pushRotated(&yawRotateNoFillSp, i, TFT_TRANSPARENT);
        }
    }
    
}

void drawHorizon(){
    //先画出最上面的扇形刻度
    if(GES_SP_H>=240){
        for(int i=330;i<=360;i=i+10){
            drawDotLine(&midHoriSp, HORI_SP_WIDTH/2,HORI_SP_HEIGHT/2,HORI_SP_HEIGHT/2,i,
                            HORI_SP_CROWN_SCALE_LEN,HORI_SP_CROWN_RADIUS_RATE(0), HORI_SP_COLOR);
        }
        //先画出最上面的扇形刻度
        for(int i=10;i<=30;i=i+10){
            drawDotLine(&midHoriSp, HORI_SP_WIDTH/2,HORI_SP_HEIGHT/2,HORI_SP_HEIGHT/2,i,
                            HORI_SP_CROWN_SCALE_LEN,HORI_SP_CROWN_RADIUS_RATE(0), HORI_SP_COLOR);
        }
    }
    int16_t font_size = GET_FONT_SIZE(GES_SP_W, GES_SP_H);
    
    //画出水平仪的左边小刻度, 一共 6个小刻度
    for(int16_t i=1;i<=HORI_SP_SCALE_NUM;i++){
        int cur_scale_y = HORI_SP_SCROLL_BEGIN_Y+HORI_SP_CROWN_H + (i-1)*HORI_SP_SCALE_PIX_INTERVAL(0);
        int16_t scale_num = HORI_SP_SCALE_VALUE_INTERVAL*2-(i-1)*HORI_SP_SCALE_VALUE_INTERVAL;
        midHoriSp.setTextColor(HORI_SP_COLOR, TFT_TRANSPARENT);
        midHoriSp.drawWideLine(HORI_SP_SCALE_L_BEGIN, cur_scale_y, HORI_SP_SCALE_L_BEGIN+HORI_SP_SCALE_W, cur_scale_y,HORI_SP_LINE_WIDTH(0), HORI_SP_COLOR);
        midHoriSp.drawWideLine(HORI_SP_SCALE_R_BEGIN, cur_scale_y, HORI_SP_SCALE_R_BEGIN+HORI_SP_SCALE_W, cur_scale_y,HORI_SP_LINE_WIDTH(0), HORI_SP_COLOR);
        
        
        if(abs(scale_num)<=90 && (abs(scale_num)%(HORI_SP_SCALE_VALUE_INTERVAL*2))==0){
            // midHoriSp.drawString(String(scale_num), left_start_x, cur_scale_y--NO_FLOAT_TEXT_Y_HALF+NO_FLOAT_TEXT_Y_DOWN, 2);
            // midHoriSp.drawString(String(scale_num), HORI_SP_SCALE_20_R_BEGIN, cur_scale_y--NO_FLOAT_TEXT_Y_HALF+NO_FLOAT_TEXT_Y_DOWN, 2);

            midHoriSp.setTextSize(FONT_SIZE);

            int16_t l_x_start = GET_TEXT_FROM_R_START(static_cast<int16_t>(HORI_SP_SCALE_L_BEGIN), font_size, scale_num);
            midHoriSp.setCursor(l_x_start, cur_scale_y-GET_TEXT_Y_HALF(font_size)+GET_TEXT_Y_DOWN(font_size));
            midHoriSp.println(String(scale_num));

            midHoriSp.setCursor(HORI_SP_TEXT_R_BEGIN, cur_scale_y-GET_TEXT_Y_HALF(font_size)+GET_TEXT_Y_DOWN(font_size));
            midHoriSp.println(String(scale_num));
        }
        
    }

}

void moveHoriSp(int16_t pitch_now){
    if(pitch_prev == pitch_now){
        return;
    }
    int pitch_diff = pitch_now-pitch_prev;
    if(abs(pitch_diff)>=90){
        pitch_diff = pitch_diff>0 ? 90 : -90;
    }
    if(pitch_diff == 0){
      return;
    }

    midHoriSp.setTextColor(HORI_SP_COLOR, TFT_TRANSPARENT);
    pitch_prev = pitch_now;

    int16_t font_size = GET_FONT_SIZE(GES_SP_W, GES_SP_H);
    for(int i=1; i <= abs(pitch_diff); i++){
        midHoriSp.scroll(0,pitch_diff>0?HORI_SP_SCALE_PIX_RATE:-HORI_SP_SCALE_PIX_RATE);
        pos_y_hori = pitch_diff>0?pos_y_hori + HORI_SP_SCALE_PIX_RATE:pos_y_hori - HORI_SP_SCALE_PIX_RATE;
        int16_t pos_y_mod = pos_y_hori-HORI_SP_SCROLL_BEGIN_Y-HORI_SP_CROWN_H;
        if((pos_y_mod) % HORI_SP_SCALE_PIX_INTERVAL(0) == 0){
            int pos_y_bu = pitch_diff>0?HORI_SP_SCROLL_BEGIN_Y+HORI_SP_CROWN_H:HORI_SP_SCROLL_BEGIN_Y+HORI_SP_CROWN_H+HORI_SP_SCALE_PIX_INTERVAL(0)*(HORI_SP_SCALE_NUM-1);
            midHoriSp.drawWideLine(HORI_SP_SCALE_L_BEGIN, pos_y_bu, HORI_SP_SCALE_L_BEGIN+HORI_SP_SCALE_W, pos_y_bu,HORI_SP_LINE_WIDTH(0), HORI_SP_COLOR);
            midHoriSp.drawWideLine(HORI_SP_SCALE_R_BEGIN, pos_y_bu, HORI_SP_SCALE_R_BEGIN+HORI_SP_SCALE_W, pos_y_bu,HORI_SP_LINE_WIDTH(0), HORI_SP_COLOR);

            int16_t value;
            if(pitch_diff>0){
                value = HORI_SP_SCALE_VALUE_INTERVAL*((HORI_SP_SCALE_NUM-1)/2)+((pos_y_mod/HORI_SP_SCALE_PIX_INTERVAL(0)))*HORI_SP_SCALE_VALUE_INTERVAL;
            } else {
                value = -HORI_SP_SCALE_VALUE_INTERVAL*((HORI_SP_SCALE_NUM-1)/2)+((pos_y_mod/HORI_SP_SCALE_PIX_INTERVAL(0)))*HORI_SP_SCALE_VALUE_INTERVAL;
            }
            
            
            if(abs(value)<=90 && (abs(value)%(HORI_SP_SCALE_VALUE_INTERVAL*2))==0){
                // midHoriSp.drawString(String(value), left_start_x, pos_y_bu--NO_FLOAT_TEXT_Y_HALF+NO_FLOAT_TEXT_Y_DOWN, 2);
                // midHoriSp.drawString(String(value), HORI_SP_SCALE_20_R_BEGIN, pos_y_bu--NO_FLOAT_TEXT_Y_HALF+NO_FLOAT_TEXT_Y_DOWN, 2);

                midHoriSp.setTextSize(FONT_SIZE);

                int16_t l_x_start = GET_TEXT_FROM_R_START(static_cast<int16_t>(HORI_SP_SCALE_L_BEGIN), font_size, value);
                midHoriSp.setCursor(l_x_start, pos_y_bu-GET_TEXT_Y_HALF(font_size)+GET_TEXT_Y_DOWN(font_size));
                midHoriSp.println(String(value));

                midHoriSp.setCursor(HORI_SP_TEXT_R_BEGIN, pos_y_bu-GET_TEXT_Y_HALF(font_size)+GET_TEXT_Y_DOWN(font_size));
                midHoriSp.println(String(value));
            }
        }
    }
}

String convertYawShow(int yaw){
    if(yaw>=360){
        yaw = yaw -360;
    }
    String sca_value = "";
    if(yaw % 90 == 0){
        if(yaw / 90 == 3){
            sca_value = "W";
        }
        if(yaw / 90 == 2){
            sca_value = "S";
        }
        if(yaw / 90 == 1){
            sca_value = "E";
        }
        if(yaw / 90 == 4 || yaw ==0){
            sca_value = "N";
        }
    } else {
        sca_value = String(yaw);
    }
    return sca_value;
}

void drawYaw(){
    yawSp.setTextColor(GREEN, TFT_TRANSPARENT);

    int16_t font_size = GET_FONT_SIZE(GES_SP_W, GES_SP_H)+1;
    for(int i=1; i<=5; i++){
        int value = (i-1)*YAW_SP_SC_VALUE_INTV-YAW_SP_SC_VALUE_INTV*2;
        value = value<0?360-abs(value):value;
        int pos_x = YAW_SP_BEGIN_X + (i-1)*YAW_SP_SC_PIXEL_INTV;
        int pos_x_text;
        if(value>99){
            pos_x_text = pos_x-FONT2_3_DIGIT_W/2;
        }
        if(value<99){
            pos_x_text = pos_x-FONT2_2_DIGIT_W/2;
            if(value==0){
                pos_x_text = pos_x-FONT2_1_DIGIT_W/2;
            }
        }
        // yawSp.drawString(String(value), pos_x_text, 0, 2);
        yawSp.setTextSize(FONT_SIZE);
        yawSp.setCursor(pos_x_text, 0+GET_TEXT_Y_DOWN(font_size));
        yawSp.println(String(value));
        // yawSp.drawFastVLine(pos_x, 0, YAW_SP_H, GREEN);
    }
    // yawSp.pushToSprite(bgSprP, 0, 0, TFT_TRANSPARENT);
}

/**
 * pox_y =1 top
 * pox_y =2 bottom
 */
void moveYaw(Rgb_Sprite *bgSprP, int16_t yaw_now, int16_t uiType, int16_t pox_y){
    int float_start = GES_SP_W/2-YAW_FLOAT_W/2;
    yawFloatFixSp.fillScreen(uiType ==1?BLACK:YAW_SP_NUM_TEXT_BG_COLOR);
    yawFloatFixSp.drawRect(0,0,YAW_FLOAT_W,YAW_FLOAT_H,WHITE);
    yawFloatFixSp.setTextColor(YAW_SP_NUM_TEXT_COLOR, uiType ==1?BLACK:YAW_SP_NUM_TEXT_BG_COLOR);

    // yawFloatFixSp.drawString(String(yaw_now),pos_x_text,2, 2);
    yawFloatFixSp.setTextSize(FONT_SIZE);

    int16_t font_size = GET_FONT_SIZE(GES_SP_W, GES_SP_H) + 1;
    int16_t l_x_start = GET_TEXT_L_START(static_cast<int16_t>(YAW_FLOAT_W), font_size, yaw_now);

    yawFloatFixSp.setCursor(l_x_start, YAW_FLOAT_H/2-GET_TEXT_Y_HALF(font_size)+GET_TEXT_Y_DOWN(font_size)-2);
    yawFloatFixSp.println(String(yaw_now));


    if(bgSprP != NULL){
        //这里涉及到屏幕的多层控件显示, 顶层控件作为底层控件的指针参数传进去
        if(pox_y == 1){
            yawFloatFixSp.pushToSprite(bgSprP, GES_SP_W/2-YAW_FLOAT_W/2,0, TFT_TRANSPARENT);
        } else {
            yawFloatFixSp.pushToSprite(bgSprP, YAW_ROTATE_SP_BG_W/2-YAW_FLOAT_W/2, YAW_ROTATE_SP_BG_H-YAW_FLOAT_H-4, TFT_TRANSPARENT);
        }
    } else {
        // yawSp.pushSprite(0, 0);
        // yawSp.pushSprite(0, 0, TFT_TRANSPARENT);
        yawFloatFixSp.pushSprite(GES_SP_W/2-YAW_FLOAT_W/2,YAW_FLOAT_H);
    }

    // int yaw_diff = yaw_now - yaw_prev;
    // if(abs(yaw_diff)>300){
    //     yaw_diff = abs(yaw_diff)-360;
    // }
    
    // for(int i=1;i<abs(yaw_diff);i++){
    //     yawSp.scroll(yaw_diff>0?-2:2,0);//yaw 变大，向左滚动

    //     pos_x_yaw = yaw_diff>0?pos_x_yaw-2:pos_x_yaw+2;
    //     int pos_x_mod = pos_x_yaw-YAW_SP_BEGIN_X;
    //     if(abs(pos_x_mod) % YAW_SP_SC_PIXEL_INTV ==0){
    //         int pos_x_bu = yaw_diff>0?(GES_SP_W-YAW_SP_BEGIN_X):YAW_SP_BEGIN_X;
            
    //         int value;
    //         if(yaw_diff>0){//yaw 变大，向左滚动, 补右边的值
    //             value = YAW_SP_SC_VALUE_INTV*2-((pos_x_mod/YAW_SP_SC_PIXEL_INTV))*YAW_SP_SC_VALUE_INTV;
    //         } else {
    //             value = YAW_SP_SC_VALUE_INTV*(-2)-((pos_x_mod/YAW_SP_SC_PIXEL_INTV))*YAW_SP_SC_VALUE_INTV;
    //         }
    //         value = value<0?360-abs(value): value;
    //         value = value>=360?value-360:value;

    //         int pos_x_text;
    //         if(value>99){
    //             pos_x_text = pos_x_bu-FONT2_3_DIGIT_W/2;
    //         }
    //         if(value<99){
    //             pos_x_text = pos_x_bu-FONT2_2_DIGIT_W/2;
    //             if(value==0){
    //                 pos_x_text = pos_x_bu-FONT2_1_DIGIT_W/2;
    //             }
    //         }
    //         // yawSp.drawString(String(value), pos_x_text, 0, 2);

    //         yawFloatFixSp.setTextSize(FONT_SIZE);
    //         yawFloatFixSp.setCursor(pos_x_text, 0+TEXT_Y_DOWN);
    //         yawFloatFixSp.println(String(value));
    //     }
    // }
    // yaw_prev = yaw_now;
}


void drawAirSpeed(){
    //空速刻度上的最大刻度数,总的刻度长度是 40, 刻度长度的像素长度是 120, 因此大刻度长 30 像素, 小刻度长 3个像素
    //画出小刻度
    for(int i = 1; i<= 20; i++){
        //画出小的刻度数值
        int cur_scale_y = TWO_SP_BEGIN_DRAW_Y + (i-1) * TWO_SP_SMALL_SCALE_GAP;
        //空速的刻度上的大的指标 x 坐标, 小的指标 x 坐标
        airSpeedSp.drawLine(TWO_SP_WIDTH-TWO_SP_SMALL_SCALE_WIDTH, cur_scale_y, TWO_SP_WIDTH, cur_scale_y, TWO_SP_SCALE_LINE_COLOR);
    }
    //画出大刻度

    int16_t font_size = GET_FONT_SIZE(GES_SP_W, GES_SP_H);

    for(int i = 1; i<= 5; i++){
        //画出大的刻度数值
        int cur_scale_y = TWO_SP_BEGIN_DRAW_Y + (i-1) * TWO_SP_BIG_SCALE_GAP;
        //每个大刻度显示的刻度数值
        int16_t scale_num = TWO_SP_BIG_SCALE_GAP*2-(i-1)*TWO_SP_BIG_SCALE_GAP;
        airSpeedSp.setTextColor(TWO_SP_SCALE_TEXT_COLOR, TFT_TRANSPARENT);

        // airSpeedSp.drawString(String(scale_num), scale_num==0?18:10, cur_scale_y-(abs(scale_num)>99?2:6), abs(scale_num)>99?1:2);
        airSpeedSp.setTextSize(FONT_SIZE);

        int16_t l_x_start = GET_TEXT_FROM_R_START(static_cast<int16_t>(TWO_SP_WIDTH-TWO_SP_BIG_SCALE_WIDTH), font_size, scale_num);
        airSpeedSp.setCursor(l_x_start, cur_scale_y-GET_TEXT_Y_HALF(font_size)+GET_TEXT_Y_DOWN(font_size));
        airSpeedSp.println(String(scale_num));

        //空速的刻度上的大的指标 x 坐标, 小的指标 x 坐标
        airSpeedSp.drawWideLine(TWO_SP_WIDTH-TWO_SP_BIG_SCALE_WIDTH, cur_scale_y, TWO_SP_WIDTH, cur_scale_y,TWO_SP_BIG_SCALE_LINE_WIDTH(0), TWO_SP_SCALE_LINE_COLOR);
    }
}

void drawAltitude(){

    //空速刻度上的最大刻度数,总的刻度长度是 40, 刻度长度的像素长度是 120, 因此大刻度长 30 像素, 小刻度长 3个像素
    //画出小刻度

    int16_t font_size = GET_FONT_SIZE(GES_SP_W, GES_SP_H);

    for(int i = 1; i<= 20; i++){
        //画出小的刻度数值
        int cur_scale_y = TWO_SP_BEGIN_DRAW_Y + (i-1) * TWO_SP_SMALL_SCALE_GAP;
        //空速的刻度上的大的指标 x 坐标, 小的指标 x 坐标
        altitudeSp.drawLine(0, cur_scale_y, TWO_SP_SMALL_SCALE_WIDTH, cur_scale_y, TWO_SP_SCALE_LINE_COLOR);
    }
    //画出大刻度
    for(int i = 1; i<= 5; i++){
        //画出大的刻度数值
        int cur_scale_y = TWO_SP_BEGIN_DRAW_Y + (i-1) * TWO_SP_BIG_SCALE_GAP;
        //每个大刻度显示的刻度数值
        int scale_num = TWO_SP_BIG_SCALE_GAP*2-(i-1)*TWO_SP_BIG_SCALE_GAP;
        altitudeSp.setTextColor(TWO_SP_SCALE_TEXT_COLOR, TFT_TRANSPARENT);

        // altitudeSp.drawString(String(scale_num), TWO_SP_BIG_SCALE_WIDTH, cur_scale_y-(abs(scale_num)>99?2:6), abs(scale_num)>99?1:2);
        altitudeSp.setTextSize(FONT_SIZE);
        altitudeSp.setCursor(TWO_SP_BIG_SCALE_WIDTH, cur_scale_y-GET_TEXT_Y_HALF(font_size)+GET_TEXT_Y_DOWN(font_size));
        altitudeSp.println(String(scale_num));

        //空速的刻度上的大的指标 x 坐标, 小的指标 x 坐标
        altitudeSp.drawWideLine(0, cur_scale_y, TWO_SP_BIG_SCALE_WIDTH, cur_scale_y,TWO_SP_BIG_SCALE_LINE_WIDTH(0), TWO_SP_SCALE_LINE_COLOR);
    }
}

//这里的参数有 *bgSprP 是因为不同的ui背景，airSpeedSp 需要pushToSprite 的背景对象不同
void moveAirSpeedGraph(Rgb_Sprite *bgSprP, int16_t air_speed_now, int16_t uiType){
    int16_t float_start_y = TWO_SP_HEIGHT/2-TWO_SP_FLOAT_H(0)/2;
    // int float_start_y = TWO_SP_HEIGHT/2;
    airSpeedFloatSp.fillRect(0,float_start_y,TWO_SP_FLOAT_W,TWO_SP_FLOAT_H(0),uiType==1?BLACK:TWO_SP_FLOAT_BG_COLOR);
    airSpeedFloatSp.drawRect(0,float_start_y,TWO_SP_FLOAT_W,TWO_SP_FLOAT_H(0),WHITE);
    airSpeedFloatSp.setTextColor(TWO_SP_FLOAT_TEXT_COLOR, uiType==1?BLACK:TWO_SP_FLOAT_BG_COLOR);
    // airSpeedFloatSp.drawString(String(air_speed_now),4,float_start_y + 2, 2);

    airSpeedFloatSp.setTextSize(FONT_SIZE);

    int16_t font_size = GET_FONT_SIZE(GES_SP_W, GES_SP_H);
    int16_t l_x_start = GET_TEXT_FROM_R_START(static_cast<int16_t>(TWO_SP_WIDTH), static_cast<int16_t>(font_size+1), air_speed_now);
    if(air_speed_now < -99){
        l_x_start = l_x_start - TWO_SP_WIDTH/5;
    }

    airSpeedFloatSp.setCursor(l_x_start, TWO_SP_HEIGHT/2-GET_TEXT_Y_HALF(font_size+1)+GET_TEXT_Y_DOWN(font_size+1));
    airSpeedFloatSp.println(String(air_speed_now));

    if(bgSprP != NULL){
        airSpeedSp.pushToSprite(bgSprP, 0, TWO_SP_MARGIN_TOP-TWO_SP_BEGIN_DRAW_Y,TFT_TRANSPARENT);
        airSpeedFloatSp.pushToSprite(bgSprP, 0, TWO_SP_MARGIN_TOP-TWO_SP_BEGIN_DRAW_Y,TFT_TRANSPARENT);
        
    } else {
        // airSpeedFloatSp.pushToSprite(&airSpeedSp, 0,0,TFT_TRANSPARENT);
        airSpeedSp.pushSprite(0, TWO_SP_MARGIN_TOP-TWO_SP_BEGIN_DRAW_Y);
        // airSpeedSp.pushSprite(0, TWO_SP_MARGIN_TOP-TWO_SP_BEGIN_DRAW_Y, TFT_TRANSPARENT);
    }
    
    //确定滚动区域的起始坐标,区域是从坐标向右下角展开
    int16_t speed_diff = air_speed_now-air_speed_prev;

    if(speed_diff == 0){
        return;
    }
    air_speed_prev = air_speed_now;
    for(int16_t i=1;i<=abs(speed_diff);i++){

        airSpeedSp.scroll(0, speed_diff>0?1:-1);
        
        pos_y_air = speed_diff>0?pos_y_air + 1:pos_y_air - 1;
        if((pos_y_air-TWO_SP_BEGIN_DRAW_Y) % TWO_SP_SMALL_SCALE_GAP == 0){
            int pos_y_bu = speed_diff>0?TWO_SP_BEGIN_DRAW_Y:TWO_SP_BEGIN_DRAW_Y+TWO_SP_BIG_SCALE_GAP*4;
            airSpeedSp.drawLine(TWO_SP_WIDTH-TWO_SP_SMALL_SCALE_WIDTH,pos_y_bu,TWO_SP_WIDTH,pos_y_bu,TWO_SP_SCALE_LINE_COLOR); //补上小刻度
            if((pos_y_air-TWO_SP_BEGIN_DRAW_Y) % TWO_SP_BIG_SCALE_GAP == 0){
                int pos_y_bu = speed_diff>0?TWO_SP_BEGIN_DRAW_Y:TWO_SP_BEGIN_DRAW_Y+TWO_SP_BIG_SCALE_GAP*4;
                //补大刻度
                airSpeedSp.drawWideLine(TWO_SP_WIDTH-TWO_SP_BIG_SCALE_WIDTH,pos_y_bu,TWO_SP_WIDTH,pos_y_bu,TWO_SP_BIG_SCALE_LINE_WIDTH(0),TWO_SP_SCALE_LINE_COLOR); //补上大刻度
                int16_t value = speed_diff>0?pos_y_air-TWO_SP_BEGIN_DRAW_Y+TWO_SP_BIG_SCALE_GAP*2:pos_y_air-TWO_SP_BEGIN_DRAW_Y-TWO_SP_BIG_SCALE_GAP*2;
                airSpeedSp.setTextSize(FONT_SIZE);

                int16_t l_x_start = GET_TEXT_FROM_R_START(static_cast<int16_t>(TWO_SP_WIDTH-TWO_SP_BIG_SCALE_WIDTH), font_size, value);
                airSpeedSp.setCursor(l_x_start, pos_y_bu-GET_TEXT_Y_HALF(font_size)+GET_TEXT_Y_DOWN(font_size));
                airSpeedSp.println(String(value));
            }
        }
    }
}


void moveAltitudeGraph(Rgb_Sprite *bgSprP, int16_t altitude_now, int16_t uiType){
    int16_t float_start_y = TWO_SP_HEIGHT/2-TWO_SP_FLOAT_H(0)/2;
    altitudeFloatSp.fillRect(0,float_start_y,TWO_SP_FLOAT_W,TWO_SP_FLOAT_H(0),uiType==1?BLACK:TWO_SP_FLOAT_BG_COLOR);
    altitudeFloatSp.drawRect(0,float_start_y,TWO_SP_FLOAT_W,TWO_SP_FLOAT_H(0),WHITE);
    altitudeFloatSp.setTextColor(TWO_SP_FLOAT_TEXT_COLOR, uiType==1?BLACK:TWO_SP_FLOAT_BG_COLOR);
    // altitudeFloatSp.drawString(String(altitude_now),4,float_start_y + 2, 2);

    altitudeFloatSp.setTextSize(FONT_SIZE);

    int16_t font_size = GET_FONT_SIZE(GES_SP_W, GES_SP_H);


    int16_t pos_x;
    if((font_size+1) == 3 && altitude_now<-99){
        pos_x =0;
    } else {
        pos_x = 4;
    }
    altitudeFloatSp.setCursor(pos_x,TWO_SP_HEIGHT/2 -GET_TEXT_Y_HALF(font_size+1)+GET_TEXT_Y_DOWN(font_size+1));
    altitudeFloatSp.println(String(altitude_now));

    if(bgSprP != NULL){
    //     //第二层绑定到背景，然后最顶层再绑定到背景
        altitudeSp.pushToSprite(bgSprP, GES_SP_W-TWO_SP_WIDTH, TWO_SP_MARGIN_TOP-TWO_SP_BEGIN_DRAW_Y,TFT_TRANSPARENT);
        altitudeFloatSp.pushToSprite(bgSprP, GES_SP_W-TWO_SP_WIDTH, TWO_SP_MARGIN_TOP-TWO_SP_BEGIN_DRAW_Y,TFT_TRANSPARENT);
    } else {
    //     altitudeFloatSp.pushToSprite(&altitudeSp, 0,0,TFT_TRANSPARENT);
        altitudeSp.pushSprite(GES_SP_W-TWO_SP_WIDTH, TWO_SP_MARGIN_TOP-TWO_SP_BEGIN_DRAW_Y);
        // altitudeSp.pushSprite(GES_SP_W-TWO_SP_WIDTH, TWO_SP_MARGIN_TOP-TWO_SP_BEGIN_DRAW_Y, TFT_TRANSPARENT);
    }
    
    //确定滚动区域的起始坐标,区域是从坐标向右下角展开
    long altitude_diff = altitude_now-altitude_prev;

    if(altitude_diff == 0){
        return;
    }
    altitude_prev = altitude_now;
    for(long i=1;i<=abs(altitude_diff);i++){

        altitudeSp.scroll(0, altitude_diff>0?1:-1);
        
        pos_y_alti = altitude_diff>0?pos_y_alti + 1:pos_y_alti - 1;
        if((pos_y_alti-TWO_SP_BEGIN_DRAW_Y) % TWO_SP_SMALL_SCALE_GAP == 0){
            int pos_y_bu = altitude_diff>0?TWO_SP_BEGIN_DRAW_Y:TWO_SP_BEGIN_DRAW_Y+TWO_SP_BIG_SCALE_GAP*4;
            altitudeSp.drawLine(0,pos_y_bu,TWO_SP_SMALL_SCALE_WIDTH,pos_y_bu,TWO_SP_SCALE_LINE_COLOR); //补上小刻度
            if((pos_y_alti-TWO_SP_BEGIN_DRAW_Y) % TWO_SP_BIG_SCALE_GAP == 0){
                int pos_y_bu = altitude_diff>0?TWO_SP_BEGIN_DRAW_Y:TWO_SP_BEGIN_DRAW_Y+TWO_SP_BIG_SCALE_GAP*4;
                //补大刻度
                altitudeSp.drawWideLine(0,pos_y_bu,TWO_SP_BIG_SCALE_WIDTH,pos_y_bu,TWO_SP_BIG_SCALE_LINE_WIDTH(0),TWO_SP_SCALE_LINE_COLOR); //补上大刻度
            
                int value = altitude_diff>0?pos_y_alti-TWO_SP_BEGIN_DRAW_Y+TWO_SP_BIG_SCALE_GAP*2:pos_y_alti-TWO_SP_BEGIN_DRAW_Y-TWO_SP_BIG_SCALE_GAP*2;
                // altitudeSp.drawString(String(value), TWO_SP_BIG_SCALE_WIDTH, pos_y_bu-(abs(value)>99?2:6), abs(value)>99?1:2);

                altitudeSp.setTextSize(FONT_SIZE);
                altitudeSp.setCursor(TWO_SP_BIG_SCALE_WIDTH, pos_y_bu-GET_TEXT_Y_HALF(font_size)+GET_TEXT_Y_DOWN(font_size));
                altitudeSp.println(String(value));
            }
        }
    }
}

void gestureInit(int16_t x, int16_t y){
    //初始化显示开始
//   drawSkyGroudAngle(&bgSprAllHori, 0, 180);
    // rorateHorizon(&bgSprAllHori, 0);
    drawSkyGroudAngleByCircle(&bgSprAllHori, 0, 0, (max(GES_SP_W,GES_SP_H)/2)*1.41, GES_SP_W/2, GES_SP_H/2);//带有地平线的背景

    drawAirSpeed();
    drawAltitude();
    drawHorizon();
    // drawYawRotateSp(x, y, false);
    // drawYaw();
    
}


/**
 * bgType 背景显示的类型
 * 1 全黑的背景
 * 2 全屏幕的地平线背景
 * 3 带椭圆形的地平线背景
 */
void drawGestureByData(int bgType, ges_data_t ges_data, int x, int y) {
    // mavlink_message_t msg_hb;
    // uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    // mavlink_msg_heartbeat_pack(1, 1, &msg_hb, 2, 0, 64, 0, MAV_STATE_STANDBY);
    // uint16_t len = mavlink_msg_to_send_buffer(buf, &msg_hb);
    // Serial2.write(buf, len);
    // // delay(10);
    // mavlink_send_request();
      if(bgType == 1){
          bgSprAllHori.fillScreen(BLACK);//全黑的背景
      }
      if(bgType == 2){
          drawSkyGroudAngleByCircle(&bgSprAllHori, ges_data.pitch, ges_data.roll, (max(GES_SP_W,GES_SP_H)/2)*1.41, GES_SP_W/2, GES_SP_H/2);//带有地平线的背景
      }
      if(bgType == 3){
          bgSprAllHori.fillScreen(BLACK);//全黑的背景
          drawSkyGroudAngleByCircle(&bgSprEllipse, ges_data.pitch, ges_data.roll, ((max(ELLIPSE_SP_W,static_cast<int>(ELLIPSE_SP_H(0))))/2)*ELLIPSE_SP_RADIUS_RATE(0), ELLIPSE_SP_W/2, ELLIPSE_SP_H(0)/2);
      }
      
      if(YAW_SP_COMPASS_SHOW == 1){
          if(bgType == 1){
              if(!has_init_yaw_bg_1){
                  drawYawRotateSp(x, y, false);
                  has_init_yaw_bg_1 = true;
              }
          }
          if(bgType == 2){
              if(!has_init_yaw_bg_2){
                  drawYawRotateSp(x, y, false);
                  has_init_yaw_bg_2 = true;
              }
          }
          if(bgType == 3){
              if(!has_init_yaw_bg_3){
                  drawYawRotateSp(x, y, true);
                  has_init_yaw_bg_3 = true;
              }
          }
          if(bgType < 3){
              yawRotateNoFillSp.setPivot(YAW_ROTATE_SP_RADIUS, YAW_ROTATE_SP_RADIUS);
              yawRotateNoFillSp.pushRotated(&yawRotateBgSp, 360-ges_data.yaw);
          } else {
              yawRotateSp.setPivot(YAW_ROTATE_SP_RADIUS, YAW_ROTATE_SP_RADIUS);
              yawRotateSp.pushRotated(&yawRotateBgSp, 360-ges_data.yaw);
          }
          
          moveYaw(&yawRotateBgSp, ges_data.yaw, bgType, 2);
          yawRotateBgSp.pushToSprite(&bgSprAllHori, 0, y+(GES_SP_H-YAW_ROTATE_SP_BG_H), TFT_TRANSPARENT);
      }
      
      moveYaw(&bgSprAllHori, ges_data.yaw, bgType, 1);
  
      if(bgType == 3){
          airSpeedSpBg.fillScreen(TWO_SP_SCALE_LINE_BG_COLOR);
          altitudeSpBg.fillScreen(TWO_SP_SCALE_LINE_BG_COLOR);
          airSpeedSpBg.pushToSprite(&bgSprAllHori, 0, TWO_SP_MARGIN_TOP-TWO_SP_BEGIN_DRAW_Y,TFT_TRANSPARENT);
          altitudeSpBg.pushToSprite(&bgSprAllHori, GES_SP_W-TWO_SP_WIDTH, TWO_SP_MARGIN_TOP-TWO_SP_BEGIN_DRAW_Y,TFT_TRANSPARENT);
      }

      moveAirSpeedGraph(&bgSprAllHori, LEFT_FLOAT_SP_SHOW==1?ges_data.air_speed:0, bgType);

      moveAltitudeGraph(&bgSprAllHori, RIGHT_FLOAT_SP_SHOW==1?ges_data.altitude:0, bgType);
      
      moveHoriSp(ges_data.pitch);
  
      if(bgType < 3){
          midHoriSp.pushRotated(&bgSprAllHori, ges_data.roll, TFT_TRANSPARENT);
          midHoriLineSp.pushToSprite(&bgSprAllHori, GES_SP_W/2-HORI_SP_H_LINE_W/2,GES_SP_H/2, TFT_TRANSPARENT);
      } else {
          midHoriSp.pushRotated(&bgSprEllipse, ges_data.roll, TFT_TRANSPARENT);
          midHoriLineSp.pushToSprite(&bgSprEllipse, ELLIPSE_SP_W/2-HORI_SP_H_LINE_W/2,ELLIPSE_SP_H(0)/2, TFT_TRANSPARENT);
          bgSprEllipse.pushToSprite(&bgSprAllHori, (GES_SP_W-ELLIPSE_SP_W)/2,GES_SP_H/2-ELLIPSE_SP_H(0)/2);
      }
  
      if(YAW_SP_COMPASS_SHOW == 1){
          yawRotateVLineSp.pushToSprite(&bgSprAllHori, GES_SP_W/2, y+YAW_RORATE_SP_V_LINE_Y, TFT_TRANSPARENT);
      }
      bgSprAllHori.pushSprite(x, y);
  
  }