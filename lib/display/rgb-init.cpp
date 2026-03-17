#include "rgb-init.h"
#include "rgb-pin-config.h"

#include "font/u8g2_font_yang.h"
#include "font/u8g2_font_yang_10.h"
#include "font/u8g2_font_yang_11.h"
#include "font/u8g2_font_yang_14.h"
#include "font/u8g2_font_yang_18.h"
#include "font/u8g2_font_yang_22.h"
#include "font/u8g2_font_yang_26.h"
#include "font/u8g2_font_yang_32.h"
#include "font/u8g2_font_yang_40.h"
#include "font/u8g2_font_yang_48.h"

const uint8_t *GFX_FONT = u8g2_font_yang;
const uint8_t *GFX_FONT_10 = u8g2_font_yang_10;
const uint8_t *GFX_FONT_11 = u8g2_font_yang_11;
const uint8_t *GFX_FONT_14 = u8g2_font_yang_14;
const uint8_t *GFX_FONT_18 = u8g2_font_yang_18;
const uint8_t *GFX_FONT_22 = u8g2_font_yang_22;
const uint8_t *GFX_FONT_26 = u8g2_font_yang_26;
const uint8_t *GFX_FONT_32 = u8g2_font_yang_32;
const uint8_t *GFX_FONT_40 = u8g2_font_yang_40;
const uint8_t *GFX_FONT_48 = u8g2_font_yang_48;

// Arduino_ESP32QSPI *qspi = new Arduino_ESP32QSPI(PIN_NUM_LCD_CS, 
//                                                   PIN_NUM_LCD_PCLK,
//                                                  PIN_NUM_LCD_DATA0,
//                                                  PIN_NUM_LCD_DATA1,
//                                                  PIN_NUM_LCD_DATA2,
//                                                  PIN_NUM_LCD_DATA3,
//                                               false);
// Arduino_CO5300 *gfx = new Arduino_CO5300(qspi, PIN_NUM_LCD_RST,3, 460, 460,0,0,0,10);

Arduino_ESP32DSIPanel *dsipanel = new Arduino_ESP32DSIPanel(
                                                            display_cfg.hsync_pulse_width,
                                                            display_cfg.hsync_back_porch,
                                                            display_cfg.hsync_front_porch,
                                                            display_cfg.vsync_pulse_width,
                                                            display_cfg.vsync_back_porch,
                                                            display_cfg.vsync_front_porch,
                                                            display_cfg.prefer_speed,
                                                            display_cfg.lane_bit_rate);

Arduino_DSI_Display *gfx = new Arduino_DSI_Display(
                                                    display_cfg.width,
                                                    display_cfg.height,
                                                    dsipanel,
                                                    2,
                                                    true,
                                                    display_cfg.lcd_rst,
                                                    display_cfg.init_cmds,
                                                    display_cfg.init_cmds_size);



//全屏的 sp
Rgb_Sprite bgSprAllHori = Rgb_Sprite(gfx);

//椭圆的 sp
Rgb_Sprite bgSprEllipse = Rgb_Sprite(gfx);

Rgb_Sprite airSpeedSp = Rgb_Sprite(gfx);
Rgb_Sprite altitudeSp = Rgb_Sprite(gfx);
Rgb_Sprite airSpeedSpBg = Rgb_Sprite(gfx);
Rgb_Sprite altitudeSpBg = Rgb_Sprite(gfx);
Rgb_Sprite airSpeedFloatSp = Rgb_Sprite(gfx);
Rgb_Sprite altitudeFloatSp = Rgb_Sprite(gfx);

Rgb_Sprite midHoriSp = Rgb_Sprite(gfx);
Rgb_Sprite midHoriLineSp = Rgb_Sprite(gfx);//姿态仪中间的标记横线

Rgb_Sprite yawSp = Rgb_Sprite(gfx);
Rgb_Sprite yawSpBlackBg = Rgb_Sprite(gfx);
Rgb_Sprite yawFloatFixSp = Rgb_Sprite(gfx);

Rgb_Sprite yawRotateBgSp = Rgb_Sprite(gfx);
Rgb_Sprite yawRotateSp = Rgb_Sprite(gfx);
Rgb_Sprite yawRotateNoFillSp = Rgb_Sprite(gfx);
Rgb_Sprite yawRotateTextSp = Rgb_Sprite(gfx);
Rgb_Sprite yawRotateVLineSp = Rgb_Sprite(gfx);

