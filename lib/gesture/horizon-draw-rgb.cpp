#include "horizon-draw-rgb.h"

double radians_to_degrees(double radians) {
    return radians * (180.0 / 3.14);
}

/**
 * 根据左右点的坐标, 计算完整的一条横线的颜色
 * y0xr 就是 y0+x(中间向下), y0-x(中间向上), y0+r(底部), y0-r(顶部)
 * max_pos_y 是切线左右点得最大的 y 值
 */
uint32_t calc_total_line_color(int l_angle, int r_angle, int32_t y0xr, float max_pos_y){
    uint32_t color;
    //首先按照统一的思路，只要左角比右角大, 就算这个横线是天空的颜色
    if(l_angle>r_angle){
        color = SKY_COLOR;
    } else {
        color = GROUND_COLOR;
    }
    //这里 y0xr > max_pos_y 有特殊情况是因为 y0xr<max_pos_y出现左角在左圆,右点在右圆的时候，上面这个if的判断也是成立的
    if(y0xr > max_pos_y){//要绘制的横线在切线之下
        //左点在左半圆, 右点在右半圆，只要要绘制的横线在切线之下, 横线都是天空的颜色
        if((l_angle>=0 && l_angle<=180) && (r_angle>=180 && r_angle<=359)){
            color = SKY_COLOR;
        }
        //右点在左半圆, 左点在右半圆, 只要要绘制的横线在切线之下, 横线都是地面的颜色
        if((r_angle>=0 && r_angle<=180) && (l_angle>=180 && l_angle<=359)){
            color = GROUND_COLOR;
        }
    }
    return color;
}

/**
 * 根据图形库的绘制圆形的思路改造的绘制带有地平线的2部分圆
 */
