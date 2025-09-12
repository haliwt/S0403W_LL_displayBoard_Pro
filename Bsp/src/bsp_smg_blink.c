#include "bsp.h"


// ====== 配置区 ======
#define ADDR_DIGIT45  0xC9
#define ADDR_DIGIT56  0xCA
#define ADDR_DIGIT67  0xCB   //time colon
#define ADDR_DIGIT78  0xCC
#define ADDR_WIND     0xCE
#define ADDR_MISC     0xCF

#define T8_HUM        0x01

volatile bool blink_on = true;       // 闪烁状态
volatile uint16_t blink_counter = 0; // 闪烁计数

// ====== 定时器中断（10ms周期） ======
//if (run_t.gTimer_digital5678_ms>= 4) { // 50*10ms = 500ms
//		 run_t.gTimer_digital5678_ms=0;
//		   blink_on = !blink_on;
//	}


// ====== 风速显示 ======
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

// ====== 数码管显示 ======
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
static void display_digits(uint8_t mask, bool blink) {
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
    uint8_t colon = gpro_t.disp_time_colon_flag ? TIME_COLON : TIME_NO_COLON;
    if (!blink) {
        TM1723_Write_Display_Data(ADDR_DIGIT67, 0x0);
    } else {
        TM1723_Write_Display_Data(ADDR_DIGIT67,
            (colon + lcdNumber6_Low[lcd_t.number6_low] + lcdNumber7_High[lcd_t.number7_high]) & mask);
    }

    // DIGIT78
    if (!blink) {
        TM1723_Write_Display_Data(ADDR_DIGIT78, 0x0);
    } else {
        TM1723_Write_Display_Data(ADDR_DIGIT78,
            (lcdNumber7_Low[lcd_t.number7_low] + lcdNumber8_High[lcd_t.number8_high]) & mask);
    }

    update_wind_speed_display(mask);

   
}
#endif 

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

    } else {
        run_t.timer_time_minutes = 0;
        run_t.gTimer_timing = 0;

        if (run_t.timer_time_hours != 0) {
            run_t.timer_timing_define_flag = timing_success;
            run_t.display_set_timer_or_works_time_mode = timer_time;
            run_t.gModel = 2;
            if (wifi_link_net_state() == 1) {
                gpro_t.receive_copy_cmd = ack_not_ai_mode;
                gpro_t.gTimer_again_send_power_on_off = 0;
                SendData_Set_Command(0x27, 0x02);
            }
        } else {
            run_t.timer_timing_define_flag = timing_not_definition;
            run_t.display_set_timer_or_works_time_mode = works_time;
            run_t.gModel = 1;
        }
    }

    TIM1723_Write_Cmd(LUM_VALUE);
}






