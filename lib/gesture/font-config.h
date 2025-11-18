
#ifndef FONT_CONFIG_H
#define FONT_CONFIG_H

#define PROTOCOL 2  //RGB
// #define PROTOCOL 1 // SPI

//字体大小的定义
// 1 的字体用 12
#define FONT1_H 10 //字体为2时全部位数占用的高度
#define FONT1_1_DIGIT_W 6 //字体为2时2位数整个字体占用的像素
#define FONT1_2_DIGIT_W 12 //字体为2时2位数整个字体占用的像素
#define FONT1_3_DIGIT_W 18 //字体为负的2位数整个字体占用的像素
#define FONT1_MINUS_1_DIGIT_W 12 //字体为负的2位数整个字体占用的像素
#define FONT1_MINUS_2_DIGIT_W 18 //字体为负的2位数整个字体占用的像素
#define FONT1_MINUS_3_DIGIT_W 24 //字体为负的2位数整个字体占用的像素

// 2 的字体用 18
#define FONT2_H 14 //字体为2时全部位数占用的高度
#define FONT2_1_DIGIT_W 8 //字体为2时2位数整个字体占用的像素
#define FONT2_2_DIGIT_W 16 //字体为2时2位数整个字体占用的像素
#define FONT2_3_DIGIT_W 26 //字体为负的2位数整个字体占用的像素
#define FONT2_MINUS_1_DIGIT_W 16 //字体为负的2位数整个字体占用的像素
#define FONT2_MINUS_2_DIGIT_W 26 //字体为负的2位数整个字体占用的像素
#define FONT2_MINUS_3_DIGIT_W 32 //字体为负的2位数整个字体占用的像素

// 3 的字体用 22 
#define FONT3_H 20 //字体为2时全部位数占用的高度
#define FONT3_1_DIGIT_W 12 //字体为2时2位数整个字体占用的像素
#define FONT3_2_DIGIT_W 24 //字体为2时2位数整个字体占用的像素
#define FONT3_3_DIGIT_W 36 //字体为负的2位数整个字体占用的像素
#define FONT3_MINUS_1_DIGIT_W 24 //字体为负的2位数整个字体占用的像素
#define FONT3_MINUS_2_DIGIT_W 36 //字体为负的2位数整个字体占用的像素
#define FONT3_MINUS_3_DIGIT_W 40 //字体为负的2位数整个字体占用的像素


// 4 的字体用 26 
#define FONT4_H 24 //字体为2时全部位数占用的高度
#define FONT4_1_DIGIT_W 16 //字体为2时2位数整个字体占用的像素
#define FONT4_2_DIGIT_W 32 //字体为2时2位数整个字体占用的像素
#define FONT4_3_DIGIT_W 44 //字体为负的2位数整个字体占用的像素
#define FONT4_MINUS_1_DIGIT_W 32 //字体为负的2位数整个字体占用的像素
#define FONT4_MINUS_2_DIGIT_W 44 //字体为负的2位数整个字体占用的像素
#define FONT4_MINUS_3_DIGIT_W 50 //字体为负的2位数整个字体占用的像素

// 5 的字体用 32 
#define FONT5_H 28 //字体为2时全部位数占用的高度
#define FONT5_1_DIGIT_W 20 //字体为2时2位数整个字体占用的像素
#define FONT5_2_DIGIT_W 40 //字体为2时2位数整个字体占用的像素
#define FONT5_3_DIGIT_W 54 //字体为负的2位数整个字体占用的像素
#define FONT5_MINUS_1_DIGIT_W 40 //字体为负的2位数整个字体占用的像素
#define FONT5_MINUS_2_DIGIT_W 54 //字体为负的2位数整个字体占用的像素
#define FONT5_MINUS_3_DIGIT_W 60 //字体为负的2位数整个字体占用的像素

// 6 的字体用 40 
#define FONT6_H 30 //字体为2时全部位数占用的高度
#define FONT6_1_DIGIT_W 24 //字体为2时2位数整个字体占用的像素
#define FONT6_2_DIGIT_W 40 //字体为2时2位数整个字体占用的像素
#define FONT6_3_DIGIT_W 60 //字体为负的2位数整个字体占用的像素
#define FONT6_MINUS_1_DIGIT_W 48 //字体为负的2位数整个字体占用的像素
#define FONT6_MINUS_2_DIGIT_W 70 //字体为负的2位数整个字体占用的像素
#define FONT6_MINUS_3_DIGIT_W 90 //字体为负的2位数整个字体占用的像素

// 8 的字体用 48 
#define FONT8_H 32 //字体为2时全部位数占用的高度
#define FONT8_1_DIGIT_W 28 //字体为2时2位数整个字体占用的像素
#define FONT8_2_DIGIT_W 46 //字体为2时2位数整个字体占用的像素
#define FONT8_3_DIGIT_W 70 //字体为负的2位数整个字体占用的像素
#define FONT8_MINUS_1_DIGIT_W 46 //字体为负的2位数整个字体占用的像素
#define FONT8_MINUS_2_DIGIT_W 70 //字体为负的2位数整个字体占用的像素
#define FONT8_MINUS_3_DIGIT_W 50 //字体为负的2位数整个字体占用的像素

/*根据分辨率加载不同的字体, 
应该根据宽，高两个参数中小的那个来判断,不同的ui，
分辨率相同，需要的字体大小不相同，需要ui自己判断

*/
template <typename T>
int16_t GET_FONT_SIZE(T w, T h){
    int16_t min_res = min(w, h);
    int16_t font_size;
    if(min_res<240){
        font_size = 1;
    } else if(min_res >=240 && min_res<300){
        font_size = 2;
    } else if(min_res >=300 && min_res<400){
        font_size = 2;
    } else if(min_res >=400 && min_res<500){
        font_size = 2;
    } else if(min_res >500){
        font_size = 3;
    }
    return font_size;
}

template <typename T>
int16_t GET_TEXT_Y_HALF(T font_size){
    int16_t pos_y;
    if(font_size == 1){
        pos_y = FONT1_H/2;
    }
    if(font_size == 2){
        pos_y = FONT2_H/2;
    }
    if(font_size == 3){
        pos_y = FONT3_H/2;
    }
    if(font_size == 4){
        pos_y = FONT4_H/2;
    }
    if(font_size == 6){
        pos_y = FONT6_H/2;
    }
    if(font_size == 8){
        pos_y = FONT8_H/2;
    }
    return pos_y;
}

template <typename T>
int16_t GET_TEXT_Y_DOWN(T font_size){
    int16_t pos_y;
    if(PROTOCOL == 1){
        //SPI
        return 0;
    }
    if(font_size == 1){
        pos_y = FONT1_H;
    }
    if(font_size == 2){
        pos_y = FONT2_H;
    }
    if(font_size == 3){
        pos_y = FONT3_H;
    }
    if(font_size == 4){
        pos_y = FONT4_H;
    }
    if(font_size == 6){
        pos_y = FONT6_H;
    }
    if(font_size == 8){
        pos_y = FONT8_H;
    }
    return pos_y;
}

template <typename T>
int16_t GET_TEXT_L_START(T rect_size, T font_size, T text_value){
    int16_t pos_x = rect_size/2;
    if(font_size == 1){
        if(text_value >=0 && text_value <10){
            pos_x = pos_x - FONT1_1_DIGIT_W/2;
        }
        if(text_value >=10 && text_value <=99){
            pos_x = pos_x - FONT1_2_DIGIT_W/2;
        }
        if(text_value >=100){
            pos_x = pos_x - FONT1_3_DIGIT_W/2;
        }
        if(text_value >= -99 && text_value <=-10){
            pos_x = pos_x - FONT1_MINUS_2_DIGIT_W/2;
        }
        if(text_value <=-99){
            pos_x = pos_x - FONT1_MINUS_3_DIGIT_W/2;
        }
    }
    if(font_size == 2){
        if(text_value >=0 && text_value <10){
            pos_x = pos_x - FONT2_1_DIGIT_W/2;
        }
        if(text_value >=10 && text_value <=99){
            pos_x = pos_x - FONT2_2_DIGIT_W/2;
        }
        if(text_value >=100){
            pos_x = pos_x - FONT2_3_DIGIT_W/2;
        }
        if(text_value >= -99 && text_value <=-10){
            pos_x = pos_x - FONT2_MINUS_2_DIGIT_W/2;
        }
        if(text_value <=-99){
            pos_x = pos_x - FONT2_MINUS_3_DIGIT_W/2;
        }
    }
    if(font_size == 3){
        if(text_value >=0 && text_value <10){
            pos_x = pos_x - FONT3_1_DIGIT_W/2;
        }
        if(text_value >=10 && text_value <=99){
            pos_x = pos_x - FONT3_2_DIGIT_W/2;
        }
        if(text_value >=100){
            pos_x = pos_x - FONT3_3_DIGIT_W/2;
        }
        if(text_value >= -99 && text_value <=-10){
            pos_x = pos_x - FONT3_MINUS_2_DIGIT_W/2;
        }
        if(text_value <=-99){
            pos_x = pos_x - FONT3_MINUS_3_DIGIT_W/2;
        }
    }
    if(font_size == 4){
        if(text_value >=0 && text_value <10){
            pos_x = pos_x - FONT4_1_DIGIT_W/2;
        }
        if(text_value >=10 && text_value <=99){
            pos_x = pos_x - FONT4_2_DIGIT_W/2;
        }
        if(text_value >=100){
            pos_x = pos_x - FONT4_3_DIGIT_W/2;
        }
        if(text_value >= -99 && text_value <=-10){
            pos_x = pos_x - FONT4_MINUS_2_DIGIT_W/2;
        }
        if(text_value <=-99){
            pos_x = pos_x - FONT4_MINUS_3_DIGIT_W/2;
        }
    }
    if(font_size == 6){
        if(text_value >=0 && text_value <10){
            pos_x = pos_x - FONT6_1_DIGIT_W/2;
        }
        if(text_value >=10 && text_value <=99){
            pos_x = pos_x - FONT6_2_DIGIT_W/2;
        }
        if(text_value >=100){
            pos_x = pos_x - FONT6_3_DIGIT_W/2;
        }
    }
    if(font_size == 8){
        if(text_value >=0 && text_value <10){
            pos_x = pos_x - FONT8_1_DIGIT_W/2;
        }
        if(text_value >=10 && text_value <=99){
            pos_x = pos_x - FONT8_2_DIGIT_W/2;
        }
        if(text_value >=100){
            pos_x = pos_x - FONT8_3_DIGIT_W/2;
        }
    }
    return pos_x;
}