void selfFillCircle(int32_t x0, int32_t y0, int32_t r, int l_angle, int r_angle, Rgb_Sprite *bgSprP){
    // Serial.println("l_angle= " + String(l_angle) + " r_angle= " + String(r_angle));
    if(l_angle>360){
        l_angle = l_angle-360;
    }
    if(l_angle==360){
        l_angle = 1;
    }
    if(r_angle>=360){
        r_angle = r_angle-360;
    }
    if(r_angle<0){
        r_angle = r_angle + 360;
    }

    float l_pos_x = x0 + r * sin(radians(l_angle));
    float l_pos_y = y0 - r * cos(radians(l_angle));
    float r_pos_x = x0 + r * sin(radians(r_angle));
    float r_pos_y = y0 - r * cos(radians(r_angle));

    float min_pos_y = min(l_pos_y, r_pos_y);
    float max_pos_y = max(l_pos_y, r_pos_y);
    float min_pos_x = min(l_pos_x, r_pos_x);
    float max_pos_x = max(l_pos_x, r_pos_x);

    //切线的斜率正负可以判断倾斜的方向，大于 0 是左上角到右下角倾斜，小于 0 是右上角到左下角倾斜，但无法判断左边角, 右边角是否颠倒
    float slope_rate = (r_pos_y - l_pos_y) / (r_pos_x - l_pos_x);

    //确定要减去的原始的x 坐标是取最大还是最小的
    float yuan_x = slope_rate > 0 ? min_pos_x : max_pos_x;

    int32_t  x  = 0;
    int32_t  y  = 0;
    int32_t  dx = 1;
    int32_t  dy = r+r;
    int32_t  p  = -(r>>1);

    if(y0 < min_pos_y || y0 > max_pos_y){ //说明中间开始的横线在 切线的 min(y) 上面，切线没有干扰到中间的画线,中间的画线跟之前的一样画就行
        uint32_t color = calc_total_line_color(l_angle, r_angle, y0, max_pos_y);
        bgSprP->drawFastHLine(x0 - r, y0, dy+1, color);//从中间向下边画
    } else {
        //这一部分在面对半径小于x坐标的时候会出现黑色的画面，也就是绘制不成功，串口也没法打印
        if(slope_rate!=0){
            float s_y = y0;
            float s_x = ((s_y - min_pos_y) / slope_rate) + yuan_x;
            bgSprP->drawFastHLine(x0 - r, y0, s_x-(x0 - r), min_pos_y==l_pos_y?GROUND_COLOR:SKY_COLOR);//从中间向下边画
            bgSprP->drawFastHLine(s_x, y0, dy+1-(s_x-(x0 - r)), min_pos_y==l_pos_y?SKY_COLOR:GROUND_COLOR);//从中间向下边画
        }
        
    }
    //依然是从中间跟上下两边一直画
    while(x<r){
        if(p>=0) {//p>=0, 说明顶部，底部这部分还没画完, 如果这部分刚好被切线切中，就要分割
            // delay(200);
            if((y0 + r)>max_pos_y || (y0 + r)<min_pos_y){// y0+r 是底部下边,超过切线的最低点，就是y最大值，就跟之前的一样画
                uint32_t color = calc_total_line_color(l_angle, r_angle, y0 + r, max_pos_y);
                bgSprP->drawFastHLine(x0 - x, y0 + r, dx, color);
            } else { // y0+r 没超过切线的最低点, 需要分成天跟地2部分来画
                //根据当前y坐标确定x坐标
                float s_y = y0 + r;
                float s_x = ((s_y - min_pos_y) / slope_rate) + yuan_x;
                bgSprP->drawLine(x0 - x, y0 + r, s_x, y0 + r, max_pos_y==r_pos_y?GROUND_COLOR:SKY_COLOR);//切线左边
                bgSprP->drawLine(s_x, s_y, x0 - x + dx, s_y, max_pos_y==r_pos_y?SKY_COLOR:GROUND_COLOR);//切线右边
            }
            if((y0 - r)<min_pos_y || (y0 - r)>max_pos_y){// y0-r 是顶部上边
                uint32_t color = calc_total_line_color(l_angle, r_angle, y0 - r, max_pos_y);
                bgSprP->drawFastHLine(x0 - x, y0 - r, dx, color);
            } else {
                float s_y = y0 - r;
                float s_x = ((s_y - min_pos_y) / slope_rate) + yuan_x;
                bgSprP->drawLine(x0 - x, y0 - r, s_x, y0 - r, min_pos_y==l_pos_y?GROUND_COLOR:SKY_COLOR);//切线左边
                bgSprP->drawLine(s_x, s_y, x0 - x + dx, s_y, min_pos_y==l_pos_y?SKY_COLOR:GROUND_COLOR);//切线右边
            }
            dy = dy - 2;
            p = p - dy;
            r--;
        }
        dx = dx + 2;
        p = p + dx;
        x++;
        // delay(200);
        
        //画的是中间向下的部分
        if((y0 + x) < min_pos_y || (y0 + x) > max_pos_y){ //说明中间开始的横线在 切线的 min(y) 上面，切线没有干扰到中间的画线,中间的画线跟之前的一样画就行
            uint32_t color = calc_total_line_color(l_angle, r_angle, y0 + x, max_pos_y);
            bgSprP->drawFastHLine(x0 - r, y0 + x, dy+1, color);//从中间向下边画
        } else {
            float s_y = y0 + x;
            float s_x = ((s_y - min_pos_y) / slope_rate) + yuan_x;

            bgSprP->drawLine(x0 - r, y0 + x, s_x, y0 + x, min_pos_y==l_pos_y?GROUND_COLOR:SKY_COLOR);//切线左边
            bgSprP->drawLine(s_x, s_y, x0 - r + dy+1, s_y, min_pos_y==l_pos_y?SKY_COLOR:GROUND_COLOR);//切线右边
        }
        //画的是中间向上的部分
        if((y0 - x) > max_pos_y || (y0 - x) < min_pos_y){//横线 y 坐标在切线底部的下面max(y)，或者 y 坐标在切线顶部的上面 min(y)
            uint32_t color = calc_total_line_color(l_angle, r_angle, y0 - x, max_pos_y);
            bgSprP->drawFastHLine(x0 - r, y0 - x, dy+1, color);//从中间向上边画
        } else {
            float s_y = y0 - x;
            float s_x = ((s_y - min_pos_y) / slope_rate) + yuan_x;
            bgSprP->drawLine(x0 - r, y0 - x, s_x, y0 - x, min_pos_y==l_pos_y?GROUND_COLOR:SKY_COLOR);//切线左边
            bgSprP->drawLine(s_x, s_y, x0 - r + dy+1, s_y, min_pos_y==l_pos_y?SKY_COLOR:GROUND_COLOR);//切线右边
        }
    }
}

void drawSkyGroudAngleByCircle(Rgb_Sprite *bgSprP, int pitch, int roll, int radius, float center_x, float center_y){
    if(pitch >= 60 && pitch <= 120){
      //前后向上倾斜在水平线上面的 的 60-120 之间，左右再怎么倾斜都没法看到地面,都是天空
      // tft.fillScreen(SKY_COLOR);
      bgSprP->fillCircle(center_x, center_y, radius, SKY_COLOR);
      return;
    }
    if((pitch >= -120 && pitch <= -60) || (pitch >= 240 && pitch <= 300)){
      //前后向下倾斜在水平线体面的 60-120 之间，左右再怎么倾斜都没法看到天空,都是地面
      // tft.fillScreen(GROUND_COLOR);
      bgSprP->fillCircle(center_x, center_y, radius, GROUND_COLOR);
      return;
    }

    int a_angle = radians_to_degrees(acos(abs(pitch)/(45 * 1.41)));
    // int a_angle = acos(abs(pitch)/(45));
    int l_angle, r_angle;
    if(pitch > 0){
        l_angle = 270 + (roll - (90 - a_angle));
        r_angle = l_angle - 2 * a_angle;
    } else {
        l_angle = 270 + (roll + (90 - a_angle));
        r_angle = l_angle + 2 * a_angle;
    }
    selfFillCircle(center_x, center_y, radius, l_angle, r_angle, bgSprP);
}