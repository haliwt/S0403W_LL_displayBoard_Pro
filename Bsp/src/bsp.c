#include "bsp.h"

process_state gpro_t;


//static void  disp_set_timer_timing_value_fun(void);

static void disp_normal_timing_handler(void);

static void fan_default_warning_fun(void);

static void ptc_high_temp_warning_fun(void);

static void power_on_init_disp_time_numbers(void);

uint8_t first_set_ptc_on;


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


void mode_key_long_fun(void)
{
   
   if(run_t.ptc_warning ==0){

       run_t.gModel=0;
       gpro_t.gTimer_disp_temp_humi_value=0;
       run_t.display_set_timer_or_works_time_mode = setup_timer;
      
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
   
  static uint8_t not_ai_mode_flag,not_ai_default=0xff;
  static uint8_t ai_mode_flag,ai_default = 0xff;
   switch(run_t.display_set_timer_or_works_time_mode){

    case works_time:

     if(lcd_t.display_beijing_time_flag == 0 && gpro_t.power_on_every_times == 1 ){

              gpro_t.power_on_every_times++;
              run_t.gTimer_disp_time_seconds=0; //WT.EDIT 2025.01.08
             

     }
     else if(lcd_t.display_beijing_time_flag == 1 && gpro_t.power_on_every_times == 1 ){

                gpro_t.power_on_every_times++;


     }
    
        
     if(run_t.power_on_disp_smg_number ==1){
       run_t.power_on_disp_smg_number++; 
       if(run_t.dispTime_hours> 24){
          run_t.dispTime_hours=0;
          run_t.dispTime_minutes =0;
       }
         lcd_t.number5_low=(run_t.dispTime_hours ) /10;
         lcd_t.number5_high =lcd_t.number5_low;//(run_t.dispTime_hours) /10;

    	 lcd_t.number6_low = (run_t.dispTime_hours ) %10;;
    	 lcd_t.number6_high =  lcd_t.number6_low ;//(run_t.dispTime_hours ) %10;
         
         lcd_t.number7_low = (run_t.dispTime_minutes )/10;
    	 lcd_t.number7_high = lcd_t.number7_low;//(run_t.dispTime_minutes )/10;

    	 lcd_t.number8_low = (run_t.dispTime_minutes )%10;
    	 lcd_t.number8_high = lcd_t.number8_low ;//(run_t.dispTime_minutes )%10;

     
      power_on_init_disp_time_numbers();
       

      }
      disp_normal_timing_handler(); 
    
    
     Setup_Timer_Times_Donot_Display();
     if(wifi_link_net_state()==1 && (ai_default != ai_mode_flag)){ //WT.EDIT 2025.01.03
             ai_default = ai_mode_flag;
             not_ai_mode_flag++;
             gpro_t.send_ack_cmd = ack_ai_mode;
             gpro_t.gTimer_again_send_power_on_off =0;
			 SendData_Set_Command(0x27,0x01); //MODE_AI,BUR NO_BUZZER);

     }
     

    break;


    case setup_timer:
		
      disp_set_timer_timing_value_fun();
      

    break;

    case timer_time:

       if(wifi_link_net_state()==1 && (not_ai_default != not_ai_mode_flag)){ //WT.EDIT 2025.01.03
             not_ai_default = not_ai_mode_flag;
             ai_mode_flag++;
             gpro_t.send_ack_cmd = ack_not_ai_mode;
             gpro_t.gTimer_again_send_power_on_off =0;
			 SendData_Set_Command(0x27,0x02); //NOT_MODE_AI,BUR NO_BUZZER);

        }
       disp_timer_run_times();
       Works_Counter_Time();

    break;

    case FAN_WARNING: //fan warning 

      
         fan_default_warning_fun();
         if(run_t.ptc_warning == 1){

             if(gpro_t.gTimer_fan_to_ptc_warning > 2){

                   gpro_t.gTimer_fan_to_ptc_warning = 0;
               

                 run_t.display_set_timer_or_works_time_mode = PTC_WARNING;

            }

         }

    break;

    case PTC_WARNING: //ptc warning

         ptc_high_temp_warning_fun();
         if(run_t.fan_warning == 1){

             if(gpro_t.gTimer_fan_to_ptc_warning > 2){

                   gpro_t.gTimer_fan_to_ptc_warning = 0;
               

               run_t.display_set_timer_or_works_time_mode =FAN_WARNING;

            }

         }

    break;

    }

}

/******************************************************************************
	*
	*Function Name:void disp_normal_timing_handler(void)
	*Function: display of icon , "1" -> ON ,"0"-> OFF
	*Input Ref:NO
	*Return Ref:NO
	*
******************************************************************************/
static void disp_normal_timing_handler(void)
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
     
     

	  } 
    

}
/******************************************************************************
	*
	*Function Name:static void power_on_init_disp_time_numbers(void)
	*Function: display of icon , "1" -> ON ,"0"-> OFF
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

/**************************************************************************************************
*
*Function Name:void static disp_set_timer_timing_value_fun(void)
*
*
*
*****************************************************************************************************/
#if 0
static void disp_set_timer_timing_value_fun(void)
{
  
     static uint8_t tim_bit_1_hours,tim_bit_2_hours;
      gpro_t.gTimer_disp_temp_humi_value=0;
      if(run_t.gTimer_key_timing < 4){

            tim_bit_2_hours = run_t.timer_time_hours /10 ;
			tim_bit_1_hours = run_t.timer_time_hours %10;
           // HAL_Delay(20);
			run_t.timer_time_minutes  =0;

			
            
			lcd_t.number5_low=tim_bit_2_hours;
			lcd_t.number5_high =tim_bit_2_hours;

			lcd_t.number6_low = tim_bit_1_hours;
			lcd_t.number6_high = tim_bit_1_hours;

			lcd_t.number7_low=0;
			lcd_t.number7_high =0;

			lcd_t.number8_low = 0;
			lcd_t.number8_high = 0;

       
    
      if(run_t.gTimer_digital5678_ms < 3){
                 TM1723_Write_Display_Data(0xC9,(0x01+lcdNumber4_Low[lcd_t.number4_low]+lcdNumber5_High[lcd_t.number5_high]) & 0xff);//display digital '4,5'
                 TM1723_Write_Display_Data(0xCA,(T15+lcdNumber5_Low[lcd_t.number5_low]+lcdNumber6_High[lcd_t.number6_high]) & 0xff);
	             if(gpro_t.disp_time_colon_flag ==1)
                   TM1723_Write_Display_Data(0xCB,(TIME_COLON+lcdNumber6_Low[lcd_t.number6_low]+lcdNumber7_High[lcd_t.number7_high]) & 0xff);
                 else 
				 	 TM1723_Write_Display_Data(0xCB,TIME_NO_COLON+lcdNumber6_Low[lcd_t.number6_low]+lcdNumber7_High[lcd_t.number7_high]);//d

				 TM1723_Write_Display_Data(0xCC,(lcdNumber7_Low[lcd_t.number7_low]+lcdNumber8_High[lcd_t.number8_high]) & 0xff);
                 if(run_t.disp_wind_speed_grade >66){
                    TM1723_Write_Display_Data(0xCE,(T13+lcdNumber8_Low[lcd_t.number8_low]+ WIND_SPEED_FULL) & 0xff);
                  }
				  else if(run_t.disp_wind_speed_grade ==1){ //WT.EDIT 2025.04.16
	                  if(run_t.disp_wind_speed_grade >33 && run_t.disp_wind_speed_grade <67){
	                     TM1723_Write_Display_Data(0xCE,(T13+lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_TWO) & 0xff);
	    
	                  }
	                  else if(run_t.disp_wind_speed_grade <34){
	                     TM1723_Write_Display_Data(0xCE,(T13+lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_ONE) & 0xff);
	                  }
				  }

                  TM1723_Write_Display_Data(0xCF,(T16+T11)&0x05);//
       }
       else if(run_t.gTimer_digital5678_ms > 2 && run_t.gTimer_digital5678_ms < 5){
                 TM1723_Write_Display_Data(0xC9,(0x01+lcdNumber4_Low[lcd_t.number4_low]+lcdNumber5_High[lcd_t.number5_high]) & 0x0f);//display digital '4,5'
                 TM1723_Write_Display_Data(0xCA,(lcdNumber5_Low[lcd_t.number5_low]+lcdNumber6_High[lcd_t.number6_high]) & 0x01);
                 if(gpro_t.disp_time_colon_flag ==1)
				 	TM1723_Write_Display_Data(0xCB,(TIME_COLON+lcdNumber6_Low[lcd_t.number6_low]+lcdNumber7_High[lcd_t.number7_high]) & 0x01);
                 else
				 	 TM1723_Write_Display_Data(0xCB,TIME_NO_COLON+lcdNumber6_Low[lcd_t.number6_low]+lcdNumber7_High[lcd_t.number7_high]);//d

				 TM1723_Write_Display_Data(0xCC,(T14+lcdNumber7_Low[lcd_t.number7_low]+lcdNumber8_High[lcd_t.number8_high]) & 0x01);
                 if(run_t.disp_wind_speed_grade >66){
                    TM1723_Write_Display_Data(0xCE,(lcdNumber8_Low[lcd_t.number8_low]+ WIND_SPEED_FULL) & 0xE1);
                 }
				 else if(run_t.disp_wind_speed_grade ==1){ //WT.EDIT 2025.04.16
	                 if(run_t.disp_wind_speed_grade >33 && run_t.disp_wind_speed_grade <67){
	                     TM1723_Write_Display_Data(0xCE,(lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_TWO) & 0xE1);
	                 }
	                  else if(run_t.disp_wind_speed_grade <34){
	                     TM1723_Write_Display_Data(0xCE,(lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_ONE) & 0xE1);
	                 }
				 }
                 TM1723_Write_Display_Data(0xCF,((T16+T12+T10)&0x0B));//
       }
             
       if(run_t.gTimer_digital5678_ms > 3){ //4
           run_t.gTimer_digital5678_ms=0;
             

        }

       }
       else{ //4
            
              run_t.timer_time_minutes = 0;
              run_t.gTimer_timing=0;
             
               if(run_t.timer_time_hours !=0){  
                     
                      run_t.timer_timing_define_flag = timing_success;
                      run_t.display_set_timer_or_works_time_mode = timer_time;
                      run_t.gModel =2 ; //WT.EDIT 2024.11.08
                      if(wifi_link_net_state()==1){
                        gpro_t.receive_copy_cmd = ack_not_ai_mode; //WT.EDIT .2025.01.03
                        gpro_t.gTimer_again_send_power_on_off =0;
					      SendData_Set_Command(0x27,0x02); //MODE_AI,BUR NO_BUZZER);

                      }
                }
                else{
                     run_t.timer_timing_define_flag = timing_not_definition ;

                     run_t.display_set_timer_or_works_time_mode = works_time;
                     run_t.gModel =1 ;  //WT.EDIT 2024.11.08
                     
                     
                }
         
    
      }
                
                
    TIM1723_Write_Cmd(LUM_VALUE);//(0x97);//(0x94);//(0x9B);

 }
