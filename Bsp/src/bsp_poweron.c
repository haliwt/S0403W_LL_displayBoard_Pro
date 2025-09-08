#include "bsp.h"

static void display_lcd_Icon_init(void);

void power_on_ref_init(void)
{

   if(gpro_t.power_on_every_times==1){
    
      gpro_t.power_on_every_times ++ ;
      run_t.disp_wind_speed_grade =100;//WT.EDIT 2025.04.16
      run_t.wifi_set_temperature=40; //WT.EDIT 2025.04.16
      display_lcd_Icon_init();
      power_on_display_temp_handler();//WT.EDIT 2025.03.28


    }
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
  
   
   if(gpro_t.smart_phone_app_timer_power_on_flag == 0){

    	run_t.gModel =1; //WT.EDIT 2022.09.01
    	run_t.plasma=1;
    	run_t.dry =1;
    	run_t.ultrasonic =1;
    
    }

	gpro_t.set_temp_value_success =0;
    run_t.smart_phone_set_temp_value_flag=0;

    
    
    run_t.disp_wind_speed_grade =100;

    
	

   run_t.display_set_timer_or_works_mode =works_time;

	
     run_t.gTimer_timing=0;

	 run_t.timer_time_hours =0;
	 run_t.timer_time_minutes =0;
	 

	 run_t.timer_timing_define_flag = timing_not_definition;

	 

	 if(lcd_t.display_beijing_time_flag == 0 ){

	 run_t.gTimer_disp_time_seconds=0;
	 run_t.dispTime_hours=0;
     run_t.dispTime_minutes=0;
	 
	  lcd_t.number5_low=(run_t.dispTime_hours ) /10;
     lcd_t.number5_high =(run_t.dispTime_hours) /10;

	 lcd_t.number6_low = (run_t.dispTime_hours ) %10;;
	 lcd_t.number6_high = (run_t.dispTime_hours ) %10;
     
     lcd_t.number7_low = (run_t.dispTime_minutes )/10;
	 lcd_t.number7_high = (run_t.dispTime_minutes )/10;

	 lcd_t.number8_low = (run_t.dispTime_minutes )%10;
	 lcd_t.number8_high = (run_t.dispTime_minutes )%10;
	
	 
	 }
    
	run_t.power_off_id_flag =1;
 
   

    run_t.gModel =1; //AI mode
    run_t.display_set_timer_or_works_mode=works_time ;
  
  
     Lcd_PowerOn_Fun();
   //  freertos_stop_timer1_handler();
    
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
       // run_t.power_on= power_on;
        gpro_t.long_key_power_counter =0; 
        run_t.power_on_disp_smg_number = 1;
        gpro_t.send_ack_cmd = ack_power_on;
        gpro_t.gTimer_again_send_power_on_off =0;
        run_t.wifi_set_temperature=40; //WT.EDIT 2025.01.15
        run_t.display_set_timer_or_works_mode = works_time;//WT.EDIT 2025.01.15
        run_t.smart_phone_set_temp_value_flag =0;//WT.EDIT 2025.01.15
        gpro_t.set_temp_value_success=0;//WT.EDIT 2025.01.15
       
        SendData_PowerOnOff(1);
         gpro_t.power_on_every_times=1;

        gpro_t.gTimer_mode_key_long=0;
        osDelay(3);//HAL_Delay(10);
      

    }
    else{ //power off .
        gpro_t.long_key_power_counter=0; //WT.2024.11.05
        gpro_t.key_long_power_flag=0;
        gpro_t.send_ack_cmd = ack_power_off;
        gpro_t.gTimer_again_send_power_on_off =0;
        run_t.wifi_set_temperature =40;//WT.EDIT 2025.01.15
        run_t.smart_phone_set_temp_value_flag =0;//WT.EDIT 2025.01.15
        gpro_t.set_temp_value_success=0;//WT.EDIT 2025.01.15
        SendData_PowerOnOff(0);
        osDelay(3);//HAL_Delay(10);
        /* run_t.power_on= power_off; */


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


