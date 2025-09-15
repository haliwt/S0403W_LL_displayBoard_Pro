#include "bsp.h"


// ====== 配置区 ======
#define ADDR_DIGIT45  0xC9
#define ADDR_DIGIT56  0xCA
#define ADDR_DIGIT67  0xCB   //time colon
#define ADDR_DIGIT78  0xCC
#define ADDR_WIND     0xCE
#define ADDR_MISC     0xCF

#define T8_HUM        0x01

#define  LEAF_TOGGLE_THRESHOLD    3



volatile bool blink_on = true;       // 闪烁状态
volatile uint16_t blink_counter = 0; // 闪烁计数

static void works_time_fun(void);
static void disp_fan_speed_level(void);
static void donot_disp_T13_icon_fan_speed_level(void);
static void fan_set_timer_disp_leaf(uint8_t disp);




// ====== 定时器中断（10ms周期） ======
//if (run_t.gTimer_digital5678_ms>= 4) { // 50*10ms = 500ms
//		 run_t.gTimer_digital5678_ms=0;
//		   blink_on = !blink_on;
//	}



/**
* @brief 
* @note // ====== 风速显示 ======
* @param
* @return
*/
static void update_wind_speed_display(uint8_t mask) 
{
    uint8_t base = lcdNumber8_Low[lcd_t.number8_low];
    if (run_t.disp_wind_speed_grade > 66) {
        TM1723_Write_Display_Data(ADDR_WIND, (T13 + base + WIND_SPEED_FULL) & mask);
    } else if (run_t.disp_wind_speed_grade > 33) {
        TM1723_Write_Display_Data(ADDR_WIND, (T13 + base + WIND_SPEED_TWO) & mask);
    } else {
        TM1723_Write_Display_Data(ADDR_WIND, (T13 + base + WIND_SPEED_ONE) & mask);
    }
}

/**
* @brief 
* @note // ====== 数码管显示 ======
* @param
* @return
*/
#if 0
static void display_digits(uint8_t mask, bool blink) 
{
    uint8_t colon;
	
	TM1723_Write_Display_Data(ADDR_DIGIT45, (0x01 + lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & mask);
    TM1723_Write_Display_Data(ADDR_DIGIT56, (T15 + lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high]) & mask);

    colon = gpro_t.disp_time_colon_flag ? TIME_COLON : TIME_NO_COLON;
    TM1723_Write_Display_Data(ADDR_DIGIT67, (colon + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]) & mask);

    TM1723_Write_Display_Data(ADDR_DIGIT78, (lcdNumber7_Low[lcd_t.number7_low] + lcdNumber8_High[lcd_t.number8_high]) & mask);

   // update_wind_speed_display(mask);

    TM1723_Write_Display_Data(ADDR_MISC, blink ? ((T16 + T11) & mask) : ((T16 + T12 + T10) & mask));
}

