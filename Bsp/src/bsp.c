#include "bsp.h"

process_state gpro_t;

#define DEFAULT_TEMP    40 

typedef enum{

  PTC_STATE_OFF = 0,
  PTC_STATE_ON  = 1
}PTC_State;

static PTC_State ptc_state = PTC_STATE_OFF ;

static void handle_works_time_mode(void); 
static void handle_setup_timer_mode(void); 
static void handle_timer_time_mode(void); 
static void handle_fan_warning_mode(void); 
static void handle_ptc_warning_mode(void); 




//static void  disp_set_timer_timing_value_fun(void);

static void counter_time_timing_fun(void);

static void fan_default_warning_fun(void);

static void ptc_high_temp_warning_fun(void);

static void power_on_init_disp_time_numbers(void);

static void send_ptc_command(uint8_t on_off);

static void disp_timer_set_numbers(void);

static void disp_set_timer_timing_value_fun(void) ;


void bsp_init(void)
{
  //Beijing_Time_Init();
  bsp_wifi_init();
  
  run_t.disp_wind_speed_grade =100;//WT.EDIT 2025.04.16
  run_t.wifi_set_temperature=40; //WT.EDIT 2025.04.16
  #if Enable_EventRecorder == 1  
	/* 0…60‹10‡80†40†30…4EventRecorder0…50„40†70„90‡40‹0 */
	EventRecorderInitialize(EventRecordAll, 1U);
	EventRecorderStart();
#endif
	

}

uint8_t timer_time_switch_f ;

void mode_key_long_fun(void)
{
   
   if(run_t.ptc_warning ==0){
   	
       run_t.gAI = 0;
       display_ai_icon(run_t.gAI) ;
       gpro_t.gTimer_disp_temp_humi_value=0;
       run_t.time_setting_mode = setup_timer;
        gpro_t.set_timer_f = 1;
       run_t.gTimer_key_timing=0;
       gpro_t.gTimer_disp_temp_humi_value=0;

   
   }

}
/**************************************************************************************
*
*Function Name:void display_timer_and_beijing_time_handler(void)
*Function: all display times with content 
*Input Ref:
*Return Ref:
*
**************************************************************************************/
void display_timer_and_beijing_time_handler(void)
{

	switch(run_t.time_setting_mode) {
		case works_time:		handle_works_time_mode(); break;
		case timer_time:		handle_timer_time_mode(); break;
		case setup_timer:		handle_setup_timer_mode(); break;
		case FAN_WARNING:		handle_fan_warning_mode(); break;
		case PTC_WARNING:		handle_ptc_warning_mode(); break;
	}

}
/******************************************************************************
	*
	*Function Name:static void handle_works_time_mode(void)
	*Function: display of icon , "1" -> ON ,"0"-> OFF
	*Input Ref:NO
	*Return Ref:NO
	*
******************************************************************************/   
static void handle_works_time_mode(void)
{
    static uint8_t disp_f ;
    

     if(lcd_t.display_beijing_time_flag == 0 && gpro_t.power_on_every_times == 1 ){

              gpro_t.power_on_every_times++;
              run_t.gTimer_disp_time_seconds=0; //WT.EDIT 2025.01.08
     }
     else if(lcd_t.display_beijing_time_flag == 1 && gpro_t.power_on_every_times == 1 ){
        gpro_t.power_on_every_times++;
     }
    
     counter_time_timing_fun(); 
  
	 disp_timer_run_times();
//	 if(timer_time_switch_f==2){
//	 	timer_time_switch_f++;
//	  display_time_hours_minutes_fun();
//	 }
//	 timer_time_switch_f  = 1;

}
/**
* @brief 
* @note
* @param
* @return
*/

static void handle_setup_timer_mode(void)
{

   disp_set_timer_timing_value_fun();

}

/**
* @brief 
* @note
* @param
* @return
*/

