#include "bsp.h"


#define  LEAF_TOGGLE_THRESHOLD   0


static void display_works_or_timer_timing_fun(void);



/**
* @brief 
* @note
* @param
* @return
*/
static void display_works_or_timer_timing_fun(void)
{
      
    if(run_t.time_setting_mode ==works_time){//switch(run_t.setup_timer_timing_item){

    
        
         lcd_t.number5_low=(run_t.dispTime_hours ) /10;
         lcd_t.number5_high =lcd_t.number5_low;//(run_t.dispTime_hours) /10;

    	 lcd_t.number6_low = (run_t.dispTime_hours ) %10;;
    	 lcd_t.number6_high =  lcd_t.number6_low ;//(run_t.dispTime_hours ) %10;
         
         lcd_t.number7_low = (run_t.dispTime_minutes )/10;
    	 lcd_t.number7_high = lcd_t.number7_low;//(run_t.dispTime_minutes )/10;

    	 lcd_t.number8_low = (run_t.dispTime_minutes )%10;
    	 lcd_t.number8_high = lcd_t.number8_low ;//(run_t.dispTime_minutes )%10;

    }
	else if(run_t.time_setting_mode ==timer_time){

	        lcd_t.number5_low=(run_t.timer_time_hours) /10;
			lcd_t.number5_high =lcd_t.number5_low;//(run_t.dispTime_hours) /10;
	
			lcd_t.number6_low = (run_t.timer_time_hours ) %10;;
			lcd_t.number6_high =  lcd_t.number6_low ;//(run_t.dispTime_hours ) %10;
			
			lcd_t.number7_low = (run_t.timer_time_minutes )/10;
			lcd_t.number7_high = lcd_t.number7_low;//(run_t.dispTime_minutes )/10;
	
			lcd_t.number8_low = (run_t.timer_time_minutes )%10;
			lcd_t.number8_high = lcd_t.number8_low ;//(run_t.dispTime_minutes )%10;



	}
		

}




