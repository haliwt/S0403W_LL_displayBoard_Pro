#ifndef __BSP_FREERTOS_H
#define __BSP_FREERTOS_h
#include "main.h"



void freeRTOS_Handler(void);


void App_PowerOn_Handler(void);

void App_PowerOff_Handler(void);

void app_decoder_task_isr_handler(void);

void app_xusart1_queue_isr_handler(uint8_t data);



#endif 



