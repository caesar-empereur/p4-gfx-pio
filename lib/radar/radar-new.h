// #include <TFT_eSPI.h>
#include <Arduino_GFX_Library.h>
#include "../display/rgb-init.h"
#include "radar-ui-config.h"

#ifndef RADAR_H
#define RADAR_H

void radarInitNew();

void rotatePointer(uint16_t angle);

#endif