#include "bsp.h"


/*******************************************************************************
	*
	*Function Name:void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
	*Function : timing 10ms interrupt call back function 
	*
	*
*******************************************************************************/
void tim16_int_callback_handler(void)
{
  static uint16_t tm0;
  static uint8_t tm1,tm2;
  tm0++;

	tm1++;

 
    lcd_t.gTimer_wifi_500ms++;
	lcd_t.gTimer_fan_10ms++;
	
	 lcd_t.gTimer_leaf_fast_counter++;
     lcd_t.gTimer_leaf_slow_counter++;
     
	
    if(tm1>9){ //100ms 
       tm1=0;
    
       lcd_t.gTimer_leaf_counter++;
	}

  if(tm0>99){ //1000 *1ms = 1000ms = 1s
		tm0=0;
		
		
         gpro_t.gTimer_set_temp_times++;
         gpro_t.gTimer_temp_compare_value++ ;
         gpro_t.gTimer_fan_to_ptc_warning++ ;
        
		 gpro_t.gTimer_disp_temp_humi_value++;
      
		
		 gpro_t.gTimer_counter_one_minute++;

	     gpro_t.gTimer_counter ++ ;

		 gpro_t.gTimer_send_data_counter++;
     
         

        run_t.gTimer_wifi_connect_counter++;
		run_t.gTimer_disp_time_seconds++;
		run_t.gTimer_key_timing++;

		
        run_t.gTimer_seconds_counter++;
	   
        run_t.gTimer_again_switch_works ++ ;
		
		
		
		 lcd_t.gTimer_colon_counter++;
		 gpro_t.gTimer_two_hours_second_counter++;
		 if(gpro_t.gTimer_two_hours_second_counter > 59){//one mintues .WT.EDIT 2025.11.1
		 	gpro_t.gTimer_two_hours_second_counter =0;
		    gpro_t.gTimer_two_hours_conter ++ ;

		 }
       
		 
	}

   
 }


