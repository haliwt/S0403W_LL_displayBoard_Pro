#include "bsp.h"


// ====== 配置区 ======
#define ADDR_DIGIT45  0xC9
#define ADDR_DIGIT56  0xCA
#define ADDR_DIGIT67  0xCB   //time colon
#define ADDR_DIGIT78  0xCC
#define ADDR_WIND     0xCE
#define ADDR_MISC     0xCF

#define T8_HUM        0x01

#define  LEAF_TOGGLE_THRESHOLD   0//1//3

#define LEAT_TOGGLE_SLOW         200

volatile bool blink_on = true;       // 闪烁状态
volatile uint16_t blink_counter = 0; // 闪烁计数

static void display_works_or_timer_timing_fun(void);
static void disp_fan_speed_level(void);
static void donot_disp_T13_icon_fan_speed_level(void);
//static void fan_set_timer_disp_leaf(uint8_t disp);


// 六片叶子单点旋转序列 
static const uint8_t leaf_single[6] = {T15, T10, T11, T12, T13, T14 };


// ====== 定时器中断（10ms周期） ======
//if (run_t.gTimer_digital5678_ms>= 4) { // 50*10ms = 500ms
//		 run_t.gTimer_digital5678_ms=0;
//		   blink_on = !blink_on;
//	}


uint8_t leaf_step = 0;

/**
* @brief 
* @note // ====== 风速显示 ======
* @param
* @return
*/
//static void update_wind_speed_display(uint8_t mask) 
//{
//    uint8_t base = lcdNumber8_Low[lcd_t.number8_low];
//    if (run_t.disp_wind_speed_grade > 66) {
//        TM1723_Write_Display_Data(ADDR_WIND, (T13 + base + WIND_SPEED_FULL) & mask);
//    } else if (run_t.disp_wind_speed_grade > 33) {
//        TM1723_Write_Display_Data(ADDR_WIND, (T13 + base + WIND_SPEED_TWO) & mask);
//    } else {
//        TM1723_Write_Display_Data(ADDR_WIND, (T13 + base + WIND_SPEED_ONE) & mask);
//    }
//}

/**
* @brief 
* @note // ====== 数码管显示 ======
* @param
* @return
*/

void display_digits(uint8_t mask, bool blink) 
{

   if(lcd_t.gTimer_leaf_fast_counter > 0){ //3*100ms
              
       lcd_t.gTimer_leaf_fast_counter = 0;
       gpro_t.disp_fan_switch_flag = gpro_t.disp_fan_switch_flag^ 0x01;
   }

	  switch(gpro_t.disp_fan_switch_flag){

			 case 0: //T15 ,T11 ,T13 ->ON
				TM1723_Write_Display_Data(0xC9,(T8_HUM + lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & 0xFF);

			    TM1723_Write_Display_Data(0xCA,T15 + lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high]);
                if(run_t.time_setting_mode == setup_timer){
					TM1723_Write_Display_Data(0xCB,TIME_NO_COLON + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]);
				}
				else
				   TM1723_Write_Display_Data(0xCB,gpro_t.disp_time_colon_flag + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]);

				TM1723_Write_Display_Data(0xCF,T16+T11);//T12,T11,T10
               
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
				  

                //T14
				TM1723_Write_Display_Data(0xCC,T14_NO+lcdNumber7_Low[lcd_t.number7_low]+lcdNumber8_High[lcd_t.number8_high]);//display "7,8'

			 
			 break;

			 case 1://T10+T12+T14 ON
                 TM1723_Write_Display_Data(0xC9,(T8_HUM + lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & 0xFF);

			     TM1723_Write_Display_Data(0xCA,T15_NO + lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high]);
                 if(run_t.time_setting_mode == setup_timer){
					TM1723_Write_Display_Data(0xCB,TIME_NO_COLON + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]);
				 }
				 else TM1723_Write_Display_Data(0xCB,gpro_t.disp_time_colon_flag + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]);
				TM1723_Write_Display_Data(0xCF,T16+T10+T12);//T12,T11,T10
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

                //T14
				 TM1723_Write_Display_Data(0xCC,T14+lcdNumber7_Low[lcd_t.number7_low]+lcdNumber8_High[lcd_t.number8_high]);//display "7,8'
                
			   break;
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

	 
       
        if (run_t.time_setting_mode == setup_timer){

		     if(lcd_t.gTimer_leaf_slow_counter > 80){ //10*20
						 
					lcd_t.gTimer_leaf_counter = 0;
					gpro_t.disp_fan_switch_flag	^= 1;
			
		
			   fan_runing_fun();
		    }

		}
		else if(lcd_t.gTimer_leaf_counter > LEAF_TOGGLE_THRESHOLD){ //100ms *3 
	              
	            lcd_t.gTimer_leaf_counter = 0;
	            gpro_t.disp_fan_switch_flag  ^= 1;
	         

			  fan_runing_fun();
		
         }



}
/**
* @brief 
* @note
* @param
* @return
*/

