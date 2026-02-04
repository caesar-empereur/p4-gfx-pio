


#include "../display/rgb-init.h"
#include "horizon-draw-rgb.h"
#include "gesdata.h"

#include <list>

#ifndef GESTURE_RGB_H
#define GESTURE_RGB_H

#ifdef __cplusplus
extern "C" {
#endif

void gestureInit(int16_t x, int16_t y);

void drawGestureByData(int bgType, ges_data_t ges_data, int x, int y);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif

