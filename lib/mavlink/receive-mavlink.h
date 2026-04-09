#include <Arduino.h>
#include "MAVLink.h"
#include "../gesture/gesdata.h"

// #define UART_0
#define UART_1
// #define UART_2

ges_data_t mavlink_receive_parse();

void mavlink_send_request();

// ges_data_t getMavlinkData();