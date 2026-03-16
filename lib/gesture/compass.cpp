#include "compass.h"

extern Rgb_Sprite compassSp;
extern Rgb_Sprite compassBgSp;
extern Rgb_Sprite compassTextSp;

extern Rgb_Sprite compassMidTextSp;
extern Rgb_Sprite compassPointertSp;
extern Rgb_Sprite compassCoverSp;

void drawDotLineC(Rgb_Sprite *bgSprP, uint16_t pos_x_mid, uint16_t pos_y_mid, uint16_t radius, int angle, int lenght, float ratio, uint16_t color){
    float start_x = pos_x_mid + radius*ratio*sin(radians(angle));
    float start_y = pos_y_mid - radius*ratio*cos(radians(angle));

    //算出内圈点的结束点
    float end_x   = pos_x_mid + (radius*ratio - lenght) * sin(radians(angle));
    float end_y   = pos_y_mid - (radius*ratio - lenght) * cos(radians(angle));
    bgSprP->drawLine(start_x, start_y, end_x, end_y, color);
}

void drawCompass(){
    compassSp.fillScreen(BLACK);

    compassSp.fillCircle(COMPS_SP_RADIUS, COMPS_SP_RADIUS, COMPS_SP_RADIUS, COMPS_SP_BG_COLOR);
    
    
    compassSp.drawCircle(COMPS_SP_RADIUS, COMPS_SP_RADIUS, COMPS_SP_RADIUS, YAW_SP_SCALE_LINE_COLOR);

    compassTextSp.setTextSize(1);

    uint16_t radius = COMPS_SP_RADIUS;
    for(int i=0;i<360;i=i+COMPS_SP_SCALE_S_GAP){
        if(i % COMPS_SP_SCALE_B_GAP ==0){
            drawDotLineC(&compassSp, radius,radius,radius,i,COMPS_SP_SCALE_B_LEN,1, COMPS_SP_SCALE_COLOR);
        } else {
            drawDotLineC(&compassSp, radius,radius,radius,i,COMPS_SP_SCALE_S_LEN,1, COMPS_SP_SCALE_COLOR);
        }
        
    }

    int16_t font_size = GET_FONT_SIZE(COMPS_SP_W, COMPS_SP_H);

    for(int16_t i=0;i<360;i=i+30){
        float end_x   = COMPS_SP_RADIUS + (COMPS_SP_RADIUS - COMPS_SP_SCALE_B_LEN*1.5) * sin(radians(i));
        float end_y   = COMPS_SP_RADIUS - (COMPS_SP_RADIUS - COMPS_SP_SCALE_B_LEN*1.5) * cos(radians(i));
        compassTextSp.setTextColor(COMPS_SP_TEXT_COLOR, TFT_TRANSPARENT);
        compassTextSp.fillScreen(TFT_TRANSPARENT);
        int16_t l_x_start = GET_TEXT_L_START(static_cast<int16_t>(COMPS_TEXT_SP_W), font_size, i);
        int16_t y_down = COMPS_TEXT_SP_H/2-GET_TEXT_Y_HALF(font_size)+GET_TEXT_Y_DOWN(font_size);
        compassTextSp.setCursor(l_x_start, y_down);
        compassTextSp.println(String(i));

        compassSp.setPivot(end_x, end_y);
        compassTextSp.pushRotated(&compassSp, i, TFT_TRANSPARENT);
    }


    compassCoverSp.fillRoundRect(COMPS_SP_W*0.45, COMPS_SP_H*0.16, COMPS_SP_W*0.095, COMPS_SP_H*0.59, COMPS_SP_W*0.06, GREEN);

    //左边的大机翼
    compassCoverSp.fillTriangle(COMPS_SP_W*0.45, COMPS_SP_H*0.37, COMPS_SP_W*0.45, COMPS_SP_H*0.5, COMPS_SP_W*0.2, COMPS_SP_H*0.58, GREEN);
    //左边的尾翼
    compassCoverSp.fillTriangle(COMPS_SP_W*0.45, COMPS_SP_H*0.7, COMPS_SP_W*0.5, COMPS_SP_H*0.75, COMPS_SP_H*0.37, COMPS_SP_H*0.77, GREEN);
    //右边的大机翼
    compassCoverSp.fillTriangle(COMPS_SP_W*0.54, COMPS_SP_H*0.37, COMPS_SP_W*0.54, COMPS_SP_H*0.5, COMPS_SP_W*0.79, COMPS_SP_H*0.58, GREEN);
    //右边的尾翼
    compassCoverSp.fillTriangle(COMPS_SP_W*0.54, COMPS_SP_H*0.7, COMPS_SP_W*0.5, COMPS_SP_H*0.75, COMPS_SP_W*0.62, COMPS_SP_H*0.77, GREEN);
}

void ratateCompass(int16_t yaw, int16_t x, int16_t y){
    compassSp.setPivot(COMPS_SP_RADIUS, COMPS_SP_RADIUS);
    compassSp.pushRotated(&compassBgSp, 360-yaw, TFT_TRANSPARENT);
    // compassPointertSp.pushRotated(&compassBgSp, 0, TFT_TRANSPARENT);
    compassCoverSp.pushRotated(&compassBgSp, 0, TFT_TRANSPARENT);

    int16_t font_size = GET_FONT_SIZE(COMPS_SP_W, COMPS_SP_H);
    int16_t mid_size = 0;
    if(font_size <= 2){
        mid_size = 4;
    } else if(font_size == 3){
        mid_size = 6;
    } else if(font_size == 4){
        mid_size = 8;
    }

    int16_t l_x_start = GET_TEXT_L_START(static_cast<int16_t>(COMPS_MID_TEXT_SP_W), mid_size, yaw);
    int16_t y_down = COMPS_MID_TEXT_SP_H/2-GET_TEXT_Y_HALF(mid_size)+GET_TEXT_Y_DOWN(mid_size);

    
    compassBgSp.pushSprite(x, y);
}