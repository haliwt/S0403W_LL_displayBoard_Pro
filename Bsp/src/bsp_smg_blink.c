#include "bsp.h"


// ====== 配置区 ======
#define ADDR_DIGIT45  0xC9
#define ADDR_DIGIT56  0xCA
#define ADDR_DIGIT67  0xCB   //time colon
#define ADDR_DIGIT78  0xCC
#define ADDR_WIND     0xCE
#define ADDR_MISC     0xCF

#define T8_HUM        0x01



#define LEAT_TOGGLE_SLOW         200

volatile bool blink_on = true;       // 闪烁状态
volatile uint16_t blink_counter = 0; // 闪烁计数


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


/**
* @brief 
* @note // ====== 数码管显示 ======
* @param
* @return
*/




    	

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
*@brief : display time 4 number hours,mintues,
**/

void display_time_hours_minutes_fun(void)
{
   disp_fan_speed_and_time_handler();
}
/************************************************************
*
*Funtion Name:void dsiplay_numbers_one_to_four_fun(uint8_t disp)
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




