#include "bsp.h"

uint8_t power_on_step;

uint8_t counter_flag ;



static void power_off_breath_Led(void);


static void power_on_ref_init(void);


static void display_lcd_Icon_init(void);

/*
	*@brief :
	*@param:
	*@retval:
*/
void power_on_handler(void)
{
   
   switch(power_on_step){

      case 0:
          power_on_ref_init();
	      gpro_t.temp_key_set_value =0;
          power_on_step =1;
          gpro_t.gTimer_two_hours_second_counter=0;
		  gpro_t.gTimer_two_hours_conter=0;
      break;

	  case 1:

	    disp_temp_humidity_wifi_icon_handler();
	    power_on_step =2;

	  break;

	  case 2:
	  	display_timer_and_beijing_time_handler();
        power_on_step =3;
	  break;

	  case 3:
	  	#if 0
	  	  if(gpro_t.gTimer_two_hours_conter > 119){ //WT.EDIT2025.10.30
		   	   gpro_t.gTimer_two_hours_conter=0;
               gpro_t.stopTwoHours_flag = 1; //WT.EDIT 2025.11.10
		       gpro_t.send_ack_cmd = 0x1C;
			   gpro_t.two_hours_cp_flag= 0xC1;
			    gpro_t.ack_cp_repeat_counter=0;
			   gpro_t.gTimer_cp_timer_counter =0;
		       SendData_twoHours_Data(0x78);//120 minutes
               vTaskDelay(pdMS_TO_TICKS(5));

		   }
		   else if(gpro_t.stopTwoHours_flag == 1 && gpro_t.gTimer_two_hours_conter > 10){
		        gpro_t.gTimer_two_hours_conter=0;
				gpro_t.send_ack_cmd = 0x1C;//WT.EDIT 2025.11.10
				gpro_t.two_hours_cp_flag=0xC0;
				 gpro_t.ack_cp_repeat_counter=0;
			    gpro_t.gTimer_cp_timer_counter =0;
				gpro_t.stopTwoHours_flag = 0;
				SendData_twoHours_Data(0x0A);//10 minutes
                vTaskDelay(pdMS_TO_TICKS(5));

          }
		 #endif 
	     power_on_step =4;

	  break;

	  case 4:
        if(gpro_t.gTimer_soft_version_counter > 5){
			gpro_t.gTimer_soft_version_counter=0;
	   		 sendNotice_toMainBoard(0xF0,0x01); //WT.EDIT 2025.10.31 new version : 0x01 
			vTaskDelay(pdMS_TO_TICKS(5));

        }
	  
	  power_on_step =5;


	  break;

	 case 5:
     if(gpro_t.temp_key_set_value==0 && gpro_t.gTimer_temp_compare_value > 3 ){
	 	gpro_t.gTimer_temp_compare_value =0;


	   set_temperature_compare_value_fun();
	   if(LL_USART_IsActiveFlag_ORE(USART1)){
          LL_USART_ClearFlag_ORE(USART1);
       }

     }

	 power_on_step =1;

	 break;


   }


}

static void power_on_ref_init(void)
{

  
     if(gpro_t.smart_phone_app_timer_power_on_flag == 0){

    	
    	run_t.plasma=1;
    	run_t.dry =1;
    	run_t.ultrasonic =1;
    
    }
	 run_t.gModel =1; //WT.EDIT 2022.09.01
     run_t.gTimer_timing=0;

	 run_t.timer_time_hours =0;
	 run_t.timer_time_minutes =0;
	 run_t.power_off_id_flag=1;
      gpro_t.temp_key_set_value =0;
	  gpro_t.power_on_every_times=1 ;
      run_t.disp_wind_speed_grade =100;//WT.EDIT 2025.04.16
      run_t.wifi_set_temperature=40; //WT.EDIT 2025.04.16
      if(run_t.wifi_link_net_success ==0){

		 run_t.gTimer_disp_time_seconds=0;
		 run_t.dispTime_hours=0;
	     run_t.dispTime_minutes=0;

      }
	  else if(run_t.wifi_link_net_success ==1 && lcd_t.display_beijing_time_flag==0){
	          run_t.gTimer_disp_time_seconds=0;
			  run_t.dispTime_hours=0;
			  run_t.dispTime_minutes=0;

      }

      gpro_t.stopTwoHours_flag=0;
	
      LCD_BACK_LIGHT_ON();
	  POWER_ON_LED() ;
	  LED_MODEL_ON() ;
      display_lcd_Icon_init();
      power_on_display_temp_handler();//WT.EDIT 2025.03.28


    
}
/**************************************************************************
 * 
 *Function Name:void power_on_key_short_fun(void)
 *Function:
 *Input Ref:
 *Return Ref:
 * 
*****************************************************************************/
void power_on_key_short_fun(void)
{
  
    power_on_step=0;
	gpro_t.set_temp_value_success =0;
    run_t.smart_phone_set_temp_value_flag=0;

    run_t.disp_wind_speed_grade =100;

    run_t.display_set_timer_or_works_time_mode =works_time;

	 run_t.timer_timing_define_flag = timing_not_definition;

	 run_t.power_off_id_flag =1;
     run_t.display_set_timer_or_works_time_mode=works_time ;
     gpro_t.gTimer_two_hours_conter=0;
  
     Lcd_PowerOn_Fun();
   
    
}


