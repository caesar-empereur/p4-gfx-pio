#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#include "weixue-320-820-init.h"
#include "weixue-320-820-2-init.h"
#include "yu-ying-400-960-init.h"
#include "yu-ying-480-480-init.h"
#include "qin-tang-480-800-init.h"
#include "st7701s-init-340-800.h"
#include "rgb-pin-config.h"
#include "qspi-pin-config.h"
#include "../gesture/gesture-ui-config.h"
#include "../gesture/compass-ui-config.h"
#include "../radar/radar-ui-config.h"
#include "../sprite/Rgb_Sprite.h"
#include "../configui/global_config.h"
#include "U8g2lib.h"
#include "displays_config.h"

#ifdef __cplusplus
extern "C" {
#endif

void initDisplay();

#ifdef __cplusplus
} /*extern "C"*/
#endif