static void disp_set_timer_timing_value_fun(void) 
{
    static uint8_t tim_bit_1_hours, tim_bit_2_hours,tim_bit_1_minutes,tim_bit_2_minutes;

    uint8_t mask;


    if(run_t.gTimer_key_timing < 4) {
        tim_bit_2_hours = run_t.timer_time_hours / 10;
        tim_bit_1_hours = run_t.timer_time_hours % 10;

		tim_bit_2_minutes = run_t.timer_time_minutes /10;
        tim_bit_1_minutes = run_t.timer_time_minutes % 10;

        lcd_t.number5_low =  tim_bit_2_hours;
		lcd_t.number5_high = tim_bit_2_hours;
        lcd_t.number6_low =  tim_bit_1_hours;
		lcd_t.number6_high = tim_bit_1_hours;
		
        lcd_t.number7_low = tim_bit_2_minutes ;
		lcd_t.number7_high = tim_bit_2_minutes;
        lcd_t.number8_low = tim_bit_1_minutes ;
		lcd_t.number8_high= tim_bit_1_minutes;

        //mask = blink_on ? 0xFF : 0x0F; // 闪烁掩码
        //display_digits(mask, 1);
	

    } 
	else{
        
        run_t.gTimer_seconds_counter = 0;

        if (run_t.timer_time_hours != 0 && gpro_t.add_dec_key_be_pressed == 1){
            run_t.timer_set_success_flag = timing_success;
            run_t.time_setting_mode = timer_time;
            run_t.gAI = 0;
			gpro_t.add_dec_key_be_pressed++;
			sendCmdNote_to_Data(0x2B,0x01);
            tx_thread_sleep(2);
            

        }
		else  if (run_t.timer_time_hours == 0 && gpro_t.add_dec_key_be_pressed == 1){
            run_t.timer_set_success_flag = timing_not_definition;
            run_t.time_setting_mode = works_time;
	        run_t.timer_time_minutes =0;
            run_t.gAI = 1;
		    gpro_t.add_dec_key_be_pressed++;
			sendCmdNote_to_Data(0x2B,0);
            tx_thread_sleep(1);

		}
        else if( run_t.timer_set_success_flag == timing_not_definition){
            run_t.gAI = 1;
            run_t.time_setting_mode = works_time;
		     sendCmdNote_to_Data(0x2B,0);
             tx_thread_sleep(1);

			
        }
		else if( run_t.timer_set_success_flag == timing_success){
			 run_t.gAI = 0;
            run_t.time_setting_mode = timer_time;

		}
    }

    TIM1723_Write_Cmd(LUM_VALUE);
}

/**
* @brief 
* @note
* @param
* @return
*/

static void handle_timer_time_mode(void)
{
        
     if(run_t.gTimer_again_switch_works > 2 && run_t.timer_set_success_flag==0){

            run_t.time_setting_mode = works_time ;
			run_t.gAI =1;
		    display_ai_icon(run_t.gAI) ;
			display_not_ai_timer_mode();
	        power_on_init_disp_time_numbers();
		}

      
	 
       disp_timer_run_times();
       counter_time_timing_fun(); 
	   
//	   if(timer_time_switch_f==1){
//	 	timer_time_switch_f++;
//	    display_time_hours_minutes_fun();
//	   }
    
}
/**
* @brief 
* @note
* @param
* @return
*/

static void handle_fan_warning_mode(void)
{
    fan_default_warning_fun();
         if(run_t.ptc_warning == 1){

             if(gpro_t.gTimer_fan_to_ptc_warning > 2){

                   gpro_t.gTimer_fan_to_ptc_warning = 0;
               

                 run_t.time_setting_mode = PTC_WARNING;

            }

         }
}
/**
* @brief 
* @note
* @param
* @return
*/

static void handle_ptc_warning_mode(void)
{

     ptc_high_temp_warning_fun();
         if(run_t.fan_warning == 1){

             if(gpro_t.gTimer_fan_to_ptc_warning > 2){

                   gpro_t.gTimer_fan_to_ptc_warning = 0;
               

               run_t.time_setting_mode =FAN_WARNING;

            }

         }

   
}



