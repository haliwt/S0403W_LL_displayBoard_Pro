#ifndef __BSP_USART_H
#define __BSP_USART_H
#include "main.h"




typedef enum ack_sig{

  ack_null,
  ack_power_on=1 ,
  ack_power_off=2,
  ack_wifi_on=3,
  ack_ptc_on=4,
  ack_ptc_off=5,
  ack_plasma_on=6,
  ack_plasma_off=7,
  ack_ultra_on=8,
  ack_ultra_off=9,
  ack_ai_mode=0x0A,
  ack_not_ai_mode=0X0B,
  ack_buzzer_sound=0X0D
  

}ack_sit_e;


uint8_t bcc_check(const unsigned char *data, int len) ;


void usart1_isr_callback_handler(uint8_t data);




//void send_cmd_ack_hanlder(void);

void clear_rx_buff(void);

bool extract_frame(void) ;

void parse_handler(void);


#endif 