#else 
static void display_digits(uint8_t mask, bool blink) 
{
  // uint8_t colon ;
   	uint8_t colon_base,t15_base ,t14_base,t13_base,t10_base;
   #if 0
   // DIGIT45
    if (!blink) {
        TM1723_Write_Display_Data(ADDR_DIGIT45, (T8_HUM+lcdNumber4_Low[lcd_t.number4_low])& 0x0F); // 共阴空白
    } else {
        TM1723_Write_Display_Data(ADDR_DIGIT45,
            (T8_HUM+lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & mask);
    }

    // DIGIT56
    if (!blink) {
        TM1723_Write_Display_Data(ADDR_DIGIT56, 0x0);
    } else {
        TM1723_Write_Display_Data(ADDR_DIGIT56,
            (T15 + lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high]) & mask);
    }

    // DIGIT67
     colon = gpro_t.disp_time_colon_flag ? TIME_COLON : TIME_NO_COLON;
    if (!blink) {
        TM1723_Write_Display_Data(ADDR_DIGIT67, 0x0);
    } else {
        TM1723_Write_Display_Data(ADDR_DIGIT67,
            (TIME_COLON + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]) & mask);
    }

    // DIGIT78
    if (!blink) {
        TM1723_Write_Display_Data(ADDR_DIGIT78, 0x0);
    } else {
        TM1723_Write_Display_Data(ADDR_DIGIT78,(lcdNumber7_Low[lcd_t.number7_low] + lcdNumber8_High[lcd_t.number8_high]) & mask);
    }

	//DIGIT8T
	if (!blink) {
        TM1723_Write_Display_Data(0xCE, 0x0+WIND_SPEED_FULL);
    } else {
        TM1723_Write_Display_Data(0xCE,(lcdNumber8_Low[lcd_t.number8_low] + WIND_SPEED_FULL) & mask);
    }

    //update_wind_speed_display(mask);
   #else 

   	 // DIGIT45
	    if (!blink_on) {
	        TM1723_Write_Display_Data(ADDR_DIGIT45, (T8_HUM+lcdNumber4_Low[lcd_t.number4_low])& 0x0F); // 共阴空白
	    } else {
	        TM1723_Write_Display_Data(ADDR_DIGIT45,
	            (T8_HUM+lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & 0xFF);
	    }

	    // DIGIT56
	    t15_base = gpro_t.disp_fan_switch_flag ? T15 : T15_NO;
	    if (!blink_on) {
		    if(t15_base==0)
	          TM1723_Write_Display_Data(ADDR_DIGIT56, t15_base+0x0);
			else 
			    TM1723_Write_Display_Data(ADDR_DIGIT56, t15_base+0x0);
	    } 
		else{
			if(t15_base==0)
	        	TM1723_Write_Display_Data(ADDR_DIGIT56,(t15_base + lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high]) & 0XFF);
            else
	        	TM1723_Write_Display_Data(ADDR_DIGIT56,(t15_base + lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high]) & 0XFF);		
		}

	    // DIGIT67
	   // colon = gpro_t.disp_time_colon_flag ? TIME_COLON : TIME_NO_COLON;
	    if (!blink_on) {
	        TM1723_Write_Display_Data(ADDR_DIGIT67, 0x0);
	    } 
		else {
	        TM1723_Write_Display_Data(ADDR_DIGIT67,
	            (TIME_COLON + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]) & 0XFF);
	    }

	    // DIGIT78
	    t14_base = gpro_t.disp_fan_switch_flag ? 0 : 1;
	    if (!blink_on) {
			 if(t14_base==0){
			 	TM1723_Write_Display_Data(ADDR_DIGIT78, t14_base+0x0);
			 }
			 else{
			    TM1723_Write_Display_Data(ADDR_DIGIT78, t14_base+0x0);

			 }
	    } else {
			if(t14_base==0)
	         TM1723_Write_Display_Data(ADDR_DIGIT78,(t14_base+lcdNumber7_Low[lcd_t.number7_low] + lcdNumber8_High[lcd_t.number8_high]) & 0xFF);
            else 
			  TM1723_Write_Display_Data(ADDR_DIGIT78,(t14_base+lcdNumber7_Low[lcd_t.number7_low] + lcdNumber8_High[lcd_t.number8_high]) & 0xFF);	
		}

		//DIGIT8T
		t13_base = gpro_t.disp_fan_switch_flag ? 0 : 1;
		if (!blink_on) {
	        if(t13_base==0){
			if(run_t.disp_wind_speed_grade >66){
				TM1723_Write_Display_Data(0xCE,(T13+WIND_SPEED_FULL) );
			}
			else if(run_t.wifi_link_net_success ==1){ //WT.EDIT 2025.04.16 logic is not rigorous.
				if(run_t.disp_wind_speed_grade >33 && run_t.disp_wind_speed_grade <67){
				TM1723_Write_Display_Data(0xCE,(T13+WIND_SPEED_TWO) & 0xff);

			}
			else if(run_t.disp_wind_speed_grade <34){
				TM1723_Write_Display_Data(0xCE,(T13+WIND_SPEED_ONE));

			}
			}
	        }
			else{

			if(run_t.disp_wind_speed_grade >66){
				TM1723_Write_Display_Data(0xCE,WIND_SPEED_FULL);
			}
			else if(run_t.wifi_link_net_success ==1){ //WT.EDIT 2025.04.16 the logic is not rigorous
					if(run_t.disp_wind_speed_grade >33 && run_t.disp_wind_speed_grade <67){
					TM1723_Write_Display_Data(0xCE,WIND_SPEED_TWO);

			}
			else if(run_t.disp_wind_speed_grade <34){
				TM1723_Write_Display_Data(0xCE,WIND_SPEED_ONE);
			}

			}

			}
	    } 
		else {//blink_on = 1
	        if(t13_base==0){
			if(run_t.disp_wind_speed_grade >66){
				TM1723_Write_Display_Data(0xCE,(T13+lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_FULL) );
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
	        else if(t13_base ==1){
	       // TM1723_Write_Display_Data(0xCE,(lcdNumber8_Low[lcd_t.number8_low] + WIND_SPEED_FULL) & mask);
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
	    }

		t10_base = gpro_t.disp_fan_switch_flag ? 0 : 1;
        if(t10_base ==0)
	      TM1723_Write_Display_Data(0xCF,((T11+T16)& 0x05));//
	    else
		  TM1723_Write_Display_Data(0xCF,((T16+T12+T10)&0x0B));//


   #endif 
   
}
#endif 
/**
* @brief 
* @note
* @param
* @return
*/

// ====== 主函数 ======
void disp_set_timer_timing_value_fun(void) 
{
    static uint8_t tim_bit_1_hours, tim_bit_2_hours;
    //gpro_t.gTimer_disp_temp_humi_value = 0;
    uint8_t mask;
	
	 if (run_t.gTimer_digital5678_ms>= 3) { // 50*10ms = 500ms
			 run_t.gTimer_digital5678_ms=0;
			   blink_on = !blink_on;
		}

	  if(lcd_t.gTimer_leaf_counter > LEAF_TOGGLE_THRESHOLD) {//3*100ms

            lcd_t.gTimer_leaf_counter = 0;
            gpro_t.disp_fan_switch_flag  ^= 1;
	  	}

    if (run_t.gTimer_key_timing < 4) {
        tim_bit_2_hours = run_t.timer_time_hours / 10;
        tim_bit_1_hours = run_t.timer_time_hours % 10;
        run_t.timer_time_minutes = 0;

        lcd_t.number5_low = lcd_t.number5_high = tim_bit_2_hours;
        lcd_t.number6_low = lcd_t.number6_high = tim_bit_1_hours;
        lcd_t.number7_low = lcd_t.number7_high = 0;
        lcd_t.number8_low = lcd_t.number8_high = 0;

        mask = blink_on ? 0xFF : 0x0F; // 闪烁掩码
        display_digits(mask, blink_on);

    } 
	else{
        run_t.timer_time_minutes = 0;
        run_t.gTimer_timing = 0;

        if (run_t.timer_time_hours != 0){
            run_t.timer_timing_define_flag = timing_success;
            run_t.display_set_timer_or_works_time_mode = timer_time;
            run_t.gModel = 2;
            if (wifi_link_net_state() == 1) {
                gpro_t.receive_copy_cmd = ack_not_ai_mode;
                gpro_t.gTimer_again_send_power_on_off = 0;
                SendData_Set_Command(0x27, 0x02);
            }
        } 
		else{
            run_t.timer_timing_define_flag = timing_not_definition;
            run_t.display_set_timer_or_works_time_mode = works_time;
            run_t.gModel = 1;
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
#if 0
void disp_fan_leaf_run_icon(void)
{
    static uint8_t switch_flag;

    // 检查是否有风扇或PTC警告
    if (run_t.fan_warning != 0 || run_t.ptc_warning != 0) return;

    // 检查是否处于非定时器模式
    if (run_t.display_set_timer_or_works_time_mode == setup_timer) return;

    // 控制风扇动画切换频率
    if (lcd_t.gTimer_leaf_counter <= 4) return;

    lcd_t.gTimer_leaf_counter = 0;
    switch_flag ^= 0x01;

    // 显示通用部分
    works_timer_disp_numaber();

    TM1723_Write_Display_Data(0xC9, (HUM_T8 + lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & 0xff);

    uint8_t ca_data = lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high];
    if (switch_flag == 1) {
        ca_data += T15;  // 特殊偏移用于动画效果
    }
    TM1723_Write_Display_Data(0xCA, ca_data);

    uint8_t colon_base = gpro_t.disp_time_colon_flag ? TIME_COLON : TIME_NO_COLON;
    TM1723_Write_Display_Data(0xCB, colon_base + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]);

    // 控制风扇叶片显示状态
    fan_disp_speed_leaf(switch_flag == 1 ? 0 : 1);

	 /* 冒号闪烁计数器（与上面独立，保持原行为） */
    if (lcd_t.gTimer_colon_counter > 0) {
        lcd_t.gTimer_colon_counter = 0;
        colon_flag ^= 1;
        gpro_t.disp_time_colon_flag = (colon_flag != 0) ? 1 : 0;
    }
	
}
#else 
void disp_fan_leaf_run_icon(void)
{
    
    static uint8_t colon_base,colon_flag;
    static uint8_t t15_base;


    /* 主显示更新：仅当无风扇/ptc 报警时执行 */
    if (run_t.fan_warning == 0 && run_t.ptc_warning == 0) {

        if (run_t.display_set_timer_or_works_time_mode != setup_timer){
            if(lcd_t.gTimer_leaf_counter > LEAF_TOGGLE_THRESHOLD){ //3*100ms

            lcd_t.gTimer_leaf_counter = 0;
            gpro_t.disp_fan_switch_flag  ^= 1;

            /* 更新要显示的数字（小时/分钟）并写入公共位,数字5678 */
            works_time_fun();
            TM1723_Write_Display_Data(0xC9,
                (T8_HUM + lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & 0xFF);

            /* 位置 CA 的不同位（闪烁效果） */
             t15_base = gpro_t.disp_fan_switch_flag ? T15 : T15_NO;
             TM1723_Write_Display_Data(0xCA,
                    t15_base + lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high]);
          

            /* 冒号根据全局标志显示/隐藏 */
			colon_base = gpro_t.disp_time_colon_flag ? TIME_COLON : TIME_NO_COLON;
            TM1723_Write_Display_Data(0xCB,
                    colon_base + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]);
           

            /* 风速显示由 fan_disp_speed_leaf 处理，注意保持原有参数关系 */
            fan_disp_speed_leaf(gpro_t.disp_fan_switch_flag ? 0 : 1);
        }
    	}
    }
	 /* 冒号闪烁计数器（与上面独立，保持原行为） */
    if (lcd_t.gTimer_colon_counter > 0) {
        lcd_t.gTimer_colon_counter = 0;
        colon_flag ^= 1;
        gpro_t.disp_time_colon_flag = (colon_flag != 0) ? 1 : 0;
    }
}
//	   else if(run_t.display_set_timer_or_works_time_mode == setup_timer && lcd_t.gTimer_leaf_counter > LEAF_TOGGLE_THRESHOLD) {//3*100ms){
//
//			lcd_t.gTimer_leaf_counter = 0;
//            gpro_t.disp_fan_switch_flag  ^= 1;
//	   
//			 // DIGIT45
//	    if (!blink_on) {
//	        TM1723_Write_Display_Data(ADDR_DIGIT45, (T8_HUM+lcdNumber4_Low[lcd_t.number4_low])& 0x0F); // 共阴空白
//	    } else {
//	        TM1723_Write_Display_Data(ADDR_DIGIT45,
//	            (T8_HUM+lcdNumber4_Low[lcd_t.number4_low] + lcdNumber5_High[lcd_t.number5_high]) & 0xFF);
//	    }
//
//	    // DIGIT56
//	    t15_base = gpro_t.disp_fan_switch_flag ? T15 : T15_NO;
//	    if (!blink_on) {
//		    
//	        TM1723_Write_Display_Data(ADDR_DIGIT56, t15_base+0x0);
//	    } else {
//			
//	        TM1723_Write_Display_Data(ADDR_DIGIT56,
//	            (t15_base + lcdNumber5_Low[lcd_t.number5_low] + lcdNumber6_High[lcd_t.number6_high]) & 0XFF);
//	    }
//
//	    // DIGIT67
//	   // colon = gpro_t.disp_time_colon_flag ? TIME_COLON : TIME_NO_COLON;
//	    if (!blink_on) {
//	        TM1723_Write_Display_Data(ADDR_DIGIT67, 0x0);
//	    } else {
//	        TM1723_Write_Display_Data(ADDR_DIGIT67,
//	            (TIME_COLON + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]) & 0XFF);
//	    }
//
//	    // DIGIT78
//	    t14_base = gpro_t.disp_fan_switch_flag ? 0 : 1;
//	    if (!blink_on) {
//			 TM1723_Write_Display_Data(ADDR_DIGIT78, t14_base+0x0);
//	    } else {
//			
//	        TM1723_Write_Display_Data(ADDR_DIGIT78,(t14_base+lcdNumber7_Low[lcd_t.number7_low] + lcdNumber8_High[lcd_t.number8_high]) & 0xFF);
//	    }
//
//		//DIGIT8T
//		t13_base = gpro_t.disp_fan_switch_flag ? 0 : 1;
//		if (!blink_on) {
//	       
//			if(run_t.disp_wind_speed_grade >66){
//				TM1723_Write_Display_Data(0xCE,(T13+WIND_SPEED_FULL) );
//			}
//			else if(run_t.wifi_link_net_success ==1){ //WT.EDIT 2025.04.16 logic is not rigorous.
//				if(run_t.disp_wind_speed_grade >33 && run_t.disp_wind_speed_grade <67){
//				TM1723_Write_Display_Data(0xCE,(T13+WIND_SPEED_TWO) & 0xff);
//
//			}
//			else if(run_t.disp_wind_speed_grade <34){
//				TM1723_Write_Display_Data(0xCE,(T13+WIND_SPEED_ONE) & 0xff);
//
//			}
//			}
//	    } else {
//	       // TM1723_Write_Display_Data(0xCE,(lcdNumber8_Low[lcd_t.number8_low] + WIND_SPEED_FULL) & mask);
//			if(run_t.disp_wind_speed_grade >66){
//			TM1723_Write_Display_Data(0xCE,(lcdNumber8_Low[lcd_t.number8_low]+ WIND_SPEED_FULL) & 0xff);
//			}
//			else if(run_t.wifi_link_net_success ==1){ //WT.EDIT 2025.04.16 the logic is not rigorous
//			if(run_t.disp_wind_speed_grade >33 && run_t.disp_wind_speed_grade <67){
//			TM1723_Write_Display_Data(0xCE,(lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_TWO) & 0xff);
//
//			}
//			else if(run_t.disp_wind_speed_grade <34){
//			TM1723_Write_Display_Data(0xCE,(lcdNumber8_Low[lcd_t.number8_low]+WIND_SPEED_ONE) & 0xff);
//			}
//			}
//	    }
//
//
//	   }
//	}

   




#endif 

/**
* @brief 
* @note
* @param
* @return
*/
static void works_time_fun(void)
{
      
    if(run_t.display_set_timer_or_works_time_mode ==works_time){//switch(run_t.setup_timer_timing_item){

    
        
         lcd_t.number5_low=(run_t.dispTime_hours ) /10;
         lcd_t.number5_high =lcd_t.number5_low;//(run_t.dispTime_hours) /10;

    	 lcd_t.number6_low = (run_t.dispTime_hours ) %10;;
    	 lcd_t.number6_high =  lcd_t.number6_low ;//(run_t.dispTime_hours ) %10;
         
         lcd_t.number7_low = (run_t.dispTime_minutes )/10;
    	 lcd_t.number7_high = lcd_t.number7_low;//(run_t.dispTime_minutes )/10;

    	 lcd_t.number8_low = (run_t.dispTime_minutes )%10;
    	 lcd_t.number8_high = lcd_t.number8_low ;//(run_t.dispTime_minutes )%10;

    }

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

static void fan_set_timer_disp_leaf(uint8_t disp)
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