/******************************************************************************
	*
	*Function Name:void counter_time_timing_fun(void)
	*Function: display of icon , "1" -> ON ,"0"-> OFF
	*Input Ref:NO
	*Return Ref:NO
	*
******************************************************************************/
static void counter_time_timing_fun(void)
{
  
   if(run_t.gTimer_disp_time_seconds >59){ //minute
		
		run_t.gTimer_disp_time_seconds=0;
        run_t.dispTime_minutes ++;
       
          
		if(run_t.dispTime_minutes > 59){
			run_t.dispTime_minutes=0;
			run_t.dispTime_hours ++;
		    
		if(run_t.dispTime_hours >23){
			run_t.dispTime_hours=0;

		}

		}
      if(lcd_t.display_beijing_time_flag == 0){
	      sendCmdNote_to_threeData(0x6C,run_t.dispTime_hours, run_t.dispTime_minutes,run_t.gTimer_disp_time_seconds);
	      tx_thread_sleep(2);
	  }
   	}
   

}
/******************************************************************************
	*
	*Function Name:static void power_on_init_disp_time_numbers(void)
	*Function: 显示时间的4位数字-小时:分钟
	*Input Ref:NO
	*Return Ref:NO
	*
******************************************************************************/
static void power_on_init_disp_time_numbers(void)
{
     
   
	TM1723_Write_Display_Data(0xC9,(T8_HUM+lcdNumber4_Low[lcd_t.number4_low]+lcdNumber5_High[lcd_t.number5_high]) & 0xff);

    TM1723_Write_Display_Data(0xCA,lcdNumber5_Low[lcd_t.number5_low]+lcdNumber6_High[lcd_t.number6_high]);//display digit
    if(gpro_t.disp_time_colon_flag==1)
     TM1723_Write_Display_Data(0xCB,TIME_COLON+lcdNumber6_Low[lcd_t.number6_low]+lcdNumber7_High[lcd_t.number7_high]);//d
    else 
		 TM1723_Write_Display_Data(0xCB,TIME_NO_COLON+lcdNumber6_Low[lcd_t.number6_low]+lcdNumber7_High[lcd_t.number7_high]);//d
    fan_disp_speed_leaf(1);

}
/**
*
*@brief 
*@notice don't time colon ":"
*@param
*@return 
*
**/
static void disp_timer_set_numbers(void)
{
     
    TM1723_Write_Display_Data(0xC9,(T8_HUM+lcdNumber4_Low[lcd_t.number4_low]+lcdNumber5_High[lcd_t.number5_high]) & 0xff);

    TM1723_Write_Display_Data(0xCA,lcdNumber5_Low[lcd_t.number5_low]+lcdNumber6_High[lcd_t.number6_high]);//display digit
 
    TM1723_Write_Display_Data(0xCB,TIME_NO_COLON+lcdNumber6_Low[lcd_t.number6_low]+lcdNumber7_High[lcd_t.number7_high]);//d
    fan_disp_speed_leaf(1);

}

/**
*
*@brief 
*@notice
*@param
*@return 
*
**/
void display_not_ai_timer_mode(void)
{
    switch(run_t.time_setting_mode){//switch(run_t.setup_timer_timing_item){

      case works_time:
        
	    display_ai_icon(1) ;
	
        
         lcd_t.number5_low=(run_t.dispTime_hours ) /10;
         lcd_t.number5_high =lcd_t.number5_low;//(run_t.dispTime_hours) /10;

    	 lcd_t.number6_low = (run_t.dispTime_hours ) %10;;
    	 lcd_t.number6_high =  lcd_t.number6_low ;//(run_t.dispTime_hours ) %10;
         
         lcd_t.number7_low = (run_t.dispTime_minutes )/10;
    	 lcd_t.number7_high = lcd_t.number7_low;//(run_t.dispTime_minutes )/10;

    	 lcd_t.number8_low = (run_t.dispTime_minutes )%10;
    	 lcd_t.number8_high = lcd_t.number8_low ;//(run_t.dispTime_minutes )%10;

     
      
    
    break;

     case timer_time:
 
           display_ai_icon(0) ;
        
           	lcd_t.number5_low=(run_t.timer_time_hours ) /10;
			lcd_t.number5_high =lcd_t.number5_low;//(run_t.timer_time_hours) /10;

			lcd_t.number6_low = (run_t.timer_time_hours ) %10;;
			lcd_t.number6_high = lcd_t.number6_low;//(run_t.timer_time_hours ) %10;

			lcd_t.number7_low = (run_t.timer_time_minutes )/10;
			lcd_t.number7_high =lcd_t.number7_low ;//(run_t.timer_time_minutes)/10;

			lcd_t.number8_low = (run_t.timer_time_minutes)%10;
			lcd_t.number8_high = lcd_t.number8_low ;//(run_t.timer_time_minutes )%10;


    break;

     }

     


//	power_on_init_disp_time_numbers();


}

