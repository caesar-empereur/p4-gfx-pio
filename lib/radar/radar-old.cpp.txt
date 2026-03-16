#include "radar-old.h"


extern TFT_eSPI    tft;

uint16_t width=240, height=240, edge=2;
uint16_t radius=min(width,height)/2;

int show_angle = 20;
float cross_line_width = 2;

// 画出雷达右下角的扫描的度数
static void draw_angle(const String& string, int32_t x, int32_t y, uint8_t font){
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.fillRect(220,220, 20, 20, TFT_BLACK);
    //这里的显示数字是以坐标开始向右下方显示的
    tft.drawString(string, x, y, font);
}

//画出东西南北4个方向的字符
static void draw_direction_string(const String& string){
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  if(string.compareTo("N")){
    tft.drawString("N", width/2-8, 0, 4);
  }
  if(string.compareTo("E")){
    tft.drawString("E", width-15, height/2-10, 4);
  }
  if(string.compareTo("S")){
    tft.drawString("S", width/2-8, height-20, 4);
  }
  if(string.compareTo("W")){
    tft.drawString("W", 0, height/2-10, 4);
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
static void drawDotLine(uint16_t radius, int angle, int lenght, float ratio){
    //先算出外圈点的开始点, 根据正余弦计算
    float start_x = radius + radius*ratio*sin(radians(angle));
    float start_y = radius - radius*ratio*cos(radians(angle));

    //算出内圈点的结束点
    float end_x   = radius + (radius*ratio - lenght) * sin(radians(angle));
    float end_y   = radius - (radius*ratio - lenght) * cos(radians(angle));
    tft.drawLine(start_x, start_y, end_x, end_y, TFT_GREEN);
}

/**
 * 画出圆上任意的一个点，根据中心点的坐标, 半径，角度
*/
static void drawCirclePixel(uint16_t centen_x, uint16_t center_y, int radius, int angle){

  int suffix = 4;
  float pos_x_prve = centen_x + radius*sin(radians(angle-suffix));
  float pos_y_prve = center_y - radius*cos(radians(angle-suffix));

  float pos_x = centen_x + radius*sin(radians(angle));
  float pos_y = center_y - radius*cos(radians(angle));

  float pos_x_next = centen_x + radius*sin(radians(angle+suffix));
  float pos_y_next = center_y - radius*cos(radians(angle+suffix));

  tft.drawWideLine(pos_x_prve, pos_y_prve, pos_x_next, pos_y_next, 3, TFT_GREEN);
}

//外圈刻度上的点数 count
static void drawDot(int count){
  //每个点数之间的角度
  int angle = 360 / count;
  for(int i = angle; i<360;) {
    drawDotLine(radius, i, 10, 1);
    i = i + angle;
  }
}

//这里画横竖，2个斜线，一共4条线，按照顺时针分成8条线
static void drawCrossLine(){
  for (int i=0; i<= 360; i = i + 45){
    tft.drawWideLine(width/2, height/2, calc_x_pos(radius, i), calc_y_pos(radius, i), cross_line_width,TFT_GREEN, TFT_GREEN);
  }
}

static void drawCircleA(uint16_t radius){
  //0.66, 0.33 分别是半径的 2/3, 1/3
  for(int i=0;i<360;){
    drawCirclePixel(width/2, height/2, radius, i);
    i=i+2;
  }
}

void radarInit(){

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  tft.drawString("N", width/2-8, 0, 4);
  tft.drawString("E", width-15, height/2-10, 4);
  tft.drawString("S", width/2-8, height-20, 4);
  tft.drawString("W", 0, height/2-10, 4);

  tft.drawSmoothArc(width/2, height/2, radius-edge, radius-edge-1, 0, 360, TFT_GREEN, TFT_GREEN);
  tft.drawSmoothArc(width/2, height/2, radius*0.66, radius*0.66-1, 0, 360, TFT_GREEN, TFT_GREEN);
  tft.drawSmoothArc(width/2, height/2, radius*0.33, radius*0.33-1, 0, 360, TFT_GREEN, TFT_GREEN);
  drawDot(24); 
  drawCrossLine();

}

void drawBlenderPointer(int angle){
  int d = 2, p = 0;
  for(int i=1;i<=20;i++){
    
    float x_end = radius + (radius-5)*sin(radians(angle+i));
    float y_end = radius - (radius-5)*cos(radians(angle+i));

    p = p + d;
    d = d + 2;
    if(p>255){
      p = 255;
    }
    tft.drawLine(width/2, height/2, x_end, y_end, tft.alphaBlend(p,  TFT_GREEN, TFT_BLACK));
  }
}

int angle = 1;

void scanCircle(){
    //画雷达的动起来的线条
    if(angle>360){
      angle =1;
    }
    // delay(5);

    draw_angle(String(angle), width-25, height-15, 2);

    // float x_end = radius + (radius-edge-5)*sin(radians(angle));
    // float y_end = radius - (radius-edge-5)*cos(radians(angle));
    // tft.drawLine(width/2, height/2, x_end, y_end, TFT_GREEN);
    drawBlenderPointer(angle);
    
    // int erase_angle = angle-show_angle;
    int erase_angle = angle;

    //把刚刚画过的线清除掉, RADIUS-3 是为了不把最边缘的那个圆也给清除掉
    float erase_x_end = radius + (radius-edge-5)*sin(radians(erase_angle));
    float erase_y_end = radius - (radius-edge-5)*cos(radians(erase_angle));
    tft.drawLine(width/2, height/2, erase_x_end, erase_y_end, TFT_BLACK);

    //补上东西南北4个字母
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    if(erase_angle >5 && erase_angle <10){
      tft.drawString("N", width/2-8, 0, 4);
    }
    if(erase_angle >95 && erase_angle <100){
      tft.drawString("E", width-15, height/2-10, 4);
    }
    if(erase_angle >185 && erase_angle <190){
      tft.drawString("S", width/2-8, height-20, 4);
    }
    if(erase_angle >275 && erase_angle <280){
      tft.drawString("W", 0, height/2-10, 4);
    }

    //补线，补横竖还有2条斜线，这里每次只会到45度角的倍数的时候才补上圆中心点到45度角倍数的线
    if ((erase_angle-5) % 45 ==0){
      tft.drawWideLine(width/2, height/2, calc_x_pos(radius, erase_angle-5), calc_y_pos(radius, erase_angle-5),cross_line_width,TFT_GREEN, TFT_GREEN);
    }
    
    //补圆形上的刻度
    // drawDotLine(radius, (erase_angle/15)*15, 10, 1);

    //补圆形上得点
    uint32_t start = static_cast<uint32_t>(erase_angle-2);
    uint32_t end = static_cast<uint32_t>(erase_angle+2);
    //这里每个角度加上180是因为弧形函数的起点是跟屏幕方向的起点是相反的
    tft.drawSmoothArc(width/2,height/2, radius*0.66, radius*0.66-1, start+180, end+180, TFT_GREEN, TFT_GREEN);
    tft.drawSmoothArc(width/2,height/2, radius*0.33, radius*0.33-1, start+180, end+180, TFT_GREEN, TFT_GREEN);

    angle = angle +5;
}