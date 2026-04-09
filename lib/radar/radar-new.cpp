
#include "radar-new.h"

// extern TFT_eSPI    tft;
extern Rgb_Sprite radarBgSp;
// extern Rgb_Sprite radarBgFrameSp;
extern Rgb_Sprite radarPointerSp;
extern Rgb_Sprite radarPSmallSp;

// uint16_t *bg_frame_buffer = new uint16_t[RADAR_SP_W * RADAR_SP_H];

uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc){
    // Split out and blend 5 bit red and blue channels
    uint32_t rxb = bgc & 0xF81F;
    rxb += ((fgc & 0xF81F) - rxb) * (alpha >> 2) >> 6;
    // Split out and blend 6 bit green channel
    uint32_t xgx = bgc & 0x07E0;
    xgx += ((fgc & 0x07E0) - xgx) * alpha >> 8;
    // Recombine channels
    return (rxb & 0xF81F) | (xgx & 0x07E0);
}

// 画出雷达右下角的扫描的度数
static void draw_angle(const String& string, int32_t x, int32_t y, uint8_t font){
    radarBgSp.setTextColor(GREEN, BLACK);
    //这里的显示数字是以坐标开始向右下方显示的
    radarBgSp.drawString(string, x, y, font);
}

//画出东西南北4个方向的字符
static void draw_direction_string(const String& string){
    radarBgSp.setTextColor(GREEN, BLACK);
    if(string.compareTo("N")){
      radarBgSp.drawString("N", RADAR_SP_W/2-8, 0, 4);
    }
    if(string.compareTo("E")){
      radarBgSp.drawString("E", RADAR_SP_W-15, RADAR_SP_H/2-10, 4);
    }
    if(string.compareTo("S")){
      radarBgSp.drawString("S", RADAR_SP_W/2-8, RADAR_SP_H-20, 4);
    }
    if(string.compareTo("W")){
      radarBgSp.drawString("W", 0, RADAR_SP_H/2-10, 4);
    }
}

static float calc_x_pos(uint16_t radius, int angle){
  return radius + radius*sin(radians(angle));
}
static float calc_y_pos(uint16_t radius, int angle){
  return radius - radius*cos(radians(angle));
}

/*
画出圆上的直线，相当于圆上的刻度
radius 半径
angle 角度
lenght 刻度的长度，单位为像素
ratio 在半径多长的比例的圆形上面
*/
static void drawDotLine(Rgb_Sprite *radarBgSp, uint16_t count, uint16_t radius, uint16_t lenght, float ratio){
    //每个点数之间的角度
    uint16_t angle = 360 / count;
    for(uint16_t i = angle; i<360;i = i + angle) {
        //先算出外圈点的开始点, 根据正余弦计算
        float start_x = radius + radius*ratio*sin(radians(angle));
        float start_y = radius - radius*ratio*cos(radians(angle));

        //算出内圈点的结束点
        float end_x   = radius + (radius*ratio - lenght) * sin(radians(angle));
        float end_y   = radius - (radius*ratio - lenght) * cos(radians(angle));
        radarBgSp->drawLine(start_x, start_y, end_x, end_y, GREEN);
    }
    
}

//这里画横竖，2个斜线，一共4条线，按照顺时针分成8条线
void drawCrossLine(Rgb_Sprite *radarBgSp){
    for (uint16_t i=0; i<= 360; i = i + 45){
        // radarBgSp->drawWideLine(RADAR_SP_W/2, RADAR_SP_H/2, calc_x_pos(RADAR_RADIUS, i), calc_y_pos(RADAR_RADIUS, i), CROSS_LINE_W, GREEN);
        radarBgSp->drawLine(RADAR_SP_W/2, RADAR_SP_H/2, calc_x_pos(RADAR_RADIUS, i), calc_y_pos(RADAR_RADIUS, i), GREEN);
    }
}