/**************************************************************************
 * 
 *Function Name:void power_key_long_fun(void)
 *Function:
 *Input Ref:
 *Return Ref:
 * 
*****************************************************************************/
void power_key_long_fun(void)
{
    
    run_t.wifi_link_net_success =0;
    run_t.gTimer_wifi_connect_counter=0;
  
    run_t.wifi_receive_led_fast_led_flag=0; //adjust if mainboard receive of connect wifi of signal
    run_t.wifi_led_fast_blink_flag=1;
  
}

/**************************************************************************
 * 
 *Function Name:void power_on_off_handler(void)
 *Function:
 *Input Ref:
 *Return Ref:
 * 
*****************************************************************************/
void power_on_off_handler(void)
{
    if(run_t.power_on== power_off){
        run_t.power_on= power_on;
        SendData_PowerOnOff(1);
		osDelay(5);
		 
                
		power_on_step =0;
       // gpro_t.long_key_power_counter =0; 
        run_t.power_on_disp_smg_number = 1;
	   //copy 
        gpro_t.send_ack_cmd = ack_power_on;
	    gpro_t.ack_cp_cmd_flag= 0x11;
		 gpro_t.ack_cp_repeat_counter=0;
        gpro_t.gTimer_cp_timer_counter =0;
		//end
        run_t.wifi_set_temperature=40; //WT.EDIT 2025.01.15
        run_t.display_set_timer_or_works_time_mode = works_time;//WT.EDIT 2025.01.15
        run_t.smart_phone_set_temp_value_flag =0;//WT.EDIT 2025.01.15
        gpro_t.set_temp_value_success=0;//WT.EDIT 2025.01.15
       
       
     gpro_t.gTimer_mode_key_long=0;
      LCD_BACK_LIGHT_ON();
	  POWER_ON_LED() ;
	  LED_MODEL_ON() ;
      display_lcd_Icon_init();
      power_on_display_temp_handler();//WT.EDIT 2025.03.28
        //printf("power on !!!\r\n");
      

    }
    else if(run_t.power_on== power_on){ //power off .
        run_t.power_on= power_off;
        SendData_PowerOnOff(0);
		osDelay(5);
        //cp
        gpro_t.send_ack_cmd = ack_power_off;
		gpro_t.ack_cp_cmd_flag =0x10;
		 gpro_t.ack_cp_repeat_counter=0;
        gpro_t.gTimer_cp_timer_counter =0;
		//cp_end 
        run_t.wifi_set_temperature =40;//WT.EDIT 2025.01.15
        run_t.smart_phone_set_temp_value_flag =0;//WT.EDIT 2025.01.15
        gpro_t.set_temp_value_success=0;//WT.EDIT 2025.01.15
        Lcd_PowerOff_Fun();
   


    }
    
}


