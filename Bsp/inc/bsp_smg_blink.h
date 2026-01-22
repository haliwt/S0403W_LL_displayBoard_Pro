#ifndef __BSP_SMG_BLINK_H
#define __BSP_SMG_BLINK_H
#include "main.h"



void disp_set_timer_timing_value_fun(void) ;



void disp_fan_leaf_run_icon(void);

void fan_disp_speed_leaf(uint8_t dsip);

void display_digits(uint8_t mask, bool blink) ;

void counter_time_numbers(void);

void disp_time_colon_fun(void);

void fan_runing_fun(void);


#endif 

