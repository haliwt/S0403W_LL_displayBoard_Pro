#ifndef __BSP_SENDACK_H
#define __BSP_SENDACK_H
#include "main.h"


typedef enum ack_sig{

  ack_null,
  ack_power_on ,
  ack_power_off,
  ack_wifi_on,
  ack_ptc_on,
  ack_ptc_off,
  ack_plasma_on,
  ack_plasma_off,
  ack_ultra_on,
  ack_ultra_off,
  ack_ai_mode,
  ack_not_ai_mode,
  ack_buzzer_sound
  

}ack_sit_e;


void receive_data_fromm_mainboard(uint8_t *pdata);

uint8_t bcc_check(const unsigned char *data, int len) ;


void send_cmd_ack_hanlder(void);







#endif 

