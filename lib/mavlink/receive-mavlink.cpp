#include "receive-mavlink.h"

ges_data_t ges_data_mav;


ges_data_t mavlink_receive_parse() {
  mavlink_message_t msg;    //协议消息的结构体
  mavlink_status_t status;
  while(Serial.available()>0) {
    uint8_t c = Serial.read();
    // Serial1.write(c);
    //根据串口序号, 将数据从缓冲区取出来逐帧拼接成指定的消息结构体格式
    if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
        switch(msg.msgid) {
          case MAVLINK_MSG_ID_HEARTBEAT:  {
              mavlink_heartbeat_t heartbeat;
              mavlink_msg_heartbeat_decode(&msg, &heartbeat);
            }
            break;
          case MAVLINK_MSG_ID_ATTITUDE:  {
              //消息的 id 说明不同类型的数据, gps 遥测telem数据
              mavlink_attitude_t attitude;
              //解析指定的 高度纬度方位数据 
              mavlink_msg_attitude_decode(&msg, &attitude);

              int roll = static_cast<int>(attitude.roll* -60);
              int pitch = static_cast<int>(attitude.pitch* 60);
              int yaw = static_cast<int>(attitude.yaw* 60);

              if(yaw>360){
                  //方位值大于360就重置为0
                  yaw = 0;
              }

              // ges_data_t ges_data_mav;
              ges_data_mav.roll = roll;
              ges_data_mav.pitch = pitch;
              ges_data_mav.yaw = yaw;
              // ges_data_mav.air_speed = abs(roll);
              // ges_data_mav.altitude = abs(pitch);
              
              Serial.println("mavlink data, roll=" + String(roll) + ",pitch= " + String(pitch) + ",yaw= " + String(yaw));
            }
          case MAVLINK_MSG_ID_VFR_HUD: {
              mavlink_vfr_hud_t vfr_hud;
              mavlink_msg_vfr_hud_decode(&msg, &vfr_hud);
              
              int altitude =static_cast<int>( vfr_hud.alt * 0.01);
              int airSpeed = static_cast<int>(vfr_hud.airspeed);
              int groundSpeed = static_cast<int>(vfr_hud.groundspeed);

              ges_data_mav.altitude = altitude;
              ges_data_mav.air_speed = airSpeed;
              ges_data_mav.ground_speed = groundSpeed;
              Serial.println("altitude= " + String(altitude) + " airSpeed= " + String(airSpeed) 
                       + " groundSpeed= "+ String(groundSpeed));
          }
            break;
          default:
            break;
        }
        
    }
  }
  return ges_data_mav;
}

void mavlink_send_request(){
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    mavlink_msg_request_data_stream_pack(255, 190, &msg, 1, 1, 0, 5, 1);
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    Serial1.write(buf, len);
}