Rgb_Sprite compassSp = Rgb_Sprite(gfx);
Rgb_Sprite compassBgSp = Rgb_Sprite(gfx);
Rgb_Sprite compassTextSp = Rgb_Sprite(gfx);

Rgb_Sprite compassMidTextSp = Rgb_Sprite(gfx);
Rgb_Sprite compassPointertSp = Rgb_Sprite(gfx);
Rgb_Sprite compassCoverSp = Rgb_Sprite(gfx);

Rgb_Sprite rotate_re_sp = Rgb_Sprite(gfx);
Rgb_Sprite refresh_sp = Rgb_Sprite(gfx);

//这个背景在初始化的时候，把图形绘制进去，然后把帧复制出来，最后删掉
Rgb_Sprite radarBgSp = Rgb_Sprite(gfx);
Rgb_Sprite radarPointerSp = Rgb_Sprite(gfx);
Rgb_Sprite radarPSmallSp = Rgb_Sprite(gfx);
// Rgb_Sprite radarBgFrameSp = Rgb_Sprite(gfx);//用来存储初始化的背景图形帧

void initDisplay(){
    gfx->begin();
    gfx->fillScreen(BLACK);


    bgSprAllHori.createSprite(GES_SP_W, GES_SP_H);
    bgSprAllHori.setPivot(GES_SP_W/2, GES_SP_H/2);

    bgSprEllipse.createSprite(ELLIPSE_SP_W, ELLIPSE_SP_H(0));
    bgSprEllipse.fillScreen(BLACK);
    bgSprEllipse.setPivot(ELLIPSE_SP_W/2, ELLIPSE_SP_H(0)/2);


    airSpeedSp.createSprite(TWO_SP_WIDTH, TWO_SP_HEIGHT);
    airSpeedSp.fillScreen(TFT_TRANSPARENT);
    //   airSpeedSp.fillScreen(TFT_RED);
    airSpeedSp.setScrollRect(0,0,TWO_SP_WIDTH,TWO_SP_HEIGHT,TFT_TRANSPARENT);
    airSpeedSpBg.createSprite(TWO_SP_WIDTH, TWO_SP_HEIGHT);
    airSpeedSpBg.fillScreen(BLACK);

    airSpeedFloatSp.createSprite(TWO_SP_WIDTH, TWO_SP_HEIGHT);
    airSpeedFloatSp.fillScreen(TFT_TRANSPARENT);

    altitudeSp.createSprite(TWO_SP_WIDTH, TWO_SP_HEIGHT);
    altitudeSp.fillScreen(TFT_TRANSPARENT);
    altitudeSp.setScrollRect(0,0,TWO_SP_WIDTH,TWO_SP_HEIGHT,TFT_TRANSPARENT);
    altitudeSpBg.createSprite(TWO_SP_WIDTH, TWO_SP_HEIGHT);
    altitudeSpBg.fillScreen(BLACK);

    altitudeFloatSp.createSprite(TWO_SP_WIDTH, TWO_SP_HEIGHT);
    altitudeFloatSp.fillScreen(TFT_TRANSPARENT);

    
    midHoriSp.createSprite(HORI_SP_WIDTH, HORI_SP_HEIGHT);
    midHoriSp.fillScreen(TFT_TRANSPARENT);
    //     midHoriSp.fillScreen(TFT_BROWN);
    //     midHoriSp.fillRect(0,0,HORI_SP_WIDTH, HORI_SP_CROWN_H, TFT_PURPLE);
    //     midHoriSp.fillRect(0, HORI_SP_CROWN_H,HORI_SP_WIDTH,HORI_SP_HEIGHT-HORI_SP_CROWN_H, TFT_DARKGREY);
    midHoriSp.setScrollRect(0, HORI_SP_CROWN_H,HORI_SP_WIDTH,HORI_SP_HEIGHT-HORI_SP_CROWN_H,TFT_TRANSPARENT);
    midHoriSp.setPivot(HORI_SP_WIDTH/2, HORI_SP_CROWN_H+HORI_SP_SCROLL_BEGIN_Y+HORI_SP_SCALE_PIX_INTERVAL(0)*2);

    midHoriLineSp.createSprite(HORI_SP_H_LINE_W, 2);
    midHoriLineSp.fillScreen(GREEN);
    midHoriLineSp.setPivot(HORI_SP_H_LINE_W/2, 2);

    // yawSpBlackBg.setColorDepth(8);
    yawSpBlackBg.createSprite(GES_SP_W, YAW_SP_H);
    yawSpBlackBg.fillScreen(BLACK);
    yawSpBlackBg.setPivot(GES_SP_W/2, YAW_SP_H/2);

    yawSp.createSprite(GES_SP_W, YAW_SP_H);
    yawSp.fillScreen(TFT_TRANSPARENT);
    yawSp.setScrollRect(0,0,GES_SP_W,YAW_SP_H,TFT_TRANSPARENT);
    yawSp.setPivot(GES_SP_W/2, YAW_SP_H/2);

    yawFloatFixSp.createSprite(YAW_FLOAT_W, YAW_FLOAT_H);
    yawFloatFixSp.fillScreen(TFT_TRANSPARENT);

    int16_t ges_font_size = GET_FONT_SIZE(GES_SP_W, GES_SP_H);
    

    if(YAW_SP_COMPASS_SHOW ==1){
        yawRotateBgSp.createSprite(YAW_ROTATE_SP_BG_W, YAW_ROTATE_SP_BG_H);
        yawRotateBgSp.setPivot(YAW_ROTATE_SP_BG_W/2, YAW_ROTATE_SP_BG_H + YAW_ROTATE_SP_POVIT_Y);
        yawRotateBgSp.fillScreen(TFT_TRANSPARENT);

        yawRotateSp.createSprite(YAW_ROTATE_SP_RADIUS*2, YAW_ROTATE_SP_RADIUS*2);
        yawRotateSp.setPivot(YAW_ROTATE_SP_RADIUS, YAW_ROTATE_SP_RADIUS);
        yawRotateSp.fillScreen(TFT_TRANSPARENT);
        yawRotateNoFillSp.createSprite(YAW_ROTATE_SP_RADIUS*2, YAW_ROTATE_SP_RADIUS*2);
        yawRotateNoFillSp.setPivot(YAW_ROTATE_SP_RADIUS, YAW_ROTATE_SP_RADIUS);
        yawRotateNoFillSp.fillScreen(TFT_TRANSPARENT);

        yawRotateTextSp.createSprite(YAW_RORATE_TEXT_SP_W, YAW_RORATE_TEXT_SP_H);
        yawRotateTextSp.setPivot(YAW_RORATE_TEXT_SP_W/2, YAW_RORATE_TEXT_SP_H/2);
        yawRotateTextSp.fillScreen(TFT_TRANSPARENT);

        
        yawRotateVLineSp.createSprite(2, YAW_RORATE_SP_V_LINE_LEN);
        yawRotateVLineSp.fillScreen(GREEN);

        
    }

    //单独航向罗盘部分加载的字体
    int16_t comp_font_size = GET_FONT_SIZE(COMPS_SP_W, COMPS_SP_H);
    if(COMPS_SP_SHOW == 1){
        compassSp.createSprite(COMPS_SP_W, COMPS_SP_H);
        compassSp.setPivot(COMPS_SP_W/2, COMPS_SP_H/2);
        compassSp.fillScreen(TFT_TRANSPARENT);

        compassBgSp.createSprite(COMPS_SP_W, COMPS_SP_H);
        compassBgSp.setPivot(COMPS_SP_W/2, COMPS_SP_H/2);
        compassBgSp.fillScreen(BLACK);
        compassTextSp.createSprite(COMPS_TEXT_SP_W, COMPS_TEXT_SP_H);
        compassTextSp.setPivot(COMPS_TEXT_SP_W/2, COMPS_TEXT_SP_H/2);
        compassTextSp.fillScreen(TFT_TRANSPARENT);

        compassMidTextSp.createSprite(COMPS_MID_TEXT_SP_W, COMPS_MID_TEXT_SP_H);
        compassMidTextSp.setPivot(COMPS_MID_TEXT_SP_W/2, COMPS_MID_TEXT_SP_H/2);
        compassMidTextSp.fillScreen(TFT_TRANSPARENT);
        compassPointertSp.createSprite(COMPS_MID_POINTER_SP_W, COMPS_MID_POINTER_SP_H);
        compassPointertSp.setPivot(COMPS_MID_POINTER_SP_W/2, COMPS_MID_POINTER_SP_H/2);
        compassPointertSp.fillScreen(TFT_TRANSPARENT);

        compassCoverSp.createSprite(COMPS_SP_W, COMPS_SP_H);
        compassCoverSp.fillScreen(TFT_TRANSPARENT);
        
        if(comp_font_size == 1){
            compassTextSp.setFont(GFX_FONT_18);
            compassMidTextSp.setFont(GFX_FONT_26);
        } else if(comp_font_size == 2){
            compassTextSp.setFont(GFX_FONT_22);
            compassMidTextSp.setFont(GFX_FONT_32);
        } else if(comp_font_size == 3){
            compassTextSp.setFont(GFX_FONT_26);
            compassMidTextSp.setFont(GFX_FONT_40);
        } else if(comp_font_size == 4){
            compassTextSp.setFont(GFX_FONT_32);
            compassMidTextSp.setFont(GFX_FONT_48);
        }
    }

    radarBgSp.createSprite(RADAR_SP_W, RADAR_SP_H);
    radarBgSp.fillScreen(BLACK);
    radarBgSp.setPivot(RADAR_SP_W/2, RADAR_SP_H/2);

    // radarBgFrameSp.createSprite(RADAR_SP_W, RADAR_SP_H);
    // radarBgFrameSp.fillScreen(BLACK);

    radarPointerSp.createSprite(RADAR_SP_W, RADAR_SP_H);
    radarPointerSp.fillScreen(TFT_TRANSPARENT);
    radarPointerSp.setPivot(RADAR_SP_W/2, RADAR_SP_H/2);


    radarPSmallSp.createSprite(4, RADAR_SP_H/2);
    radarPSmallSp.fillScreen(TFT_TRANSPARENT);
    radarPSmallSp.setPivot(2, RADAR_SP_H/2);

    
    if(ges_font_size == 1){
        airSpeedSp.setFont(GFX_FONT_14);
        altitudeSp.setFont(GFX_FONT_14);
        midHoriSp.setFont(GFX_FONT_14);
        yawSp.setFont(GFX_FONT_14);
        yawRotateSp.setFont(GFX_FONT_14);

        if(YAW_SP_COMPASS_SHOW ==1){
            yawRotateTextSp.setFont(GFX_FONT_14);
        }
        //下面的字体需要大一号
        altitudeFloatSp.setFont(GFX_FONT_18);
        airSpeedFloatSp.setFont(GFX_FONT_18);
        yawFloatFixSp.setFont(GFX_FONT_18);

    }
    if(ges_font_size == 2){
        airSpeedSp.setFont(GFX_FONT_18);
        altitudeSp.setFont(GFX_FONT_18);
        midHoriSp.setFont(GFX_FONT_18);
        yawSp.setFont(GFX_FONT_18);
        yawRotateSp.setFont(GFX_FONT_18);

        if(YAW_SP_COMPASS_SHOW ==1){
            yawRotateTextSp.setFont(GFX_FONT_18);
        }

        //下面的字体需要大一号
        altitudeFloatSp.setFont(GFX_FONT_22);
        airSpeedFloatSp.setFont(GFX_FONT_22);
        yawFloatFixSp.setFont(GFX_FONT_22);

    }
    if(ges_font_size == 3){
        airSpeedSp.setFont(GFX_FONT_22);
        altitudeSp.setFont(GFX_FONT_22);
        midHoriSp.setFont(GFX_FONT_22);
        yawSp.setFont(GFX_FONT_22);
        yawRotateSp.setFont(GFX_FONT_22);

        if(YAW_SP_COMPASS_SHOW ==1){
            yawRotateTextSp.setFont(GFX_FONT_22);
        }

        //下面的字体需要大一号
        altitudeFloatSp.setFont(GFX_FONT_26);
        airSpeedFloatSp.setFont(GFX_FONT_26);
        yawFloatFixSp.setFont(GFX_FONT_26);

    }
    if(ges_font_size == 4){
        airSpeedSp.setFont(GFX_FONT_26);
        altitudeSp.setFont(GFX_FONT_26);
        midHoriSp.setFont(GFX_FONT_26);
        yawSp.setFont(GFX_FONT_26);
        yawRotateSp.setFont(GFX_FONT_26);

        if(YAW_SP_COMPASS_SHOW ==1){
            yawRotateTextSp.setFont(GFX_FONT_26);
        }
        //下面的字体需要大一号
        altitudeFloatSp.setFont(GFX_FONT_32);
        airSpeedFloatSp.setFont(GFX_FONT_32);
        yawFloatFixSp.setFont(GFX_FONT_32);

    }

}