void draw_radar_bg_sp(){
    // radarBgSp.setTextColor(RADAR_TEXT_COLOR, TFT_TRANSPARENT);
    radarBgSp.fillScreen(BLACK);

    //雷达背景雾化效果, 这个 alphaBlend 函数会导致画面抖动
    for(uint16_t i=1;i<=RADAR_RADIUS;i++){
      if(i%4==0){
          radarBgSp.drawCircle(RADAR_SP_W/2, RADAR_SP_H/2, i, alphaBlend(RADAR_RADIUS-i,  RADAR_SCALE_COLOR, BLACK));
      }
    }

    radarBgSp.drawCircle(RADAR_SP_W/2, RADAR_SP_H/2, RADAR_RADIUS-2, RADAR_SCALE_COLOR);
    radarBgSp.drawCircle(RADAR_SP_W/2, RADAR_SP_H/2, RADAR_RADIUS-3, RADAR_SCALE_COLOR);
    radarBgSp.drawCircle(RADAR_SP_W/2, RADAR_SP_H/2, RADAR_RADIUS*0.66, RADAR_SCALE_COLOR);
    radarBgSp.drawCircle(RADAR_SP_W/2, RADAR_SP_H/2, RADAR_RADIUS*0.66-1, RADAR_SCALE_COLOR);
    radarBgSp.drawCircle(RADAR_SP_W/2, RADAR_SP_H/2, RADAR_RADIUS*0.33, RADAR_SCALE_COLOR);
    radarBgSp.drawCircle(RADAR_SP_W/2, RADAR_SP_H/2, RADAR_RADIUS*0.33-1, RADAR_SCALE_COLOR);
    // drawDotLine(&radarBgSp, RADAR_SCALE_DOT_NUM, RADAR_RADIUS, RADAR_SCALE_DOT_LEN, 1);
    drawCrossLine(&radarBgSp);

    // radarBgSp.drawString("N", RADAR_SP_W/2-8, 0, 4);
    // radarBgSp.drawString("E", RADAR_SP_W-15, RADAR_SP_H/2-10, 4);
    // radarBgSp.drawString("S", RADAR_SP_W/2-8, RADAR_SP_H-20, 4);
    // radarBgSp.drawString("W", 0, RADAR_SP_H/2-10, 4);
}

void draw_pointer_sp(){
    //绘制扫描的指针
    // uint16_t d = 2, p = 0;
    // for(uint16_t i=1;i<=RADAR_POINTER_ANGLE;i++){
      
    //     float x_end = RADAR_RADIUS + (RADAR_RADIUS)*sin(radians(i));
    //     float y_end = RADAR_RADIUS - (RADAR_RADIUS)*cos(radians(i));

    //     p = p + d;
    //     d = d + 2;
    //     if(p>255){
    //       p = 255;
    //     }
    //     radarPointerSp.drawLine(RADAR_SP_W/2, RADAR_SP_H/2, x_end, y_end, alphaBlend(p,  GREEN, BLACK));
    //     // radarPointerSp.drawLine(RADAR_SP_W/2, RADAR_SP_H/2, x_end, y_end, GREEN);
    // }

    radarPSmallSp.fillScreen(GREEN);

}

void radarInitNew(){

    draw_radar_bg_sp();

    //背景图形绘制好之后就复制帧出来，然后删掉释放内存
    // memcpy(bg_frame_buffer, radarBgSp.getFramebuffer(), RADAR_SP_W * RADAR_SP_H * sizeof(uint16_t));
    draw_pointer_sp();

    // radarBgSp.pushSprite(580,40);
}

void rotatePointer(uint16_t angle, int16_t x, int16_t y){
    // radarBgSp.draw16bitRGBBitmap(0,0,bg_frame_buffer, RADAR_SP_W, RADAR_SP_H);
    draw_radar_bg_sp();


    // radarPointerSp.pushRotated(&radarBgSp, angle, TFT_TRANSPARENT);
    radarPSmallSp.pushRotated(&radarBgSp, angle, TFT_TRANSPARENT);
    radarBgSp.pushSprite(x,y);

}