#endif 
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

    static uint8_t first_on_ptc;//first_set_ptc_on;

    if(gpro_t.temp_key_set_value ==1 && gpro_t.gTimer_set_temp_times > 1){ //
    
          run_t.smart_phone_set_temp_value_flag =1;
          gpro_t.temp_key_set_value =0;
          gpro_t.gTimer_temp_compare_value =0;

    }
	#if 0
    else if(gpro_t.set_temp_value_success == 1 && gpro_t.gTimer_temp_compare_value > 3 && gpro_t.temp_key_set_value ==0){

       gpro_t.gTimer_temp_compare_value =0;

      if(run_t.wifi_set_temperature <= gpro_t.temp_real_value && gpro_t.smart_phone_turn_off_ptc_flag ==0){

               run_t.dry = 0;
			   first_set_ptc_on=1; //the first open ptc heating //WT.DEDIT 2028.08.27 modify this flow codes
	           SendData_Set_Command(0x22,0x00); //close ptc 
	           osDelay(10);

	           SendData_Temp_Data(run_t.wifi_set_temperature);

			   osDelay(10);

			
            
      }
      else{

	       if(first_set_ptc_on==0){//the first open ptc heating //WT.DEDIT 2028.08.27 modify this flow codes
	          

				run_t.dry = 1;
				
	            SendData_Set_Command(0x22,0x01); //open ptc 
	            osDelay(10);
	            SendData_Temp_Data(run_t.wifi_set_temperature);
				osDelay(10);
            
	       }
		   else if(first_set_ptc_on==1 && (run_t.wifi_set_temperature -3) >= gpro_t.temp_real_value){//WT.DEDIT 2028.08.27 modify this flow codes
                 run_t.dry = 1;
				
	            SendData_Set_Command(0x22,0x01); //open ptc 
	            osDelay(10);
	            SendData_Temp_Data(run_t.wifi_set_temperature);
				osDelay(10);
			}


      }



    }
    else if(gpro_t.set_temp_value_success == 0 && gpro_t.gTimer_temp_compare_value > 5 && gpro_t.temp_key_set_value ==0){ 
        
        if(gpro_t.temp_real_value > 39){ // must be clouse ptc.
    
               first_on_ptc = 1;
               run_t.dry = 0;
               SendData_Set_Command(0x22,0x00); //close ptc 
          }
          else if(first_on_ptc == 1){
               
                 
               if(gpro_t.temp_real_value < 38){
                       run_t.dry = 1;
                       SendData_Set_Command(0x22,0x01); //open ptc  
                }
                   

          }
              
    }
  #endif 
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

}