/**
* @brief 
* @note
* @param
* @return
*/
void disp_fan_leaf_run_icon(void)
{
   
   /* 主显示更新：仅当无风扇/ptc 报警时执行 */
    if (run_t.fan_warning == 1 || run_t.ptc_warning == 1) return ;

	 
       #if 0
        if (run_t.time_setting_mode == setup_timer){

		     if(lcd_t.gTimer_leaf_counter> 4){ //10*20
                   lcd_t.gTimer_leaf_counter=0;
			 
					lcd_t.gTimer_leaf_counter = 0;
					gpro_t.disp_fan_switch_flag	^= 1;
			
		
			   fan_runing_fun();
		    }

		}
		else 
      #endif

		if(lcd_t.gTimer_leaf_counter > LEAF_TOGGLE_THRESHOLD){ //10ms *3 
	              
	            lcd_t.gTimer_leaf_counter = 0;
	            gpro_t.disp_fan_switch_flag = gpro_t.disp_fan_switch_flag ^1;
	         

			 }

	      fan_runing_fun();


}
/**
* @brief 
* @note
* @param
* @return
*/
uint8_t fan_leaf_counter;
void fan_runing_fun(void)
{
    display_works_or_timer_timing_fun();
	fan_leaf_counter ++;	
	tx_thread_sleep(10);
	switch(gpro_t.disp_fan_switch_flag){

	case 0: //T15 ,T11 ,T13 ->ON
		TM1723_Write_Display_Data(0xC9,(T8_HUM + lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & 0xFF);

		TM1723_Write_Display_Data(0xCA,(T15 + lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high])& 0xff);

	if(run_t.time_setting_mode == setup_timer){
		TM1723_Write_Display_Data(0xCB,TIME_NO_COLON + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high] & 0xff);	
	}
	else{
		TM1723_Write_Display_Data(0xCB,(gpro_t.disp_time_colon_flag + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high])& 0xff);
	}

    //T14
	TM1723_Write_Display_Data(0xCC,(T14_NO+lcdNumber7_Low[lcd_t.number7_low]+lcdNumber8_High[lcd_t.number8_high])& 0xff);//display "7,8'

	TM1723_Write_Display_Data(0xCF,(T16+T11)& 0xff);//T11

	
	
	if(run_t.disp_wind_speed_grade >66){//T13
		TM1723_Write_Display_Data(0xCE,(T13+lcdNumber8_Low[lcd_t.number8_low]+ WIND_SPEED_FULL) & 0xff);
	}
	else if(run_t.wifi_link_net_success ==1){ //WT.EDIT 2025.04.16 logic is not rigorous.
		if(run_t.disp_wind_speed_grade >33 && run_t.disp_wind_speed_grade <67){
		TM1723_Write_Display_Data(0xCE,(T13+lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_TWO) & 0xff);
		}
	}
	else if(run_t.disp_wind_speed_grade <34){
		TM1723_Write_Display_Data(0xCE,(T13+lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_ONE) & 0xff);
	}


	
	TIM1723_Write_Cmd(LUM_VALUE);


	break;

	case 1://T10+T12+T14 ON
		TM1723_Write_Display_Data(0xC9,(T8_HUM + lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & 0xFF);
		TM1723_Write_Display_Data(0xCA,(T15_NO + lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high]) & 0xff);

        if(run_t.time_setting_mode == setup_timer){
			TM1723_Write_Display_Data(0xCB,(TIME_NO_COLON + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high])& 0xff);	
        }
		else
		 TM1723_Write_Display_Data(0xCB,(gpro_t.disp_time_colon_flag + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]) & 0xff);

      

       // TM1723_Write_Display_Data(0xCF,T16+T12);//T12,T11,T10

		TM1723_Write_Display_Data(0xCC,(T14+lcdNumber7_Low[lcd_t.number7_low]+lcdNumber8_High[lcd_t.number8_high])& 0xff);//display "7,8'	

	//T13
	if(run_t.disp_wind_speed_grade >66){//T13
		TM1723_Write_Display_Data(0xCE,(T13_NO+lcdNumber8_Low[lcd_t.number8_low]+ WIND_SPEED_FULL) & 0xff);
	}
	else if(run_t.wifi_link_net_success ==1){ //WT.EDIT 2025.04.16 logic is not rigorous.
	if(run_t.disp_wind_speed_grade >33 && run_t.disp_wind_speed_grade <67){
		TM1723_Write_Display_Data(0xCE,(T13_NO+lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_TWO) & 0xff);
	}
	}
	else if(run_t.disp_wind_speed_grade <34){
		TM1723_Write_Display_Data(0xCE,(T13_NO+lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_ONE) & 0xff);
	}



	TM1723_Write_Display_Data(0xCF,(T16+T10+T12)& 0xff);//T12,T11,T10
	  //T14
	
	TIM1723_Write_Cmd(LUM_VALUE);	
	break;

  
	
	}
 }
   

void disp_time_four_numbers_init(void)
{
  
   
	TM1723_Write_Display_Data(0xC9,(T8_HUM + lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & 0xFF);

	TM1723_Write_Display_Data(0xCA,(T15 + lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high])& 0xff);

	if(run_t.time_setting_mode == setup_timer){
		TM1723_Write_Display_Data(0xCB,TIME_NO_COLON + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high] & 0xff);	
	}
	else{
		TM1723_Write_Display_Data(0xCB,(gpro_t.disp_time_colon_flag + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high])& 0xff);
	}

    //T14
	TM1723_Write_Display_Data(0xCC,(T14_NO+lcdNumber7_Low[lcd_t.number7_low]+lcdNumber8_High[lcd_t.number8_high])& 0xff);//display "7,8'

	TM1723_Write_Display_Data(0xCF,(T16+T11)& 0xff);//T11

	
	
	if(run_t.disp_wind_speed_grade >66){//T13
		TM1723_Write_Display_Data(0xCE,(T13+lcdNumber8_Low[lcd_t.number8_low]+ WIND_SPEED_FULL) & 0xff);
	}
	else if(run_t.wifi_link_net_success ==1){ //WT.EDIT 2025.04.16 logic is not rigorous.
		if(run_t.disp_wind_speed_grade >33 && run_t.disp_wind_speed_grade <67){
		TM1723_Write_Display_Data(0xCE,(T13+lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_TWO) & 0xff);
		}
	}
	else if(run_t.disp_wind_speed_grade <34){
		TM1723_Write_Display_Data(0xCE,(T13+lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_ONE) & 0xff);
	}


	
	TIM1723_Write_Cmd(LUM_VALUE);


}


