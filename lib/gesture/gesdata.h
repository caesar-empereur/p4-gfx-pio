#include "Arduino.h"

#ifndef GESDATA_H
#define GESDATA_H

typedef struct {
    /* data */
    int16_t roll;
    int16_t pitch;
    int16_t yaw;
    int16_t air_speed;
    int16_t ground_speed;
    int16_t altitude;
} ges_data_t;

#endif