/**************************************************************************
 * 
 *Function Name:void power_on_off_handler(void)
 *Function:
 *Input Ref:
 *Return Ref:
 * 
*****************************************************************************/
static void display_lcd_Icon_init(void)
{
     static uint8_t temp1,temp2;
     static uint8_t hum1,hum2;

     TIM1723_Write_Cmd(0x00);
	 TIM1723_Write_Cmd(0x40);
	 TIM1723_Write_Cmd(0x44);

 
         
    run_t.smart_phone_set_temp_value_flag =0;//WT.EDIT 2025.01.15
    gpro_t.set_temp_value_success = 0;//WT.EDIT 2025.01.15
    gpro_t.temp_key_set_value =0;//WT.EDIT 2025.01.15

    temp1 =   gpro_t.temp_real_value/ 10;//WT.EDIT 2025.01.15
    temp2   = gpro_t.temp_real_value% 10;//WT.EDIT 2025.01.15

    hum1 = gpro_t.humidity_real_value /10;
    hum2 = gpro_t.humidity_real_value %10;

    lcd_t.number1_low= temp1;
    lcd_t.number1_high =temp1;

    lcd_t.number2_low = temp2;
    lcd_t.number2_high = temp2;



    lcd_t.number3_low= hum1;
    lcd_t.number3_high =hum1;

    lcd_t.number4_low = hum2;
    lcd_t.number4_high = hum2;


    
     if(gpro_t.smart_phone_app_timer_power_on_flag == 0){

         TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_Symbol+BUG_Symbol)+lcdNumber1_High[lcd_t.number1_high])&0xff);//display digital "temp
     }
     else{
         Display_Kill_Dry_Ster_Icon();
     }
     TM1723_Write_Display_Data(0xC3,(lcdNumber1_Low[lcd_t.number1_low]+AI_Symbol+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
     TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xF1);//display "t,c"
     TM1723_Write_Display_Data(0xC5,(WIFI_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff); //Wifi

      disp_fan_leaf_init();



     TIM1723_Write_Cmd(LUM_VALUE);
}

/****************************************************************/

void power_off_handler(void)
{
    
    if(run_t.power_off_id_flag == 1){   
        run_t.power_off_id_flag++; 
		power_on_step=0;
        lcd_donot_disp_screen();
        Power_Off_Fun();
    

        LED_MODEL_OFF();
        POWER_ON_LED();

        run_t.wifi_led_fast_blink_flag=0;
        run_t.smart_phone_set_temp_value_flag=0;
        run_t.timer_time_hours =0;
        run_t.timer_time_minutes =0;

        run_t.fan_warning=0;
        run_t.ptc_warning = 0;

        run_t.gModel =0; //WT.EDIT 2022.09.01
        run_t.plasma=0;
        run_t.dry =0;
        run_t.ultrasonic =0;


        run_t.timer_timing_define_flag = timing_not_definition;

        run_t.disp_wind_speed_grade =100;	

        gpro_t.smart_phone_app_timer_power_on_flag =0;
		run_t.gFan_RunContinue =1;
	     gpro_t.gTimer_temp_compare_value=0;
		if(lcd_t.display_beijing_time_flag == 0 ){

		 run_t.gTimer_disp_time_seconds=0;
		 run_t.dispTime_hours=0;
	     run_t.dispTime_minutes=0;

        }

      
	}
    
	
   
    power_off_breath_Led();

	
	if(run_t.gFan_RunContinue == 1 && gpro_t.gTimer_temp_compare_value < 61){
       
	      LCD_BACK_LIGHT_ON();
	      LCD_Display_Wind_Icon_Handler();
     }
	 else if(gpro_t.gTimer_temp_compare_value  > 59){
           gpro_t.gTimer_temp_compare_value=0;
           run_t.gFan_RunContinue =0;
		   Lcd_PowerOff_Fun();
		   if(lcd_t.display_beijing_time_flag == 0 ){

			 run_t.gTimer_disp_time_seconds=0;
			 run_t.dispTime_hours=0;
		     run_t.dispTime_minutes=0;
            }

	}
}



/************************************************************************
	*
	*Function Name: static void Power_Off_Fun(void)
	*
	*
	*
	*
************************************************************************/
 void Power_Off_Fun(void)
{
	
        run_t.gModel =0; //WT.EDIT 2022.09.01
		run_t.plasma=0;
		run_t.dry =0;
		run_t.ultrasonic =0;

		//run_t.gPower_On=power_off;
		
		run_t.wifi_led_fast_blink_flag=0;
		run_t.timer_timing_define_flag = timing_not_definition;
		
		run_t.disp_wind_speed_grade =100;	
		
		
} 

/************************************************************************
	*
	*Function Name: static void power_off_breath_Led(void)
	*
	*
	*
	*
************************************************************************/
static void power_off_breath_Led(void)
 {

	if(gpro_t.gTimer_disp_temp_humi_value> 1){
	    gpro_t.gTimer_disp_temp_humi_value=0;
	    POWER_LED_TOGGLE();
	  
	}
	      
}



