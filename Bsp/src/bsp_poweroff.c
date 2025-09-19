#include "bsp.h"


uint8_t counter_flag ;



static void power_off_breath_Led(void);




void power_off_handler(void)
{
    
   static uint8_t dc_power_off;

   if(dc_power_off == 0){

      dc_power_off++;

   //   freertos_start_timer1_handler();
      

   }


    if(run_t.power_off_id_flag == 1){   
        run_t.power_off_id_flag++;  
        lcd_donot_disp_screen();
        Power_Off_Fun();
    

        LED_MODEL_OFF();
        POWER_ON_LED();

        run_t.wifi_led_fast_blink_flag=0;
        run_t.smart_phone_set_temp_value_flag=0;
        run_t.timer_time_hours =0;
        run_t.timer_time_minutes =0;

        run_t.fan_warning=0;
        run_t.ptc_warning = 0;

        run_t.gModel =0; //WT.EDIT 2022.09.01
        run_t.plasma=0;
        run_t.dry =0;
        run_t.ultrasonic =0;


        run_t.timer_timing_define_flag = timing_not_definition;

        run_t.disp_wind_speed_grade =100;	

        gpro_t.smart_phone_app_timer_power_on_flag =0;
   
      
	}
    
	
   
    power_off_breath_Led();
	if(run_t.gFan_RunContinue == 1){
       
	      LCD_BACK_LIGHT_ON();
	      LCD_Display_Wind_Icon_Handler();
     }
	 else if(run_t.gFan_RunContinue == 2){
         
           run_t.gFan_RunContinue =0;
		   Lcd_PowerOff_Fun();

	}
}



/************************************************************************
	*
	*Function Name: static void Power_Off_Fun(void)
	*
	*
	*
	*
************************************************************************/
 void Power_Off_Fun(void)
{
	
        run_t.gModel =0; //WT.EDIT 2022.09.01
		run_t.plasma=0;
		run_t.dry =0;
		run_t.ultrasonic =0;

		//run_t.gPower_On=power_off;
		
		run_t.wifi_led_fast_blink_flag=0;
		run_t.timer_timing_define_flag = timing_not_definition;
		
		run_t.disp_wind_speed_grade =30;	
		
		
} 

/************************************************************************
	*
	*Function Name: static void power_off_breath_Led(void)
	*
	*
	*
	*
************************************************************************/
static void power_off_breath_Led(void)
 {

      //POWER_LED_TOGGLE() ;
     // static uint8_t counter;
	  if(gpro_t.gTimer_disp_temp_humi_value> 1){
            gpro_t.gTimer_disp_temp_humi_value=0;
//		    counter = counter ^ 0x01;
//		    if(counter ==1)POWER_ON_LED() ;
//            else POWER_OFF_LED() ;
		     POWER_LED_TOGGLE();
           // lcd_power_off_light_off();
      }
      //counter_flag ++;
	  //printf("power_off_led !!!\r\n");

}



