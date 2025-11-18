#include <Arduino_GFX_Library.h>
#include "../sprite/Rgb_Sprite.h"

// uint32_t SKY_COLOR = tft.alphaBlend(125, TFT_DARKCYAN, TFT_CYAN);
#define GROUND_COLOR ORANGE
#define SKY_COLOR DARKCYAN

void drawSkyGroudAngleByCircle(Rgb_Sprite *bgSprP, int pitch, int roll, 
                                int radius, float center_x, float center_y);