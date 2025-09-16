#ifndef __BSP_CTL_H_
#define __BSP_CTL_H_
#include "main.h"

#define  TEST_UNIT    0




typedef enum{

 
   FAN_WARNING= 0x80,
   PTC_WARNING

}WARNING_STATE;

typedef enum{

   close,
   open

}run_stat_t;

typedef enum{

  copy_null,
  copy_ng,
  copy_ok

}copy_cmd_t;





typedef enum WIFI_STATE_T{
   
    WIFI_POWER_ON = 0x80,
    WIFI_POWER_OFF=0X81,
    WIFI_MODE_1=0X08,   //state ->normal works
    WIFI_MODE_2=0X18,   //state->sleeping works
    WIFI_KILL_ON=0x04,  //Anion(plasma)
    WIFI_KILL_OFF=0x14,
    WIFI_PTC_ON = 0x02, 
    WIFI_PTC_OFF = 0x12,
    WIFI_SONIC_ON = 0x01,       //ultrasonic
    WIFI_SONIC_OFF = 0x11,
    WIFI_WIND_SPEED_ITEM = 0x90,
    WIFI_TEMPERATURE = 0xA0,
    PTC_ERROR = 0xE0,
	FAN_ERROR = 0xE1,
	FAN_REMOVE_ERROR= 0xF1

}wifi_state_t;

typedef enum TIMING_T{

   timing_not_definition,
   timing_success 
}timing_t;


typedef struct __RUN{

  
   
   uint8_t gTemperature_timer_flag;
   uint8_t power_on;
   uint8_t gRun_flag;
   uint8_t power_times;
   uint8_t vk36n4d_interrupt_flag;
   uint8_t power_off_id_flag;

   
   uint8_t gTemperature;
 
 //  uint8_t gTimer_set_timer_time_seconds;
   
  uint8_t gTimer_setup_zero;

  uint8_t gFan_RunContinue;

  uint8_t temperature_flag;
   uint8_t power_off_buzzer_flag ;
  
  uint8_t timer_timing_define_flag;

  uint8_t wifi_link_flag;
  uint8_t first_power_on_flag;
  uint8_t step_run_power_on_tag;

  uint8_t power_key_detected;
  uint8_t power_on_disp_smg_number;

 
  
   uint8_t usart_flag;
   uint8_t decodeFlag;
   uint8_t gTemp_check;
   

   uint8_t  plasma;
   uint8_t  dry;
   uint8_t  fan;
   uint8_t  ultrasonic;

   uint8_t  gModel;
   

   uint8_t ptc_warning;
   uint8_t fan_warning;
  
   uint8_t  disp_wind_speed_grade;
   uint8_t  gTimer_fan_counter;
 

   uint8_t single_data;
   

   uint8_t wifi_link_net_success;

   uint8_t wifi_set_temperature;
   uint8_t wifi_set_timing;
   uint8_t wifi_set_wind_speed;
   uint8_t smart_phone_set_temp_value_flag ;
   

   uint8_t wifi_led_fast_blink_flag ;
 

   //wifi
   uint8_t  wifi_set_oneself_temperature;
  
   uint8_t wifi_receive_led_fast_led_flag;
 
   uint8_t wifi_receive_power_off_flag;


  
   //others 
   uint8_t auto_model_shut_off_ptc_flag;

   //iwdg ref
    uint8_t key_read_value;

   uint8_t gPower_repeat_times_flag;
   //timer 
  
  

   //works two houre after stop 10 minute

   uint8_t gTimer_work_break_times;
   //beijing times
   uint8_t gmt_time_hours;
   uint8_t  gmt_time_minutes;
   uint8_t  gmt_time_seconds;
   uint8_t display_set_timer_or_works_time_mode;

   
 

   uint8_t gReal_humtemp[2];
   uint8_t gInputCmd[2];
   uint8_t wifisetTime[1];
   uint8_t wifiCmd[2];

   //gTimer
 
   uint8_t gTimer_wifi_connect_counter;
 

   uint8_t gTimer_digital5678_ms;
   uint8_t gTimer_key_timing;
   uint8_t gTimer_timing;
 


   

    int8_t dispTime_hours;
	int8_t dispTime_minutes;
	int8_t gTimer_disp_time_seconds;

	int8_t  timer_time_hours;
   int8_t 	timer_time_minutes ;



	
    int8_t 	gTime_hours;


     uint8_t gTimer_ptc_fan_warning;
     uint8_t gTimer_again_switch_works ;
	
	 uint16_t gTimer_ptc_fan_blink_warning;
     uint16_t gTimer_numbers_one_two_blink;
     uint32_t wifi_key_counter;

   
 }RUN_T;

extern RUN_T run_t;












void Setup_Timer_Times_Donot_Display(void);

//void display_timer_or_timing_time_fun(uint8_t tim);


void disp_timer_run_times(void);

void Works_Counter_Time(void);





#endif 