void fan_runing_fun(void)
{

			
	switch(gpro_t.disp_fan_switch_flag){

	case 0: //T15 ,T11 ,T13 ->ON
		TM1723_Write_Display_Data(0xC9,(T8_HUM + lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & 0xFF);

		TM1723_Write_Display_Data(0xCA,T15 + lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high]);

	if(run_t.time_setting_mode == setup_timer){
		TM1723_Write_Display_Data(0xCB,TIME_NO_COLON + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]);	
	}
	else{
		TM1723_Write_Display_Data(0xCB,gpro_t.disp_time_colon_flag + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]);
	}
	TM1723_Write_Display_Data(0xCF,T16+T11);//T12,T11,T10
	//TM1723_Write_Display_Data(0xCE,(lcdNumber8_Low[lcd_t.number8_low]+ WIND_SPEED_FULL) & 0xff);

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


	//T14
		TM1723_Write_Display_Data(0xCC,T14_NO+lcdNumber7_Low[lcd_t.number7_low]+lcdNumber8_High[lcd_t.number8_high]);//display "7,8'


	break;

	case 1://T10+T12+T14 ON
		TM1723_Write_Display_Data(0xC9,(T8_HUM + lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & 0xFF);
		TM1723_Write_Display_Data(0xCA,T15_NO + lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high]);

        if(run_t.time_setting_mode == setup_timer){
			TM1723_Write_Display_Data(0xCB,TIME_NO_COLON + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]);	
        }
		else
		 TM1723_Write_Display_Data(0xCB,gpro_t.disp_time_colon_flag + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]);

		TM1723_Write_Display_Data(0xCF,T16+T10+T12);//T12,T11,T10
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

	//T14
		TM1723_Write_Display_Data(0xCC,T14+lcdNumber7_Low[lcd_t.number7_low]+lcdNumber8_High[lcd_t.number8_high]);//display "7,8'

	break;
	}
 }
   
    	

void disp_time_colon_fun(void)
{
     static uint8_t colon_flag;
	 /* 冒号闪烁计数器（与上面独立，保持原行为） */
    if (lcd_t.gTimer_colon_counter > 0) {
        lcd_t.gTimer_colon_counter = 0;
		
		colon_flag = colon_flag ^ 0x01;
   
        gpro_t.disp_time_colon_flag = (colon_flag != 0) ? 1 : 0;

		 /* 冒号根据全局标志显示/隐藏 */
	     // colon_base = gpro_t.disp_time_colon_flag ? TIME_COLON : TIME_NO_COLON;
         // TM1723_Write_Display_Data(0xCB,
               //   gpro_t.disp_time_colon_flag + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]);
    }
            	
}



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
*@brief : display time 4 number hours,mintues,
**/

void display_time_hours_minutes_fun(void)
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

   fan_runing_fun();
}
/************************************************************
*
*Funtion Name:void fan_disp_speed_leaf(uint8_t disp)
*
*
*
************************************************************/
void fan_disp_speed_leaf(uint8_t disp)
{

    if(disp==0){

      TM1723_Write_Display_Data(0xCC,T14_NO+lcdNumber7_Low[lcd_t.number7_low]+lcdNumber8_High[lcd_t.number8_high]);//display "7,8'
      disp_fan_speed_level();
      TM1723_Write_Display_Data(0xCF,((T11+T16)& 0x05));//

    }
    else{

       TM1723_Write_Display_Data(0xCC,T14+lcdNumber7_Low[lcd_t.number7_low]+lcdNumber8_High[lcd_t.number8_high]);//display "t,c
       donot_disp_T13_icon_fan_speed_level();
       TM1723_Write_Display_Data(0xCF,((T16+T12+T10)&0x0B));//

    }



}

