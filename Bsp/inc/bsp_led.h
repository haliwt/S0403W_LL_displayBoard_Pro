#ifndef __BSP_LED_H_
#define __BSP_LED_H_
#include "main.h"
#include "gpio.h"

//extern IWDG_HandleTypeDef IWDG_Handler;

#define POWER_ON_LED()             LL_GPIO_SetOutputPin(LED_POWER_GPIO_Port, LED_POWER_Pin)//do{LED_INFO_POWER_GPIO_Port->BSRR |= LED_INFO_POWER_Pin;}while(0)//HAL_GPIO_WritePin(LED_INFO_POWER_GPIO_Port, LED_INFO_POWER_Pin,GPIO_PIN_SET)
#define POWER_OFF_LED()            LL_GPIO_ResetOutputPin(LED_POWER_GPIO_Port, LED_POWER_Pin)//do{LED_INFO_POWER_GPIO_Port->BRR = LED_INFO_POWER_Pin; }while(0)//HAL_GPIO_WritePin(LED_INFO_POWER_GPIO_Port, LED_INFO_POWER_Pin,GPIO_PIN_RESET)
#define POWER_LED_TOGGLE()         do{LED_POWER_GPIO_Port->ODR ^=LED_POWER_Pin;}while(0)//LL_GPIO_TogglePin(LED_POWER_GPIO_Port, LED_POWER_Pin)//

#define LED_MODEL_ON()				LL_GPIO_SetOutputPin(LED_MODE_GPIO_Port, LED_MODE_Pin)//HAL_GPIO_WritePin(LED_INFO_MODE_GPIO_Port , LED_INFO_MODE_Pin,GPIO_PIN_SET)
#define LED_MODEL_OFF()             LL_GPIO_ResetOutputPin(LED_MODE_GPIO_Port, LED_MODE_Pin)//HAL_GPIO_WritePin(LED_INFO_MODE_GPIO_Port , LED_INFO_MODE_Pin,GPIO_PIN_RESET)


//#define POWER_LED_TOGGLE()          do{LED_POWER_GPIO_Port-> ^=ODR, LED_POWER_Pin;}while(0)  //HAL_GPIO_TogglePin(LED_INFO_POWER_GPIO_Port, LED_INFO_POWER_Pin)




extern void (*panel_led_fun)(void);


void Breath_Led(void);
void Lcd_PowerOn_Fun(void);

void Lcd_PowerOff_Fun(void);

void Panel_Led_OnOff_RunCmd(void (*panelledHandler)(void));


void Panel_Led_OnOff_Function(void);

void lcd_power_off_light_off(void);

//void IWDG_Init(uint8_t prer,uint16_t rlr);
//void IWDG_Feed(void);


#endif 
