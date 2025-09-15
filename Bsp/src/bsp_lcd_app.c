#include "bsp.h"





static void donot_disp_T13_icon_fan_speed_level(void);

//static void disp_fan_ptc_warning_time_colon_fun(void);

static void works_timer_disp_numaber(void);
static void display_temperture_humidity_value(void);

static void wifi_icon_blink_reg0xc5_handler(void);

static void display_lowbit_lunmber4_reg0xc9_handler(void);



static void display_temp_unit(uint8_t and_mask) ;

//
uint8_t display_wifi_icon_flag ;
uint8_t disp_set_timer_value;


/*************************************************************************
 	*
 	* Function Name:void disp_temp_humidity_value(void)
 	* Function :lcd display panel 
 	* Input Ref:NO
 	* Return Ref:NO
 	* 
*************************************************************************/ 
#if 0
void disp_temp_humidity_wifi_icon_handler(void)
{
    
     static uint8_t decade_temp,unit_temp,temp1,temp2;

     static uint8_t number_blink_times;// disp_set_timer_value;
     static ai_base;
	 
	
	TIM1723_Write_Cmd(0x00); //WT.EDIT 2025.03.28
	TIM1723_Write_Cmd(0x40);
	TIM1723_Write_Cmd(0x44);

    
   /***********************setup temperature value ********************************/
	 //digital 1,2 ->display "temperature"  blink  
	if(run_t.smart_phone_set_temp_value_flag ==1){

       // disp_set_timer_value =1;

        decade_temp = run_t.wifi_set_temperature / 10 ;
		unit_temp =  run_t.wifi_set_temperature % 10; //
        
		lcd_t.number1_low=decade_temp;
		lcd_t.number1_high =decade_temp;

		lcd_t.number2_low = unit_temp;
		lcd_t.number2_high = unit_temp;

	     
	 if(run_t.gTimer_numbers_one_two_blink < 6  ){ //disp number setup temperature number 1,2.
	     //display address 0xC2
	     if(run_t.dry ==1 && run_t.plasma ==1  && run_t.ultrasonic==1)
		 	TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0xff);//display digital "temp
         else if(run_t.dry ==0 && run_t.plasma ==1 && run_t.ultrasonic==1)
		 	TM1723_Write_Display_Data(0xC2,((0X01+KILL_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0xff);//display digital "temp
         else if(run_t.dry ==0 && run_t.plasma ==0 && run_t.ultrasonic==1)
		 	TM1723_Write_Display_Data(0xC2,(0X01+BUG_Symbol+lcdNumber1_High[lcd_t.number1_high]) & 0xff);//display digital "temp
		 else if(run_t.dry ==0 && run_t.plasma ==0 && run_t.ultrasonic==0){
		 	TM1723_Write_Display_Data(0xC2,(0X01+lcdNumber1_High[lcd_t.number1_high]) & 0xff);//display digital "temp
		 }
		 else if(run_t.dry ==1 && run_t.plasma ==1 && run_t.ultrasonic==0){
		 	TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0xff);//display digital "temp
		 }
		 else if(run_t.dry ==0 && run_t.plasma ==1 && run_t.ultrasonic==0){
		 	TM1723_Write_Display_Data(0xC2,((0X01+KILL_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0xff);//display digital "temp
		 }
		 else if(run_t.dry ==1 && run_t.plasma ==0 && run_t.ultrasonic==0){
		 	TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0xff);//display digital "temp
		 }
		 else if(run_t.dry ==1 && run_t.plasma ==0 && run_t.ultrasonic==1){
		 	TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0xff);//display digital "temp
		 }
		 //display addres 0xC3 -> AI icon
		 if(run_t.gModel ==1)
	        TM1723_Write_Display_Data(0xC3,(lcdNumber1_Low[lcd_t.number1_low]+AI_Symbol+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
         else
		 	TM1723_Write_Display_Data(0xC3,(lcdNumber1_Low[lcd_t.number1_low]+AI_NO_Symbol+lcdNumber2_High[lcd_t.number2_high]) & 0xfe);//display  "AI icon

		 //display address 0xC4 -> temperature icon T7
		 TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xff);//display "t,c"
		 
		 
	 	 }//don't Display numbers
		 else if(run_t.gTimer_numbers_one_two_blink > 5  && run_t.gTimer_numbers_one_two_blink <11){ //don't display 
            //display address 0xC2 -> 
	          if(run_t.dry ==1 && run_t.plasma ==1  && run_t.ultrasonic==1)
			 	TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0x0f);//display digital "temp
	         else if(run_t.dry ==0 && run_t.plasma ==1 && run_t.ultrasonic==1)
			 	TM1723_Write_Display_Data(0xC2,((0X01+KILL_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0x0f);//display digital "temp
	         else if(run_t.dry ==0 && run_t.plasma ==0 && run_t.ultrasonic==1)
			 	TM1723_Write_Display_Data(0xC2,((0X01+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0x0f);//display digital "temp
			 else if(run_t.dry ==0 && run_t.plasma ==0 && run_t.ultrasonic==0){
			 	TM1723_Write_Display_Data(0xC2,((0X01)+lcdNumber1_High[lcd_t.number1_high]) & 0x0f);//display digital "temp
			 }
			 else if(run_t.dry ==1 && run_t.plasma ==1 && run_t.ultrasonic==0){
			 	TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0x0f);//display digital "temp
			 }
			 else if(run_t.dry ==0 && run_t.plasma ==1 && run_t.ultrasonic==0){
			 	TM1723_Write_Display_Data(0xC2,((0X01+KILL_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0x0f);//display digital "temp
			 }
			 else if(run_t.dry ==1 && run_t.plasma ==0 && run_t.ultrasonic==0){
			 	TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0x0f);//display digital "temp
			 }
			 else if(run_t.dry ==1 && run_t.plasma ==0 && run_t.ultrasonic==1){
			 	TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0x0f);//display digital "temp
			 }
			 
				//TM1723_Write_Display_Data(0xC2,(((0X01+DRY_Symbol+KILL_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high]) & 0x0F));
	           // display address 0xC3
	           if(run_t.gModel ==1)
			     TM1723_Write_Display_Data(0xC3,((lcdNumber1_Low[lcd_t.number1_low]+AI_Symbol+lcdNumber2_High[lcd_t.number2_high])) & 0x1);
	           else
			   	 TM1723_Write_Display_Data(0xC3,((lcdNumber1_Low[lcd_t.number1_low]+AI_NO_Symbol+lcdNumber2_High[lcd_t.number2_high])) & 0x0);
	           //display address 0xC4
				TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xF1);//display "t,c"
        }
		else {
             run_t.gTimer_numbers_one_two_blink =0;
			 number_blink_times++;
		     if(number_blink_times > 2){
          
                 number_blink_times =0;
                 disp_set_timer_value =1;
				 run_t.smart_phone_set_temp_value_flag =0;
                 gpro_t.set_temp_value_success = 1;
                 gpro_t.gTimer_temp_compare_value =20; //at once 
			}

		}
        TIM1723_Write_Cmd(LUM_VALUE);//(0x9B);
	 }
	 else{ //digital "1,2" don't blink LED


       if(gpro_t.temp_key_set_value ==1){

         decade_temp = run_t.wifi_set_temperature / 10 ;
		 unit_temp =  run_t.wifi_set_temperature % 10; //
        
		lcd_t.number1_low=decade_temp;
		lcd_t.number1_high =decade_temp;

		lcd_t.number2_low = unit_temp;
		lcd_t.number2_high = unit_temp;

		
		//display address 0xC2 -> //WT.EDTI 2025.03.28
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
               if(display_wifi_icon_flag==1){
                  TM1723_Write_Display_Data(0xC5,(WIFI_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff); //Wifi
              
               }
			   else if(display_wifi_icon_flag==0){
                  TM1723_Write_Display_Data(0xC5,(WIFI_NO_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff); //Wifi
              

			   }
			   else if(display_wifi_icon_flag==2){

			       TM1723_Write_Display_Data(0xC5,(WIFI_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff); //Wifi
							 

			   }

			  osDelay(30);
        }
        else if(disp_set_timer_value==1 ){ //WT.EDIT 2025.01.11

          disp_set_timer_value++;
          temp1 =   gpro_t.temp_real_value/ 10;
          temp2   = gpro_t.temp_real_value% 10;

           lcd_t.number1_low= temp1;
		   lcd_t.number1_high =temp1;

		   lcd_t.number2_low = temp2;
	       lcd_t.number2_high = temp2;

		   
		//display address 0xC2 ->//WT.EDTI 2025.03.28
			  Display_Kill_Dry_Ster_Icon();
		
			 //display address 0xC3--> T1,
			 ai_base = run_t.gModel? 1:0;
			  if(run_t.gModel==1){
			   TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high])& 0xff);//display	"AI icon"
			  }
			 else { 
			  TM1723_Write_Display_Data(0xC3,((lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//don't display "AI icon"
			  
			  }
			 
			 //display address 0xC4
			  TM1723_Write_Display_Data(0xC4,((0x01+lcdNumber2_Low[lcd_t.number2_low])+lcdNumber3_High[lcd_t.number3_high])&0xff);

              
			if(display_wifi_icon_flag==1){
				TM1723_Write_Display_Data(0xC5,(WIFI_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff); //Wifi

			}
			else if(display_wifi_icon_flag==0){
				TM1723_Write_Display_Data(0xC5,(WIFI_NO_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff); //Wifi


			}
			else if(display_wifi_icon_flag==2){

				TM1723_Write_Display_Data(0xC5,(WIFI_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff); //Wifi


			}

			osDelay(30);
        }
		else{
		 display_temperture_humidity_value();

        }

	}
   
	
	wifi_icon_blink_reg0xc5_handler();
	
	TIM1723_Write_Cmd(LUM_VALUE);//(0x97);//(0x94);//(0x9B);


	 

}
#endif 
/*****************************************************************
  * display wifi icon 
  
******************************************************************/
#if 0
static void wifi_icon_blink_reg0xc5_handler(void)
{
        //T2->"WIFI" icon -> 0xC5
		if(run_t.wifi_link_net_success ==0){   // && run_t.gPower_On==1){ //hasn't wifi run_t.wifi_link_net_success
			if(run_t.wifi_led_fast_blink_flag==0){
				if(lcd_t.gTimer_wifi_500ms >99 && lcd_t.gTimer_wifi_500ms<200){
					 
					  TM1723_Write_Display_Data(0xC5,(WIFI_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff); //Wifi
					   display_wifi_icon_flag=1;
				}
				else if(lcd_t.gTimer_wifi_500ms <100){
					
					TM1723_Write_Display_Data(0xC5,(WIFI_NO_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff); //Wifi 
				    display_wifi_icon_flag=0;
				}
				else{
				   lcd_t.gTimer_wifi_500ms =0;
				}
			}
			else{ //be detected of wifi signal fast blink wiif icon
	
				if(lcd_t.gTimer_wifi_500ms >14 && lcd_t.gTimer_wifi_500ms<30){ //400ms
					 
					  TM1723_Write_Display_Data(0xC5,(WIFI_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff); //Wifi
					   display_wifi_icon_flag=1; 
				}
				else if(lcd_t.gTimer_wifi_500ms <15){
					
					TM1723_Write_Display_Data(0xC5,(WIFI_NO_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff); //Wifi 
				     display_wifi_icon_flag=0;
				}
				else{
				   lcd_t.gTimer_wifi_500ms =0;
				  
				}
				if(run_t.gTimer_wifi_connect_counter > 120){ //
					 run_t.gTimer_wifi_connect_counter=0;
					 run_t.wifi_led_fast_blink_flag=0;
					 
	
				}
	
			}
		   
		}
		else if(run_t.wifi_link_net_success ==1){//wifi be connect is OK 
			  run_t.wifi_led_fast_blink_flag=0;
			  TM1723_Write_Display_Data(0xC5,(WIFI_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff); //Wifi
               display_wifi_icon_flag=2;
			   TIM1723_Write_Cmd(LUM_VALUE);//(0x97);//(0x94);//(0x9B);
		}


}
#endif 
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
        if(lcd_t.gTimer_wifi_500ms > 14){
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

   if(disp_set_timer_value!=1 && gpro_t.temp_key_set_value !=1 && gpro_t.gTimer_disp_temp_humi_value>4){

		gpro_t.gTimer_disp_temp_humi_value=0;

		//display address 0xC2 -> //WT.EDTI 2025.03.28
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

    // 显示 AI 图标（0xC3）
    TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high])& 0xff);

    // 显示温度单位（0xC4）
	TM1723_Write_Display_Data(0xC4,((0x01+lcdNumber2_Low[lcd_t.number2_low])+lcdNumber3_High[lcd_t.number3_high])&0xff);
    // WiFi 图标闪烁处理（0xC5）
    wifi_icon_blink_reg0xc5_handler();

    // 显示低位数字（0xC9）
    display_lowbit_lunmber4_reg0xc9_handler();



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

  #if 0
   //number "1,2" -> temperature

	  if(run_t.dry==1 && run_t.plasma==1 && run_t.ultrasonic==1){

	     //display address 0xc2
	     TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high])&0xff);//display digital "temp
	     
		 
	  }
	  else if(run_t.dry==0 && run_t.plasma==1 && run_t.ultrasonic==1){

	      TM1723_Write_Display_Data(0xC2,((0X01+DRY_NO_Symbol+KILL_Symbol+BUG_Symbol))+lcdNumber1_High[lcd_t.number1_high]);//display digital "temp
	    
	   }
	   else if(run_t.dry==0 && run_t.plasma==0 && run_t.ultrasonic ==1){

	        TM1723_Write_Display_Data(0xC2,((0X01+DRY_NO_Symbol+KILL_NO_Symbol+BUG_Symbol))+lcdNumber1_High[lcd_t.number1_high]);//display digit
	        
	   }
	  else if(run_t.dry==0 && run_t.plasma==0 && run_t.ultrasonic ==0){

	        TM1723_Write_Display_Data(0xC2,0X01+DRY_NO_Symbol+KILL_NO_Symbol+BUG_NO_Symbol+lcdNumber1_High[lcd_t.number1_high]);//display digit
	       
	  }
	  else if(run_t.dry==1 && run_t.plasma==0 && run_t.ultrasonic ==1){

	        TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_NO_Symbol+BUG_Symbol))+lcdNumber1_High[lcd_t.number1_high]);//display digit
	       
	  }
	  else if(run_t.dry==1 && run_t.plasma==0 && run_t.ultrasonic ==0){

	        TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_NO_Symbol+BUG_NO_Symbol))+lcdNumber1_High[lcd_t.number1_high]);//display digit
	       
	  }
	  else if(run_t.dry==0 && run_t.plasma==1 && run_t.ultrasonic ==0){
	   
			  TM1723_Write_Display_Data(0xC2,((0X01+DRY_NO_Symbol+KILL_Symbol+BUG_NO_Symbol))+lcdNumber1_High[lcd_t.number1_high]);//display digit
			  
	 }
	  else if(run_t.dry==1 && run_t.plasma==1 && run_t.ultrasonic ==0){
		 
				TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_Symbol+BUG_NO_Symbol))+lcdNumber1_High[lcd_t.number1_high]);//display digit
			
	   }

  
     TIM1723_Write_Cmd(LUM_VALUE);

	#endif
	 uint8_t value ;

	 value = 0x01
        + (run_t.dry        ? DRY_Symbol  : DRY_NO_Symbol)
        + (run_t.plasma     ? KILL_Symbol : KILL_NO_Symbol)
        + (run_t.ultrasonic ? BUG_Symbol  : BUG_NO_Symbol)
        + lcdNumber1_High[lcd_t.number1_high];

    TM1723_Write_Display_Data(0xC2, value);
    TIM1723_Write_Cmd(LUM_VALUE);
   


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
    if(lcd_t.gTimer_fan_10ms <40){

    
           TM1723_Write_Display_Data(0xCA,(lcdNumber5_Low[0x0A]+lcdNumber6_High[0x0A]) & 0xff);
		   if(gpro_t.disp_time_colon_flag ==1)
           		TM1723_Write_Display_Data(0xCB,(TIME_COLON+lcdNumber6_Low[0x0A]+lcdNumber7_High[0x0A]) & 0xff);
		   else
		   	 TM1723_Write_Display_Data(0xCB,TIME_NO_COLON+lcdNumber6_Low[0x0A]+lcdNumber7_High[0X0A]);
		   
           TM1723_Write_Display_Data(0xCC,(T14+lcdNumber7_Low[0x0A]+lcdNumber8_High[0x0A]) & 0xff);
          
           TM1723_Write_Display_Data(0xCE,lcdNumber8_Low[0x0A]+WIND_SPEED_FULL);//display "t,c"
           TM1723_Write_Display_Data(0xCF,((T16+T12+T10)& 0x0B));//
                    
         }
         else if(lcd_t.gTimer_fan_10ms >39 && lcd_t.gTimer_fan_10ms<80){ //close
                
     

         TM1723_Write_Display_Data(0xCA,(T15+lcdNumber5_Low[0x0A]+lcdNumber6_High[0x0A]) & 0xff);
         if(gpro_t.disp_time_colon_flag ==1)
            TM1723_Write_Display_Data(0xCB,(TIME_COLON+lcdNumber6_Low[0x0A]+lcdNumber7_High[0x0A]) & 0xff);
		 else 
		 	 TM1723_Write_Display_Data(0xCB,(TIME_COLON+lcdNumber6_Low[0x0A]+lcdNumber7_High[0x0A]) & 0xff);
		 
         TM1723_Write_Display_Data(0xCC,(lcdNumber7_Low[0x0A]+lcdNumber8_High[0x0A]) & 0xff);
         TM1723_Write_Display_Data(0xCE,T13+lcdNumber8_Low[0x0A]+WIND_SPEED_FULL);//display "close"
        
         TM1723_Write_Display_Data(0xCF,((T11+T16) & 0x05));//
                  
    
         }
         else if(lcd_t.gTimer_fan_10ms > 79){
                lcd_t.gTimer_fan_10ms=0;
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

static uint8_t get_dry_plasma_ultra_mask(void) {
    uint8_t mask = 0x01;
    if (run_t.dry)        mask += DRY_Symbol;
    if (run_t.plasma)     mask += KILL_Symbol;
    if (run_t.ultrasonic) mask += BUG_Symbol;
    return mask;
}

static void display_icons_0xC2(uint8_t and_mask) 
{
    TM1723_Write_Display_Data(0xC2, (get_dry_plasma_ultra_mask() + lcdNumber1_High[lcd_t.number1_high]) & and_mask);
}

void display_ai_icon(uint8_t data) 
{
	uint8_t ai_symbol;
	ai_symbol = data ? AI_Symbol : AI_NO_Symbol;
    TM1723_Write_Display_Data(0xC3, (lcdNumber1_Low[lcd_t.number1_low] + ai_symbol + lcdNumber2_High[lcd_t.number2_high]));
}

static void display_temp_unit(uint8_t and_mask) {
    TM1723_Write_Display_Data(0xC4, (0x01 + lcdNumber2_Low[lcd_t.number2_low] + lcdNumber3_High[lcd_t.number3_high]) & and_mask);
}

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
    static uint8_t number_blink_times;// disp_set_timer_value;
	TIM1723_Write_Cmd(0x00);
    TIM1723_Write_Cmd(0x40);
    TIM1723_Write_Cmd(0x44);

    if (run_t.smart_phone_set_temp_value_flag) {
        set_lcd_numbers_from_value(run_t.wifi_set_temperature);

        if (run_t.gTimer_numbers_one_two_blink < 6) {
            display_icons_0xC2(0xff);
            display_ai_icon(run_t.gModel);
            display_temp_unit(0xff);
        } else if (run_t.gTimer_numbers_one_two_blink < 11) {
            display_icons_0xC2(0x0f);
            display_ai_icon(run_t.gModel);
            display_temp_unit(0xF1);
        } else {
            run_t.gTimer_numbers_one_two_blink = 0;
            if (++number_blink_times > 2) {
                number_blink_times = 0;
                disp_set_timer_value = 1;
                run_t.smart_phone_set_temp_value_flag = 0;
                gpro_t.set_temp_value_success = 1;
                gpro_t.gTimer_temp_compare_value = 20;
            }
        }
        TIM1723_Write_Cmd(LUM_VALUE);
    } 
	else {
        if (gpro_t.temp_key_set_value) {
            set_lcd_numbers_from_value(run_t.wifi_set_temperature);
            Display_Kill_Dry_Ster_Icon();
            display_ai_icon(run_t.gModel);
            display_temp_unit(0xff);
            display_wifi_icon();
            osDelay(30);
        } 
		else if (disp_set_timer_value == 1) {
            disp_set_timer_value++;
            set_lcd_numbers_from_value(gpro_t.temp_real_value);
            Display_Kill_Dry_Ster_Icon();
            display_ai_icon(run_t.gModel);
            display_temp_unit(0xff);
            display_wifi_icon();
            osDelay(30);
        } 
		else {
            display_temperture_humidity_value();
        }
    }

    wifi_icon_blink_reg0xc5_handler();
    TIM1723_Write_Cmd(LUM_VALUE);
}

