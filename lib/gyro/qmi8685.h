
#include <Arduino.h>
#include <QMI8658.h>
#include "../gesture/gesdata.h"

#ifndef QMI8685_H
#define QMI8685_H

//是否要对 pitch 修正 90度，针对那些 qmi 传感器焊接到主板上，主板连接屏幕的，这样屏幕竖放的时候传感器就已经是俯仰90度了
#define CALI_PITCH_90 1

void qmi8685_init();

ges_data_t receive_parse_qmi8685();

#endif