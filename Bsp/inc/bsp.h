#ifndef __BSP_H
#define __BSP_H
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "bsp_cmd_link.h"
#include "bsp_delay.h"
#include "bsp_ctl.h"
#include "bsp_display.h"
#include "bsp_led.h"
#include "bsp_key_app.h"
#include "bsp_key.h"
#include "bsp_lcd.h"
#include "bsp_poweroff.h"
#include "bsp_poweron.h"
#include "bsp_fan.h"
#include "bsp_sendack.h"
#include "bsp_wifi.h"
#include "bsp_lcd_app.h"
#include "bsp_freertos.h"
#include "bsp_tm1723.h"
#include "bsp_usart.h"
#include "bsp_dmatx_usart.h"
#include "interrupt_manager.h"


#include "gpio.h"
#include "usart.h"





#define  USE_FreeRTOS      1
	
#define Enable_EventRecorder 1  

#if USE_FreeRTOS == 1
	#include "FreeRTOS.h"
    #include "task.h"
    #include "cmsis_os.h"
	#include "queue.h"
	#define DISABLE_INT()    taskENTER_CRITICAL()
	#define ENABLE_INT()     taskEXIT_CRITICAL()
#else
	/* interrupt enable or disable */
	#define ENABLE_INT()	__set_PRIMASK(0)	/**/
	#define DISABLE_INT()	__set_PRIMASK(1)	/**/
#endif


	
#if Enable_EventRecorder == 1
	#include "EventRecorder.h"
#endif


typedef struct _process_state{


   uint8_t power_on_every_times;
   uint8_t key_mode_flag;
   uint8_t key_add_flag;
   uint8_t key_dec_flag;
   uint8_t smart_phone_power_on;

  
   //send data need copy this is cmd
   uint8_t send_ack_cmd;
   uint8_t receive_copy_cmd;
   uint8_t disp_time_colon_flag;
   
   uint8_t long_key_power_counter;
   uint8_t key_long_power_flag; 
  


   uint8_t gTimer_mode_key_long;
   uint8_t temp_key_set_value ;
   uint8_t set_temp_value_success;
   uint8_t humidity_real_value;
   uint8_t  temp_real_value;
   uint8_t  smart_phone_app_timer_power_on_flag;
   uint8_t  key_power_flag;
   uint8_t  smart_phone_turn_off_ptc_flag;
  


   //
   uint8_t gTimer_set_temp_times;
   uint8_t gTimer_temp_compare_value;
   uint8_t gTimer_fan_to_ptc_warning;
   uint8_t gTimer_again_send_power_on_off;
   uint8_t gTimer_disp_temp_humi_value;
  
 
   


}process_state;


extern process_state gpro_t;


void bsp_init(void);

void mode_key_long_fun(void);

void set_temperature_compare_value_fun(void);

void display_timer_and_beijing_time_handler(void);





#endif

