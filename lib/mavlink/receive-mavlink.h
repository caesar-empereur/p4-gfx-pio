#include <Arduino.h>
#include "MAVLink.h"
#include "../gesture/gesdata.h"


ges_data_t mavlink_receive_parse();

void mavlink_send_request();

// ges_data_t getMavlinkData();