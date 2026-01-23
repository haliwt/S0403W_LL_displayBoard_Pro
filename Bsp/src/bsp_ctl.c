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

     if(run_t.timer_timing_define_flag == timing_success){
      if(run_t.gTimer_timing > 59){ //
        
        run_t.gTimer_timing =0;
		#if TEST_UNIT
		  run_t.timer_time_minutes = run_t.timer_time_minutes -30;
        #else 
          run_t.timer_time_minutes --;
        

        #endif 
	    if(run_t.timer_time_minutes < 0){
		     run_t.timer_time_hours -- ;
			 run_t.timer_time_minutes =59;
           
			if(run_t.timer_time_hours < 0 ){

	           if(run_t.timer_timing_define_flag == timing_success){
			    run_t.timer_time_hours=0;
				run_t.timer_time_minutes=0;
                
                 run_t.power_on= power_off;
                 SendData_PowerOnOff(0); //send power off cmd to mainboard.WT.EDIT 2024.11.17
                 vTaskDelay(pdMS_TO_TICKS(100)); //WT.EDIT 2026.01.04
                
		
			

		   
				 Power_Off_Fun();
				 gpro_t.again_confirm_power_off_flag = 1;
				 SendData_Set_Command(0x30,0); //mainboard.WT.EDIT 2026.01.04
                 vTaskDelay(pdMS_TO_TICKS(100)); //WT.EDIT 2026.01.04
				 
			
				}
                else{
     
                     run_t.timer_time_hours =0;
                     run_t.timer_time_minutes =0;
				     run_t.display_set_timer_or_works_time_mode=works_time;
                     run_t.gModel=1;
                    
                 }
                            
                
                }
              }
           sendCmdNote_to_threeData(0x6B,run_t.timer_time_hours,run_t.timer_time_minutes,run_t.gTimer_timing) ;
		   vTaskDelay(100);
     }
     }
     else if(run_t.timer_timing_define_flag == timing_not_definition){ 

          if(run_t.gTimer_again_switch_works > 3){
             run_t.timer_time_hours =0;
             run_t.timer_time_minutes =0;
		     run_t.display_set_timer_or_works_time_mode=works_time;
             run_t.gModel=1;

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
void Setup_Timer_Times_Donot_Display(void)
{

   if(run_t.timer_timing_define_flag == timing_success){
   if(run_t.gTimer_timing > 59){ //
        
        run_t.gTimer_timing =0;
        #if TEST_UNIT
		  run_t.timer_time_minutes = run_t.timer_time_minutes -30;
        #else 
          run_t.timer_time_minutes --;

        #endif 
	    if(run_t.timer_time_minutes < 0){
		     run_t.timer_time_hours -- ;
			 run_t.timer_time_minutes =59;
           
			if(run_t.timer_time_hours < 0 ){

	           if(run_t.timer_timing_define_flag == timing_success){
			    run_t.timer_time_hours=0;
				run_t.timer_time_minutes=0;

             
          
			
	

				SendData_PowerOnOff(0); //send power off cmd to mainboard.WT.EDIT 2024.11.17
				osDelay(5);
				
           
				Power_Off_Fun();

			
				run_t.power_on=0 ;
			
				
	           
	          
                
                }
                 else{
     
                     run_t.timer_time_hours =0;
                     run_t.timer_time_minutes =0;
                 
                 }
                            
                
                }
              }
            
		     }
        sendCmdNote_to_threeData(0x6B,run_t.timer_time_hours,run_t.timer_time_minutes,run_t.gTimer_timing) ;
		   vTaskDelay(100);
    }


}

/***************************************************************
 * 
 * Function Name:
 * 
 *
 * 
 **************************************************************/




