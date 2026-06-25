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
#include "bsp_poweron.h"
#include "bsp_fan.h"
#include "bsp_threadx.h"

#include "bsp_wifi.h"
#include "bsp_lcd_app.h"
#include "bsp_smg_blink.h"

#include "bsp_tm1723.h"
#include "bsp_usart.h"
#include "bsp_dmatx_usart.h"
#include "interrupt_manager.h"
#include "bsp_fan_leaf.h"


#include "gpio.h"
#include "usart.h"

#include "app_threadx.h"



#define  USE_FreeRTOS      0
	
#define Enable_EventRecorder    0

#define DEBUG_FLAG    Enable_EventRecorder

#if USE_FreeRTOS == 1
	//#include "FreeRTOS.h"
   // #include "task.h"
   // #include "cmsis_os.h"
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
   
   uint8_t power_on_step;
   volatile uint8_t decoder_flag;
   uint8_t last_read_index ;
   uint8_t key_set_temperature_f;
  
  
   uint8_t first_set_ptc_on;
   uint8_t first_rcoder_ptc_on_flag;

   uint8_t fan_interval_stop_f ;

	 
   uint8_t two_hours_interval_f;  



  
   //send data need copy this is cmd
  


 

  
   uint8_t two_hours_cp_flag ;
   uint8_t receive_copy_buff[20];
   /********end*****/
   uint8_t disp_time_colon_flag;
   volatile uint8_t usart1_dma_sending_flag;
   

   
   uint8_t disp_fan_switch_flag ;

   uint8_t fan_run_one_minute;
   
   uint8_t key_set_temperature ;
   uint8_t set_temp_value_success;
   uint8_t humidity_real_value;
   uint8_t  temp_real_value;
   uint8_t  smart_phone_app_timer_power_on_flag;

   uint8_t stopTwoHours_flag;
   uint8_t add_dec_key_be_pressed;
   
   uint8_t first_ptc_on;
   uint8_t set_timer_f; 
   uint8_t wifi_flag ;
   uint8_t buzzer_sound_f;
 
  



 //  uint8_t gTimer_mode_key_long;
volatile   uint8_t gTimer_set_temp_times;
volatile   uint8_t gTimer_temp_compare_value;
 volatile  uint8_t gTimer_fan_to_ptc_warning;
  
  volatile uint8_t gTimer_disp_temp_humi_value;

 volatile  uint8_t gTimer_two_hours_second_counter;
 volatile  uint8_t gTimer_two_hours_conter;
 volatile  uint8_t gTimer_counter;


 volatile  uint8_t gTimer_counter_one_minute;
  
volatile   uint8_t gTimer_send_data_counter;
 
   


}process_state;


extern process_state gpro_t;


void bsp_init(void);

void mode_key_long_fun(void);

void set_temperature_compare_value_fun(void);

void display_timer_and_beijing_time_handler(void);


void direct_comparison_temp(void);


void display_not_ai_timer_mode(void);


#endif