//从右边开始倒推的字体左边位置坐标
template <typename T>
int16_t GET_TEXT_FROM_R_START(T rect_size, T font_size, T text_value){
    int16_t pos_x = rect_size;
    if(font_size == 1){
        if(text_value >=0 && text_value <10){
            pos_x = pos_x - FONT1_1_DIGIT_W;
        }
        if(text_value >=10 && text_value <=99){
            pos_x = pos_x - FONT1_2_DIGIT_W;
        }
        if(text_value >=100){
            pos_x = pos_x - FONT1_3_DIGIT_W;
        }
        if(text_value > -10 && text_value <0){
            pos_x = pos_x - FONT1_MINUS_1_DIGIT_W;
        }
        if(text_value >= -99 && text_value <=-10){
            pos_x = pos_x - FONT1_MINUS_2_DIGIT_W;
        }
        if(text_value <=-99){
            pos_x = pos_x - FONT1_MINUS_3_DIGIT_W;
        }
    }
    if(font_size == 2){
        if(text_value >=0 && text_value <10){
            pos_x = pos_x - FONT2_1_DIGIT_W;
        }
        if(text_value >=10 && text_value <=99){
            pos_x = pos_x - FONT2_2_DIGIT_W;
        }
        if(text_value >=100){
            pos_x = pos_x - FONT2_3_DIGIT_W;
        }
        if(text_value > -10 && text_value <0){
            pos_x = pos_x - FONT2_MINUS_1_DIGIT_W;
        }
        if(text_value >= -99 && text_value <=-10){
            pos_x = pos_x - FONT2_MINUS_2_DIGIT_W;
        }
        if(text_value <=-99){
            pos_x = pos_x - FONT2_MINUS_3_DIGIT_W;
        }
    }
    if(font_size == 3){
        if(text_value >=0 && text_value <10){
            pos_x = pos_x - FONT3_1_DIGIT_W;
        }
        if(text_value >=10 && text_value <=99){
            pos_x = pos_x - FONT3_2_DIGIT_W;
        }
        if(text_value >=100){
            pos_x = pos_x - FONT3_3_DIGIT_W;
        }
        if(text_value > -10 && text_value <0){
            pos_x = pos_x - FONT3_MINUS_1_DIGIT_W;
        }
        if(text_value >= -99 && text_value <=-10){
            pos_x = pos_x - FONT3_MINUS_2_DIGIT_W;
        }
        if(text_value <=-99){
            pos_x = pos_x - FONT3_MINUS_3_DIGIT_W;
        }
    }
    if(font_size == 4){
        if(text_value >=0 && text_value <10){
            pos_x = pos_x - FONT4_1_DIGIT_W;
        }
        if(text_value >=10 && text_value <=99){
            pos_x = pos_x - FONT4_2_DIGIT_W;
        }
        if(text_value >=100){
            pos_x = pos_x - FONT4_3_DIGIT_W;
        }
        if(text_value > -10 && text_value <0){
            pos_x = pos_x - FONT4_MINUS_1_DIGIT_W;
        }
        if(text_value >= -99 && text_value <=-10){
            pos_x = pos_x - FONT4_MINUS_2_DIGIT_W;
        }
        if(text_value <=-99){
            pos_x = pos_x - FONT4_MINUS_3_DIGIT_W;
        }
    }
    return pos_x;
}

#endif