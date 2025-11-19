#include "bsp.h"


/*******************************************************************************
	*
	*Function Name:void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
	*Function : timing 1ms interrupt call back function 
	*
	*
*******************************************************************************/
void tim16_int_callback_handler(void)
{
  static uint16_t tm0;
  static uint8_t tm1,tm2;
  tm0++;

	tm1++;
    tm2++;

    if(tm2> 9){//10ms * x
        tm2=0;
    lcd_t.gTimer_wifi_500ms++;
	lcd_t.gTimer_fan_10ms++;
	

     }
	
    if(tm1>99){ //100ms 
       tm1=0;
       lcd_t.gTimer_colon_ms++ ;

	   run_t.gTimer_digital5678_ms++;
       run_t.gTimer_ptc_fan_blink_warning++ ;
	    lcd_t.gTimer_leaf_counter++;
	   gpro_t.gTimer_copy_cmd_counter++;
    }

  if(tm0>999){ //1000 *1ms = 1000ms = 1s
		tm0=0;
		
		
		 gpro_t.gTimer_mode_key_long++;
         gpro_t.gTimer_set_temp_times++;
         gpro_t.gTimer_temp_compare_value++ ;
         gpro_t.gTimer_fan_to_ptc_warning++ ;
         gpro_t.gTimer_cp_timer_counter ++;
		 gpro_t.gTimer_disp_temp_humi_value++;
         gpro_t.gTimer_soft_version_counter++;
     
         //

        run_t.gTimer_wifi_connect_counter++;
		run_t.gTimer_disp_time_seconds++;
		run_t.gTimer_key_timing++;
	//	run_t.gTimer_set_timer_time_seconds ++;
		run_t.gTimer_ptc_fan_warning++;
        run_t.gTimer_timing++;
	    run_t.gTimer_work_break_times++;
        run_t.gTimer_again_switch_works ++ ;
		
		 lcd_t.gTimer_colon_counter++;
		 gpro_t.gTimer_two_hours_second_counter++;
		 if(gpro_t.gTimer_two_hours_second_counter > 59){//one mintues .WT.EDIT 2025.11.1
		 	gpro_t.gTimer_two_hours_second_counter =0;
		    gpro_t.gTimer_two_hours_conter ++ ;

		 }
       
		 
	}

   
 }


