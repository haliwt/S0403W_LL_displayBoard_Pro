#include "bsp.h"

uint16_t k;
uint8_t keyvalue;
uint8_t decade_hour;
uint8_t unit_hour;
uint8_t decade_temp;
uint8_t decade_minute;
uint8_t unit_minute;
uint8_t decade_second;
uint8_t unit_second;
uint8_t unit_temp ;

uint8_t keyvalue;

uint8_t rx_data_from_main_flag ;



void (*single_ai_fun)(uint8_t cmd);
void (*single_add_fun)(void);
void (*single_buzzer_fun)(void);
void (*sendAi_usart_fun)(uint8_t senddat);
void (*beijing_time_fun)(void);




//static void Beijing_Time_Display(void);




/************************************************************************
	*
	*Function Name: void Process_Key_Handler(uint8_t keylabel)
	*Function : key by pressed which is key numbers process 
	*Input Ref: key be pressed value 
	*Return Ref:No
	*
************************************************************************/
void Beijing_Time_Init(void)
{

	//Beijing_Time_Display_Handler(Beijing_Time_Display);

}
/**************************************************************************
 * 
 *Function Name:void mode_key_short_fun(void)
 *Function:
 *Input Ref:
 *Return Ref:
 * 
*****************************************************************************/
void mode_key_short_fun(void)
{
   
   if(run_t.ptc_warning ==0 && run_t.fan_warning ==0){
		if(run_t.display_set_timer_or_works_time_mode == works_time){
	
			//timer time + don't has ai item
			run_t.display_set_timer_or_works_time_mode = timer_time;
			run_t.gModel=0; //don't display this AI icon.
            run_t.gTimer_again_switch_works = 0;
			display_time_hours_minutes_fun();
			#if DEBUG_FLAG
			printf("key_shrot_mode-0 !!!\r\n");

			#endif 
			#if 0
			 if(run_t.wifi_link_net_success == 1){
				SendData_Set_Command(0x07,0x02); //AI command has buzzer sound .
				vTaskDelay(100);
			 }
			#endif 

		}
		else if(run_t.display_set_timer_or_works_time_mode == timer_time){
			//beijing time + ai item
			run_t.display_set_timer_or_works_time_mode = works_time;
			run_t.gTimer_again_switch_works = 0;
			run_t.gModel=1; //AI MODE
			display_time_hours_minutes_fun();
			#if DEBUG_FLAG
			printf("key_shrot_mode-1 !!!\r\n");
			#endif 
			#if 0
			 if(run_t.wifi_link_net_success ==1){
    				SendData_Set_Command(0x07,0x01); //MODE_AI,BUR NO_BUZZER);
					vTaskDelay(100);
    		 		
    		 }
            #endif 
		
		}
		
			
	}		
}



/**************************************************************************
 * 
 *Function Name:
 *Function:
 *Input Ref:
 *Return Ref:
 * 
*****************************************************************************/
void add_key_fun(void)
{
     static uint8_t power_on_fisrt_flag ;
    
	switch(run_t.display_set_timer_or_works_time_mode){

    case works_time: //set temperature value add number
    
    case timer_time: //set  

		
		run_t.wifi_set_temperature= run_t.wifi_set_temperature+1;

		if(run_t.wifi_set_temperature > 40)run_t.wifi_set_temperature= 40;

		
	   // decade_temp =  run_t.wifi_set_temperature / 10 ;
		//unit_temp =  run_t.wifi_set_temperature % 10; //
        
		//lcd_t.number1_low=decade_temp;
		//lcd_t.number1_high =decade_temp;

		///lcd_t.number2_low = unit_temp;
		//lcd_t.number2_high = unit_temp;

        gpro_t.temp_key_set_value = 1;
        gpro_t.gTimer_set_temp_times = 0;
 
		run_t.ptc_on_off_flag = 0; //WT.EDIT 2025.10.31
		gpro_t.set_up_temp_value_done = 1;

		gpro_t.first_set_ptc_on=0;
	
	   break;

	   case setup_timer:
		  
			run_t.gTimer_key_timing =0;
            run_t.timer_time_minutes =0;
			run_t.timer_time_hours++ ;//run_t.dispTime_minutes = run_t.dispTime_minutes + 60;
		    if(run_t.timer_time_hours > 24){ //if(run_t.dispTime_minutes > 59){

                 run_t.timer_time_hours=0;//run_t.dispTime_hours =0;
                

			}
			gpro_t.add_dec_key_be_pressed =1;
		    gpro_t.key_set_timer_flag=1;
      
		break;
		}	
	
}

/**************************************************************************
 * 
 *Function Name:void dec_key_fun(void)
 *Function: 1. direct key is adjust temperature value  
 *          2 .combination is set timer value
 *Input Ref:
 *Return Ref:
 * 
*****************************************************************************/
void dec_key_fun(void)
{
    static uint8_t power_on_fisrt_flag;
  
    if(run_t.power_on==1){
	   	if(run_t.ptc_warning ==0){
	
	     switch(run_t.display_set_timer_or_works_time_mode){//switch(run_t.setup_timer_timing_item){


          case works_time:

		  case timer_time: 
	
	   
			run_t.wifi_set_temperature--;
			if(run_t.wifi_set_temperature<20) run_t.wifi_set_temperature=20;
	      


	       // decade_temp =  run_t.wifi_set_temperature / 10;
			//unit_temp =  run_t.wifi_set_temperature % 10; //
       
			//lcd_t.number1_low=decade_temp;
			///lcd_t.number1_high =decade_temp;

			///lcd_t.number2_low = unit_temp;
			//lcd_t.number2_high = unit_temp;
			
             gpro_t.temp_key_set_value = 1;
             gpro_t.gTimer_set_temp_times = 0;
 
			 run_t.ptc_on_off_flag = 0; //WT.EDIT 2025.10.31
		     gpro_t.set_up_temp_value_done = 1;
			 gpro_t.first_set_ptc_on=0;
	
		    break;

			case setup_timer:
	    
			 
				run_t.gTimer_key_timing =0;
				
                run_t.timer_time_minutes =0;
				run_t.timer_time_hours -- ;//run_t.dispTime_minutes = run_t.dispTime_minutes - 1;
				if(run_t.timer_time_hours < 0){//if(run_t.dispTime_minutes < 0){

				    run_t.timer_time_hours =24;//run_t.dispTime_hours --;
					
					
				}
				gpro_t.add_dec_key_be_pressed =1;
                gpro_t.key_set_timer_flag=1;
        
             break;

	    	}
	   	  }

    }
}

/*********************************************************************************
 * 
 * Function Name:static void Beijing_Time_Dispaly(void)
 * Function :
 * Input Ref:
 * Return Ref:
 * 
**********************************************************************************/
void Beijing_Time_Display_Handler(void(*beijing_time_handler)(void))
{
	beijing_time_fun = beijing_time_handler;

}

/****************************************************************
 * 
 * Function Name:
 * Function :function of pointer 
 * 
 * 
****************************************************************/
void Single_Add_RunCmd(void(*addHandler)(void))
{
    single_add_fun = addHandler;   

}

void Single_SendBuzzer_RunCmd(void(*buzzerHandler)(void))
{
	single_buzzer_fun = buzzerHandler;

}
void Single_SendAi_Usart_RunCmd(void(*sendaiHandler)(uint8_t seddat))
{
    sendAi_usart_fun = sendaiHandler;

}










