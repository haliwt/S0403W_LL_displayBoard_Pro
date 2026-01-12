#ifndef __BSP_FREERTOS_H
#define __BSP_FREERTOS_h
#include "main.h"



void freeRTOS_Handler(void);


void xTask_PowerOn_Handler(void);

void xTask_PowerOff_Handler(void);

void xtask_decoder_task_isr_handler(void);

void app_xusart1_queue_isr_handler(uint8_t data);

void semaphore_isr(void);


#endif 



