#include "bsp.h"

RUN_T run_t;




uint8_t temp;



/*************************************************************************
	*
	*Functin Name:void disp_timer_run_times(void)
	*Function : set up timer timing function
	*
	*
	*
*************************************************************************/
void disp_timer_run_times(void)
{

     if(run_t.timer_set_success_flag == timing_success){
      if(run_t.gTimer_seconds_counter > 59){ //
        
        run_t.gTimer_seconds_counter =0;
		#if  1   //TEST_UNIT
		  run_t.timer_time_minutes = run_t.timer_time_minutes -30;
        #else 
          run_t.timer_time_minutes --;
        

        #endif 
	    if(run_t.timer_time_minutes < 0){
		     run_t.timer_time_hours -- ;
			 run_t.timer_time_minutes =59;
           
			if(run_t.timer_time_hours < 0 ){

	           if(run_t.timer_set_success_flag == timing_success){
			    run_t.timer_time_hours=0;
				run_t.timer_time_minutes=0;
            
	
                 run_t.power_on= power_off;
                 SendData_PowerOnOff(0); //send power off cmd to mainboard.WT.EDIT 2024.11.17
                 tx_thread_sleep(5); //WT.EDIT 2026.01.04
                
		         Power_Off_Fun();
				 
				 SendData_Set_Command(0x10,0); //turn off-> don't buzzer sound :mainboard.WT.EDIT 2026.01.04
                 tx_thread_sleep(5); //WT.EDIT 2026.01.04
				 
			
				}
                else{
     
                     run_t.timer_time_hours =0;
                     run_t.timer_time_minutes =0;
				     run_t.time_setting_mode=works_time;
                     //run_t.gAI=1;
                    
                 }
                            
                
                }
              }
           sendCmdNote_to_threeData(0x6B,run_t.timer_time_hours,run_t.timer_time_minutes,run_t.gTimer_seconds_counter) ;
		   tx_thread_sleep(2);
     }
     }
  
}

/*************************************************************************
	*
	*Functin Name:void Setup_Timer_Times_Donot_Display(void)
	*
	*
	*
	*
*************************************************************************/       



