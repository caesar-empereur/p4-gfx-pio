#include <Arduino.h>
#include "wit_c_sdk.h"
#include "REG.h"
#include "../gesture/gesdata.h"

#define UART_NO 2

void mpu_init();

ges_data_t receive_parse_mpu();