/******************************************************************************
	*
	*Function Name:static void disp_fan_speed_level(void)
	*Function: 
	*Input Ref: NO
	*Return Ref: NO
	*
******************************************************************************/
static void disp_fan_speed_level(void)
{

     if(run_t.disp_wind_speed_grade >66){
	 	TM1723_Write_Display_Data(0xCE,(T13+lcdNumber8_Low[lcd_t.number8_low]+ WIND_SPEED_FULL) & 0xff);
	  }
	  else if(run_t.wifi_link_net_success ==1){ //WT.EDIT 2025.04.16 logic is not rigorous.
		  if(run_t.disp_wind_speed_grade >33 && run_t.disp_wind_speed_grade <67){
		     TM1723_Write_Display_Data(0xCE,(T13+lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_TWO) & 0xff);

		  }
		  else if(run_t.disp_wind_speed_grade <34){
			 TM1723_Write_Display_Data(0xCE,(T13+lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_ONE) & 0xff);
		  }
	  }

}

/*************************************************************************************
    *
    *Function Name:static void donot_disp_T13_icon_fan_speed_level(void)
    *Function:
    *
    *
    *
**************************************************************************************/
static void donot_disp_T13_icon_fan_speed_level(void)
{
    if(run_t.disp_wind_speed_grade >66){
           TM1723_Write_Display_Data(0xCE,(lcdNumber8_Low[lcd_t.number8_low]+ WIND_SPEED_FULL) & 0xff);
     }
	 else if(run_t.wifi_link_net_success ==1){ //WT.EDIT 2025.04.16 the logic is not rigorous
         if(run_t.disp_wind_speed_grade >33 && run_t.disp_wind_speed_grade <67){
            TM1723_Write_Display_Data(0xCE,(lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_TWO) & 0xff);
    
         }
         else if(run_t.disp_wind_speed_grade <34){
            TM1723_Write_Display_Data(0xCE,(lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_ONE) & 0xff);
         }
	 }
}



void smg_power_on_init(void)
{
     TIM1723_Write_Cmd(0x00);
	 TIM1723_Write_Cmd(0x40);
	 TIM1723_Write_Cmd(0x44);

 
         
    run_t.smart_phone_set_temp_value_flag =0;//WT.EDIT 2025.01.15
    gpro_t.set_temp_value_success = 0;//WT.EDIT 2025.01.15
    gpro_t.key_set_temperature =0;//WT.EDIT 2025.01.15

   

	    lcd_t.number1_low= gpro_t.temp_real_value / 10;
	    lcd_t.number1_high =lcd_t.number1_low;

	    lcd_t.number2_low = gpro_t.temp_real_value% 10;
	    lcd_t.number2_high = lcd_t.number2_low;



	    lcd_t.number3_low= gpro_t.humidity_real_value /10;
	    lcd_t.number3_high = lcd_t.number3_low;

	    lcd_t.number4_low = gpro_t.humidity_real_value %10;
	    lcd_t.number4_high =   lcd_t.number4_low ;

    
     if(gpro_t.smart_phone_app_timer_power_on_flag == 0){
           
          TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high])&0xff);//display digital "temp
          TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);
          TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xff);//display "t,c"
	      TM1723_Write_Display_Data(0xC5,(WIFI_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xfff); //Wifi

	 }
     else{
         	
			TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high])&0xff);
			
		    TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
			
			TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xff);//display "t,c"
            TM1723_Write_Display_Data(0xC5, (WIFI_Symbol + (lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high])) & 0xff);
		
     }
	 
    TM1723_Write_Display_Data(0xC9,(T8_HUM + lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & 0xFF);
  



     TIM1723_Write_Cmd(LUM_VALUE);
    


}