/**************************************************************************************************
*
*Function Name:void set_temperature_compare_value_fun(void)
*Function:
*Input Ref:
*Return Ref:
*
*****************************************************************************************************/
void set_temperature_compare_value_fun(void)
{
    static uint8_t counter;

    if(run_t.fan_warning ==1 || run_t.ptc_warning ==1 || gpro_t.stopTwoHours_flag==1 || run_t.ptc_on_off_flag == 1\
		|| gpro_t.key_set_temperature==1)return ;

	if(gpro_t.temp_real_value > 60)return ; //WT.EDIT 2026.01.19

	uint8_t real_temp = gpro_t.temp_real_value;
	uint8_t target_temp;


	
	if(gpro_t.set_temp_value_success==1){
          target_temp = run_t.wifi_set_temperature ;//gpro_t.key_set_temperature;
    }
	else{
	   target_temp = DEFAULT_TEMP;

      }

	if(real_temp >= DEFAULT_TEMP){

          run_t.dry = 0;
		  ptc_state = PTC_STATE_OFF ;
	      gpro_t.first_set_ptc_on  = 1;
		  send_ptc_command(0);
	      tx_thread_sleep(1);
  
		  return ;
	}

	if(ptc_state == PTC_STATE_OFF){

	    if(gpro_t.first_ptc_on==0 || gpro_t.first_ptc_on==1){

			if(real_temp < target_temp){
               run_t.dry = 1;
			   ptc_state = PTC_STATE_ON ;
			   if(gpro_t.first_ptc_on==1)gpro_t.first_set_ptc_on  = 2;
			   
			   send_ptc_command(1);
			    tx_thread_sleep(1);
			}
		}
		else{
            if(real_temp < (target_temp -2)){

                 run_t.dry = 1;
				ptc_state = PTC_STATE_ON ;
			   send_ptc_command(1);
			   tx_thread_sleep(1);

			}


		}

	}
	else{
        if(real_temp >= target_temp){
            run_t.dry = 0;

		   ptc_state = PTC_STATE_OFF ;
		   send_ptc_command(0);
	       tx_thread_sleep(1);
		}

	}


	
}

/**
*@brief 
*@notice
*@param 
**/
static void send_ptc_command(uint8_t on_off)
{
  SendData_Set_Command(0x22,on_off); //open ptc  
  tx_thread_sleep(2);
  //while(!gpro_t.uart1_tx_done);
  //uart1_tx_get();

}

/**
*@brief 
*@notice
*@param 
**/
void direct_comparison_temp(void)
{
     gpro_t.first_set_ptc_on=0;
	 gpro_t.first_rcoder_ptc_on_flag =0;
	 run_t.ptc_on_off_flag =0;

	if(run_t.wifi_set_temperature <= gpro_t.temp_real_value){// && gpro_t.smart_phone_turn_off_ptc_flag ==0){

               run_t.dry = 0;
			 
			  SendData_Set_Command(0x23,0x00); //close ptc 
	          tx_thread_sleep(1);
    }
    else{

	   run_t.dry = 1;
	    SendData_Set_Command(0x23,0x01); //open ptc 
	   tx_thread_sleep(1);
	  
			    
	  }

}

