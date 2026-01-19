#include "bsp.h"





static void donot_disp_T13_icon_fan_speed_level(void);

//static void disp_fan_ptc_warning_time_colon_fun(void);

static void works_timer_disp_numaber(void);
static void display_temperture_humidity_value(void);

static void wifi_icon_blink_reg0xc5_handler(void);

static void display_lowbit_lunmber4_reg0xc9_handler(void);



uint8_t display_wifi_icon_flag ;



/*************************************************************************
 	*
 	* Function Name:static inline void set_wifi_icon(uint8_t symbol, uint8_t flag, uint8_t wifi_num_val
 	* Function :lcd display panel 
 	* Input Ref:NO
 	* Return Ref:NO
 	* 
*************************************************************************/ 
static inline void set_wifi_icon(uint8_t symbol, uint8_t flag, uint8_t wifi_num_val)
{
    TM1723_Write_Display_Data(0xC5, (symbol + wifi_num_val) & 0xff);
    display_wifi_icon_flag = flag;
}

static void wifi_icon_blink_reg0xc5_handler(void)
{
    uint8_t wifi_num_val = lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high];
	static uint8_t wifi_flag;
    uint8_t wifi_base;
    // 已连接状态
    if (run_t.wifi_link_net_success == 1) {
        run_t.wifi_led_fast_blink_flag = 0;
        set_wifi_icon(WIFI_Symbol, 2, wifi_num_val);
        TIM1723_Write_Cmd(LUM_VALUE);
        return;
    }

    // 未连接状态
    if (run_t.wifi_led_fast_blink_flag == 0) {
        // 慢闪逻辑

		if(lcd_t.gTimer_wifi_500ms > 99){
            lcd_t.gTimer_wifi_500ms=0;
			wifi_flag = !wifi_flag ;
		}

		wifi_base = wifi_flag ? WIFI_Symbol : WIFI_NO_Symbol;
        TM1723_Write_Display_Data(0xC5, (wifi_base + wifi_num_val) & 0xff);
    } 
	else {
        // 快闪逻辑
        if(lcd_t.gTimer_wifi_500ms > 9){//10*14=140ms
            lcd_t.gTimer_wifi_500ms=0;
			wifi_flag = !wifi_flag ;
		}
        wifi_base = wifi_flag ? WIFI_Symbol : WIFI_NO_Symbol;
        TM1723_Write_Display_Data(0xC5, (wifi_base + wifi_num_val) & 0xff);

        // 超时退出快闪
        if (run_t.gTimer_wifi_connect_counter > 120) {
            run_t.gTimer_wifi_connect_counter = 0;
            run_t.wifi_led_fast_blink_flag = 0;
        }
    }
}


/**
*	@brief 
*   @note
*   @param
*   @return 
*/
static void display_lowbit_lunmber4_reg0xc9_handler(void)
{
    if(run_t.display_set_timer_or_works_time_mode != setup_timer){

       TM1723_Write_Display_Data(0xC9,((T8_HUM +lcdNumber4_Low[lcd_t.number4_low])+lcdNumber5_High[lcd_t.number5_high])&0xff);

   }
}


