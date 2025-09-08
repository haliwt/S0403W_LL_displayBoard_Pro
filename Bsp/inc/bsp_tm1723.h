#ifndef __BSP_TM1723_H
#define __BSP_TM1723_H
#include "main.h"

#define STB_HIGH()      LL_GPIO_SetOutputPin(TM1723_STB_GPIO_Port, TM1723_STB_Pin)
#define STB_LOW()       LL_GPIO_ResetOutputPin(TM1723_STB_GPIO_Port, TM1723_STB_Pin)
#define SCLK_HIGH()     LL_GPIO_SetOutputPin(TM1723_SCLK_GPIO_Port, TM1723_SCLK_Pin)
#define SCLK_LOW()      LL_GPIO_ResetOutputPin(TM1723_SCLK_GPIO_Port, TM1723_SCLK_Pin)
#define DIO_HIGH()      LL_GPIO_SetOutputPin(TM1723_DIO_GPIO_Port, TM1723_DIO_Pin)
#define DIO_LOW()       LL_GPIO_ResetOutputPin(TM1723_DIO_GPIO_Port, TM1723_DIO_Pin)


#define TM1723_STB_SetHigh()      LL_GPIO_SetOutputPin(TM1723_STB_GPIO_Port, TM1723_STB_Pin)
#define TM1723_STB_SetLow()       LL_GPIO_ResetOutputPin(TM1723_STB_GPIO_Port, TM1723_STB_Pin)

#define TM1723_CLK_SetHigh()     LL_GPIO_SetOutputPin(TM1723_SCLK_GPIO_Port, TM1723_SCLK_Pin)
#define TM1723_CLK_SetLow()      LL_GPIO_ResetOutputPin(TM1723_SCLK_GPIO_Port, TM1723_SCLK_Pin)

#define TM1723_DIO_SetHigh()      LL_GPIO_SetOutputPin(TM1723_DIO_GPIO_Port, TM1723_DIO_Pin)
#define TM1723_DIO_SetLow()       LL_GPIO_ResetOutputPin(TM1723_DIO_GPIO_Port, TM1723_DIO_Pin)
 

void TM1723_Init(void);


void TM1723_DisplayDigit(uint8_t pos, uint8_t num);



#endif 