/**************************************************************************************************
*
*Function Name:static void fan_default_warning_fun(void)
*Function : fan of warning ,fan of leaf stop
*
*
****************************************************************************************************/
static void fan_default_warning_fun(void)
{

   TM1723_Write_Display_Data(0xC9,(0x01+lcdNumber4_Low[lcd_t.number4_low]+lcdNumber5_High_E[0]));//display digital 'E'
   TM1723_Write_Display_Data(0xCA,T15+lcdNumber5_Low_E[0]+lcdNumber6_High_r[0]);//display digital 'r'  
   if(gpro_t.disp_time_colon_flag ==1){           
         TM1723_Write_Display_Data(0xCB,TIME_COLON+lcdNumber6_Low_r[0]+lcdNumber7_High[0]);//display "6,7"
    }
   else{
      TM1723_Write_Display_Data(0xCB,TIME_NO_COLON+lcdNumber6_Low_r[0]+lcdNumber7_High[0]);//
   }
 
   TM1723_Write_Display_Data(0xCC,T14+lcdNumber7_Low[0]+lcdNumber8_High[2]);//display "02'
   TM1723_Write_Display_Data(0xCE,T13+lcdNumber8_Low[2]);//display "t,c"
   TM1723_Write_Display_Data(0xCF,(T10+T11+T12+T16));//

}
/**************************************************************************************************
*
*Function Name:static void ptc_high_temp_warning_fun(void)
*Function : fan of warning ,fan of leaf stop
*Input Ref:
*Return Ref:
*
****************************************************************************************************/
static void ptc_high_temp_warning_fun(void)
{
   #if 0
    TM1723_Write_Display_Data(0xC9,(0x01+lcdNumber4_Low[lcd_t.number4_low]+lcdNumber5_High_E[0]));//display digital 'E'
   // TM1723_Write_Display_Data(0xCA,T15+lcdNumber5_Low_E[0]+lcdNumber6_High_r[0]);//display digital 'r'  
    if(gpro_t.disp_time_colon_flag ==1){           
        TM1723_Write_Display_Data(0xCB,TIME_COLON+lcdNumber6_Low_r[0]+lcdNumber7_High[0]);//display "6,7"
    }
    else {
        TM1723_Write_Display_Data(0xCB,TIME_NO_COLON+lcdNumber6_Low_r[0]+lcdNumber7_High[0]);//
    }
 
    /***************************display fan of leaf *********************************************/
    if(run_t.fan_warning ==0){
        if(lcd_t.gTimer_fan_10ms >39 && lcd_t.gTimer_fan_10ms<80){




        TM1723_Write_Display_Data(0xCA,T15+lcdNumber5_Low_E[0]+lcdNumber6_High_r[0]);//display 
        TM1723_Write_Display_Data(0xCC,lcdNumber7_Low[0]+lcdNumber8_High[1]);//display "7,8'
        TM1723_Write_Display_Data(0xCE,T13+lcdNumber8_Low[1]);//display "t,c"
        TM1723_Write_Display_Data(0xCF,((T11+T16)& 0x05));//

        }
        else if(lcd_t.gTimer_fan_10ms <40){


        TM1723_Write_Display_Data(0xCA,lcdNumber5_Low_E[0]+lcdNumber6_High_r[0]);//display digital '5,6'
        TM1723_Write_Display_Data(0xCC,T14+lcdNumber7_Low[0]+lcdNumber8_High[1]);//display "t,c
        TM1723_Write_Display_Data(0xCE,lcdNumber8_Low[1]);//display "1"

        TM1723_Write_Display_Data(0xCF,((T16+T12+T10)&0x0B));//

        }
        else if(lcd_t.gTimer_fan_10ms > 79){
        lcd_t.gTimer_fan_10ms=0;
        }

    }
	#endif 

	 uint8_t colon_data;
	 static uint8_t fan_time ;

	 // 显示 'E'
    TM1723_Write_Display_Data(0xC9, 0x01 + lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High_E[0]);

    // 显示时间冒号状态
    colon_data = (gpro_t.disp_time_colon_flag == 1) ? TIME_COLON : TIME_NO_COLON;
    TM1723_Write_Display_Data(0xCB, colon_data + lcdNumber6_Low_r[0] + lcdNumber7_High[0]);

    // 风扇警告显示逻辑
    if (run_t.fan_warning == 0) {
		if(lcd_t.gTimer_fan_10ms > 40){
        lcd_t.gTimer_fan_10ms=0;
		fan_time = !fan_time;

		}

        if (fan_time){
            TM1723_Write_Display_Data(0xCA, lcdNumber5_Low_E[0] + lcdNumber6_High_r[0]);
            TM1723_Write_Display_Data(0xCC, T14 + lcdNumber7_Low[0] + lcdNumber8_High[1]);
            TM1723_Write_Display_Data(0xCE, lcdNumber8_Low[1]);
            TM1723_Write_Display_Data(0xCF, (T16 + T12 + T10) & 0x0B);
        }
        else {
            TM1723_Write_Display_Data(0xCA, T15 + lcdNumber5_Low_E[0] + lcdNumber6_High_r[0]);
            TM1723_Write_Display_Data(0xCC, lcdNumber7_Low[0] + lcdNumber8_High[1]);
            TM1723_Write_Display_Data(0xCE, T13 + lcdNumber8_Low[1]);
            TM1723_Write_Display_Data(0xCF, (T11 + T16) & 0x05);
        }
       
    }

}





