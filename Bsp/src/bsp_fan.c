#include "bsp.h"




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
        
//               lcd_t.number5_low=(run_t.timer_time_hours ) /10;
//               lcd_t.number5_high =lcd_t.number5_low;//(run_t.timer_time_hours) /10;
//    
//               lcd_t.number6_low = (run_t.timer_time_hours ) %10;;
//               lcd_t.number6_high =   lcd_t.number6_low;//(run_t.timer_time_hours ) %10;
//    
//               lcd_t.number7_low =0;
//               lcd_t.number7_high = 0;
//    
//               lcd_t.number8_low = 0;
//               lcd_t.number8_high = 0;


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

/**************************************************************************
 * 
 *Function Name:void power_on_off_handler(void)
 *Function:
 *Input Ref:
 *Return Ref:
 * 
*****************************************************************************/
void display_lcd_Icon_init(void)
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
	 
     TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[lcd_t.number1_low])+lcdNumber2_High[lcd_t.number2_high]) & 0xff);//display  "AI icon
     TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[lcd_t.number2_low]+lcdNumber3_High[lcd_t.number3_high])&0xF1);//display "t,c"
     TM1723_Write_Display_Data(0xC5,(WIFI_Symbol+lcdNumber3_Low[lcd_t.number3_low] + lcdNumber4_High[lcd_t.number4_high]) & 0xff); //Wifi

      disp_fan_leaf_init();



     TIM1723_Write_Cmd(LUM_VALUE);
}


/*************************************************************************************
    *
    *Function Name:void disp_fan_leaf_init(void)
    *Function:
    *Input Ref:NO
    *Return Ref:NO
    *
**************************************************************************************/
void disp_fan_leaf_init(void)
{
    //  static uint8_t colon_flag_toggle,
	   static uint8_t fan_blink_full;

          fan_blink_full++;
      
         lcd_t.number5_low=(run_t.dispTime_hours ) /10;
         lcd_t.number5_high =lcd_t.number5_low;//(run_t.dispTime_hours) /10;

    	 lcd_t.number6_low = (run_t.dispTime_hours ) %10;;
    	 lcd_t.number6_high =  lcd_t.number6_low ;//(run_t.dispTime_hours ) %10;
         
         lcd_t.number7_low = (run_t.dispTime_minutes )/10;
    	 lcd_t.number7_high = lcd_t.number7_low;//(run_t.dispTime_minutes )/10;

    	 lcd_t.number8_low = (run_t.dispTime_minutes )%10;
    	 lcd_t.number8_high = lcd_t.number8_low ;//(run_t.dispTime_minutes )%10;
    	 
           TM1723_Write_Display_Data(0xC9,(T8_HUM+lcdNumber4_Low[lcd_t.number4_low]+lcdNumber5_High[lcd_t.number5_high]) & 0xff);
    
           if(fan_blink_full == 1){
               TM1723_Write_Display_Data(0xCA,T15+lcdNumber5_Low[lcd_t.number5_low]+lcdNumber6_High[lcd_t.number6_high]);//display digital '5,6'
               if(gpro_t.disp_time_colon_flag ==1){
                        
                TM1723_Write_Display_Data(0xCB,TIME_COLON+lcdNumber6_Low[lcd_t.number6_low]+lcdNumber7_High[lcd_t.number7_high]);//d
               }
               else{
               
                 TM1723_Write_Display_Data(0xCB,TIME_NO_COLON+lcdNumber6_Low[lcd_t.number6_low]+lcdNumber7_High[lcd_t.number7_high]);//d
    
    
               }
                //fan speed full.
               TM1723_Write_Display_Data(0xCC,(T14+lcdNumber7_Low[lcd_t.number7_low]+lcdNumber8_High[lcd_t.number8_high]) & 0xff);
               TM1723_Write_Display_Data(0xCE,lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_FULL);//display "t,c"
               TM1723_Write_Display_Data(0xCF,((T16+T12+T10)& 0x0B));//
    
          }
          else{
    
           fan_blink_full =0;
    
    
           TM1723_Write_Display_Data(0xCA,lcdNumber5_Low[lcd_t.number5_low]+lcdNumber6_High[lcd_t.number6_high]);//display digit
           if(gpro_t.disp_time_colon_flag ==1){
    
               TM1723_Write_Display_Data(0xCB,TIME_COLON+lcdNumber6_Low[lcd_t.number6_low]+lcdNumber7_High[lcd_t.number7_high]);//d
           }
           else{
    
               TM1723_Write_Display_Data(0xCB,TIME_NO_COLON+lcdNumber6_Low[lcd_t.number6_low]+lcdNumber7_High[lcd_t.number7_high]);//d
           }
    
           //fan speed full.
               TM1723_Write_Display_Data(0xCC,(T14+lcdNumber7_Low[lcd_t.number7_low]+lcdNumber8_High[lcd_t.number8_high]) & 0xff);
               TM1723_Write_Display_Data(0xCE,lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_FULL);//display "t,c"
               TM1723_Write_Display_Data(0xCF,((T16+T12+T10)& 0x0B));//
           }
     //}
      



}



