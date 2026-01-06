#ifndef __BSP_MAIN_PARSE_H
#define __BSP_MAIN_PARSE_H
#include "main.h"

typedef enum {
    UART_STATE_WAIT_HEADER = 0,
    UART_STATE_NUM=1,
    UART_STATE_CMD_NOTICE=2,
    UART_STATE_EXEC_CMD_OR_LEN=3,
	 UART_STATE_FRAME_CMD_0X0=4,
    UART_STATE_FRAME_END=5,
    UART_STATE_BCC_CHECK,
    UART_STATE_DATA_LEN,
    UART_STATE_DATA,
    UART_STATE_DATA_END,
    UART_STATE_DATA_BCC,
    UART_STATE_FRAME_DATA_END,
    UART_STATE_DATA_BCC_CHECK
} uart_parse_state_t;

typedef enum{

    power_on_off=1,
    ptc_on_off=2,
    plasma_on_off=3,
    ultrasonic_on_off=4,
    wifi_link=5,
    buzzer_sound=6,
    ai_mode=7,
    temp_high_warning=8,
    fan_warning=9,
    fan_on_off = 0x0B,
    wifi_power_off = 0x31, //WT.EDIT 2025.12.15

     //notice no sound 
    ack_power_on_off = 0x10,
    ack_ptc_on_off = 0x12,
    ack_plasma_on_ff= 0x13,
    ack_ultrasonic_on_off = 0x14,
}signal_parase_t;


void S03_Protocol_ByteHandler(uint8_t *pdch);



#endif 