/**
*	@brief 
*   @note
*   @param
*   @return 
*/
static void display_temperture_humidity_value(void)
{
   static uint8_t ai_mode_base;
   
   if(gpro_t.temp_key_set_value !=1 && gpro_t.gTimer_disp_temp_humi_value>4){

		gpro_t.gTimer_disp_temp_humi_value=0;

		//display address 0xC2 -> //WT.EDTI 2025.03.28
		Display_Kill_Dry_Ster_Icon();

        wifi_icon_blink_reg0xc5_handler();//TM1723_Write_Display_Data(0xC5,((0x01+lcdNumber2_Low[lcd_t.number2_low])+lcdNumber3_High[lcd_t.number3_high])&0xff);
		display_lowbit_lunmber4_reg0xc9_handler();
	   
	    TIM1723_Write_Cmd(LUM_VALUE);

       }
}
/**
*	@brief 
*   @note
*   @param
*   @return 
*/
void power_on_display_temp_handler(void)
{
   #if 0
	Display_Kill_Dry_Ster_Icon();

	//display address 0xC3
	if(run_t.gModel==1){
	TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high])& 0xff);//display	"AI icon"
	}
	else { 
	TM1723_Write_Display_Data(0xC3,((lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//don't display "AI icon"

	}

	//display address 0xC4
	TM1723_Write_Display_Data(0xC4,((0x01+lcdNumber2_Low[lcd_t.number2_low])+lcdNumber3_High[lcd_t.number3_high])&0xff);

	wifi_icon_blink_reg0xc5_handler();//TM1723_Write_Display_Data(0xC5,((0x01+lcdNumber2_Low[lcd_t.number2_low])+lcdNumber3_High[lcd_t.number3_high])&0xff);
	display_lowbit_lunmber4_reg0xc9_handler();
	#endif 

	// 显示干燥/杀菌/驱蚊图标 + 数字高位
    Display_Kill_Dry_Ster_Icon();

  
    // WiFi 图标闪烁处理（0xC5）
    wifi_icon_blink_reg0xc5_handler();

    // 显示低位数字（0xC9）
    display_lowbit_lunmber4_reg0xc9_handler();

    TIM1723_Write_Cmd(LUM_VALUE);

}


/******************************************************************************
	*
	*Function Name:static void Display_Kill_Dry_Ster_Icon(void)
	*Function: display of icon , "1" -> ON ,"0"-> OFF
	*Input Ref:
	*Return Ref:
	*
******************************************************************************/
 void Display_Kill_Dry_Ster_Icon(void)
{

  // uint8_t value ;
   static uint8_t  T4,T5,T6,T3=1;
   T4= 0x02; //
   T5= 0x04;
   T6= 0x08;

//	 value = 0x01
//        + (run_t.dry        ? DRY_Symbol  : DRY_NO_Symbol)
//        + (run_t.plasma     ? KILL_Symbol : KILL_NO_Symbol)
//        + (run_t.ultrasonic ? BUG_Symbol  : BUG_NO_Symbol)
//        + lcdNumber1_High[lcd_t.number1_high];

	 if(run_t.dry == 1 &&  run_t.plasma ==1 && run_t.ultrasonic ==1){//1

	      //TM1723_Write_Display_Data(0xC2, ((T3+T4+T5+T6) +lcdNumber1_High[lcd_t.number1_high]));
		 TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high])&0xff);

		 if(run_t.gModel==1){
			TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
		 else{
			 TM1723_Write_Display_Data(0xC3,((AI_NO_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
	 
		 TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xff);//display "t,c"
	 
          
	 }
	 else if(run_t.dry == 0 && run_t.plasma ==1 && run_t.ultrasonic ==1){//2

	    T4=0;
	   // TM1723_Write_Display_Data(0xC2, ((T3+T4+T5+T6) +lcdNumber1_High[lcd_t.number1_high]));
	   TM1723_Write_Display_Data(0xC2,((0X01+KILL_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high])&0xff);
       
	 if(run_t.gModel==1){
			TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
		 else{
			 TM1723_Write_Display_Data(0xC3,((AI_NO_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
	 
		 TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xff);//display "t,c"
	 
	 }
	 else if(run_t.dry == 1 && run_t.plasma ==0 && run_t.ultrasonic ==1){//3

	     T5=0;
	    //TM1723_Write_Display_Data(0xC2, ((T3+T4+T5+T6) +lcdNumber1_High[lcd_t.number1_high]));
	    TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high])&0xff);
        
	 if(run_t.gModel==1){
			TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
		 else{
			 TM1723_Write_Display_Data(0xC3,((AI_NO_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
	 
		 TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xff);//display "t,c"
	 
	 }
	 else if(run_t.dry == 1 && run_t.plasma ==1 && run_t.ultrasonic ==0){//4

	     T6=0;
	    //TM1723_Write_Display_Data(0xC2, ((T3+T4+T5+T6) +lcdNumber1_High[lcd_t.number1_high]));
	    TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_Symbol)+lcdNumber1_High[lcd_t.number1_high])&0xff);
	    
	 if(run_t.gModel==1){
			TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
		 else{
			 TM1723_Write_Display_Data(0xC3,((AI_NO_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
	 
		 TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xff);//display "t,c"
	 
	 }
	 else if(run_t.dry == 0 && run_t.plasma ==0 && run_t.ultrasonic ==1){//5

		 T4=0;
		 T5=0;
	    //TM1723_Write_Display_Data(0xC2, ((T3+T4+T5+T6) +lcdNumber1_High[lcd_t.number1_high]));
	    TM1723_Write_Display_Data(0xC2,((0X01+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high])&0xff);
        
	 if(run_t.gModel==1){
			TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
		 else{
			 TM1723_Write_Display_Data(0xC3,((AI_NO_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
	 
		 TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xff);//display "t,c"
	 
	 }
	 else if(run_t.dry == 1 && run_t.plasma ==0 && run_t.ultrasonic ==0){//6
	 
			
			 T5=0;
			 T6=0;
			//TM1723_Write_Display_Data(0xC2, ((T3+T4+T5+T6) +lcdNumber1_High[lcd_t.number1_high]));
            TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol)+lcdNumber1_High[lcd_t.number1_high])&0xff);
          
	 if(run_t.gModel==1){
			TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
		 else{
			 TM1723_Write_Display_Data(0xC3,((AI_NO_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
	 
		 TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xff);//display "t,c"
	 
	 }
      else if(run_t.dry == 0 && run_t.plasma ==1 && run_t.ultrasonic ==0){//7

	     T4=0;
		 T6=0;
		
	     //TM1723_Write_Display_Data(0xC2, ((T3+T4+T5+T6) +lcdNumber1_High[lcd_t.number1_high]));
         TM1723_Write_Display_Data(0xC2,((0X01+KILL_Symbol)+lcdNumber1_High[lcd_t.number1_high])&0xff);
       
	  if(run_t.gModel==1){
			 TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		  }
		  else{
			  TM1723_Write_Display_Data(0xC3,((AI_NO_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		  }
	  
		  TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xff);//display "t,c"
	  
	  }
	 else if(run_t.dry == 0 && run_t.plasma ==0 && run_t.ultrasonic ==0){//8
	
			T4=0;
			T5=0;
			T6=0;
		 //  TM1723_Write_Display_Data(0xC2, ((T3+T4+T5+T6) +lcdNumber1_High[lcd_t.number1_high]));

		 TM1723_Write_Display_Data(0xC2,((0X01)+lcdNumber1_High[lcd_t.number1_high])&0xff);
		 
	 if(run_t.gModel==1){
			TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
		 else{
			 TM1723_Write_Display_Data(0xC3,((AI_NO_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
		 }
	 
		 TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xff);//display "t,c"
	 
	 }

}



/**
*	@brief 
*   @note
*   @param
*   @return 
*/
static void works_timer_disp_numaber(void)
{     
    
    switch(run_t.display_set_timer_or_works_time_mode){//switch(run_t.setup_timer_timing_item){

      case works_time:
        
         lcd_t.number5_low=(run_t.dispTime_hours ) /10;
         lcd_t.number5_high =lcd_t.number5_low;//(run_t.dispTime_hours) /10;

    	 lcd_t.number6_low = (run_t.dispTime_hours ) %10;;
    	 lcd_t.number6_high =  lcd_t.number6_low ;//(run_t.dispTime_hours ) %10;
         
         lcd_t.number7_low = (run_t.dispTime_minutes )/10;
    	 lcd_t.number7_high = lcd_t.number7_low;//(run_t.dispTime_minutes )/10;

    	 lcd_t.number8_low = (run_t.dispTime_minutes )%10;
    	 lcd_t.number8_high = lcd_t.number8_low ;//(run_t.dispTime_minutes )%10;

     
      
    
    break;

    case setup_timer:
        



    break;


    case timer_time:
 

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

}

/**********************************************************************************************************
*
*	Function Name: void power_off_disp_fan_run_handler(void)
*	Function:
*	Input Ref: NO
*	Return Ref: NO
*
**********************************************************************************************************/
void power_off_disp_fan_run_handler(void)
{

   static uint8_t  blink_flag ;
   
	if(lcd_t.gTimer_fan_10ms >40){
         lcd_t.gTimer_fan_10ms=0;

	     blink_flag = !blink_flag;
	
	}

	  if(!blink_flag){

	    TM1723_Write_Display_Data(0xCA,(lcdNumber5_Low[0x0A]+lcdNumber6_High[0x0A]) & 0xff);
	
		   TM1723_Write_Display_Data(0xCB,TIME_NO_COLON+lcdNumber6_Low[0x0A]+lcdNumber7_High[0X0A]);
		   
           TM1723_Write_Display_Data(0xCC,(T14+lcdNumber7_Low[0x0A]+lcdNumber8_High[0x0A]) & 0xff);
          
           TM1723_Write_Display_Data(0xCE,lcdNumber8_Low[0x0A]+WIND_SPEED_FULL);//display "t,c"
           TM1723_Write_Display_Data(0xCF,((T16+T12+T10)& 0x0B));//
            TIM1723_Write_Cmd(LUM_VALUE);
                    
         }
	    else{
                
     

         TM1723_Write_Display_Data(0xCA,(T15+lcdNumber5_Low[0x0A]+lcdNumber6_High[0x0A]) & 0xff);
     
		TM1723_Write_Display_Data(0xCB,(TIME_NO_COLON+lcdNumber6_Low[0x0A]+lcdNumber7_High[0x0A]) & 0xff);
		 
         TM1723_Write_Display_Data(0xCC,(lcdNumber7_Low[0x0A]+lcdNumber8_High[0x0A]) & 0xff);
         TM1723_Write_Display_Data(0xCE,T13+lcdNumber8_Low[0x0A]+WIND_SPEED_FULL);//display "close"
        
         TM1723_Write_Display_Data(0xCF,((T11+T16) & 0x05));//
          TIM1723_Write_Cmd(LUM_VALUE);
                  
        }
        

}
/******************************************************/
//optimaze codes
static void set_lcd_numbers_from_value(uint8_t value) {
    lcd_t.number1_low  = value / 10;
    lcd_t.number1_high = lcd_t.number1_low;
    lcd_t.number2_low  = value % 10;
    lcd_t.number2_high = lcd_t.number2_low;
}

static uint8_t get_dry_plasma_ultra_mask(void) 
{
    uint8_t mask = 0x01;
    if (run_t.dry)        mask += DRY_Symbol;
    if (run_t.plasma)     mask += KILL_Symbol;
    if (run_t.ultrasonic) mask += BUG_Symbol;
    return mask;
}


/**
*@ brief :
*@ parame:
*@ retval:
**/
void display_ai_icon(uint8_t data) 
{
	static uint8_t ai_symbol;
	ai_symbol = data ? AI_Symbol : AI_NO_Symbol;
    TM1723_Write_Display_Data(0xC3, (ai_symbol+lcdNumber1_Low[lcd_t.number1_low] +lcdNumber2_High[lcd_t.number2_high]));
}
/**
*@ brief :
*@ parame:
*@ retval:
**/

/**
*@ brief :
*@ parame:
*@ retval:
**/
static void display_wifi_icon(void) 
{
    uint8_t wifi_symbol;
    wifi_symbol = (display_wifi_icon_flag == 0) ? WIFI_NO_Symbol : WIFI_Symbol;
    TM1723_Write_Display_Data(0xC5, (wifi_symbol + lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff);
}
/**
*	@brief  
*   @note
*   @param
*   @return 
*/
void disp_temp_humidity_wifi_icon_handler(void) 
{
    static uint8_t number_blink_times;
	TIM1723_Write_Cmd(0x00);
    TIM1723_Write_Cmd(0x40);
    TIM1723_Write_Cmd(0x44);

    switch(gpro_t.temp_key_set_value){

	  case 1:
         if (gpro_t.gTimer_set_temp_times < 2){
            set_lcd_numbers_from_value(run_t.wifi_set_temperature);
          
             Display_Kill_Dry_Ster_Icon();
            
		 
        } 
        else{
          gpro_t.set_temp_value_success=1;
          gpro_t.temp_key_set_value =0;
           gpro_t.gTimer_temp_compare_value =10; //at once run compare value fun WT.EDIT 2025.10.31
		 
	
		  //if( run_t.smart_phone_set_temp_value_flag ==0){ //WT.EIDT 2025.10.31
		    SendData_Data(0x2A,run_t.wifi_set_temperature);
            vTaskDelay(pdMS_TO_TICKS(20));
		  ///}
		   set_temperature_compare_value_fun();
		 
        }

 

	break;

	case 0:
	     
	     display_temperture_humidity_value();

		 
    
     break;
    }

    wifi_icon_blink_reg0xc5_handler();
    TIM1723_Write_Cmd(LUM_VALUE);
}

