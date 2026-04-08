#include <Arduino_GFX_Library.h>
#include "../sprite/Rgb_Sprite.h"

#define RGB565_BROWN RGB565(150, 75, 0)
#define BROWN RGB565_BROWN

// uint32_t SKY_COLOR = tft.alphaBlend(125, TFT_DARKCYAN, TFT_CYAN);
#define GROUND_COLOR BROWN
#define SKY_COLOR DARKCYAN

// #ifdef __cplusplus
// extern "C" {
// #endif

void drawSkyGroudAngleByCircle(Rgb_Sprite *bgSprP, int pitch, int roll, 
                                int radius, float center_x, float center_y);


// #ifdef __cplusplus
// } /*extern "C"*/
// #endif