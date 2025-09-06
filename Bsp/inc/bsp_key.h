#ifndef __BSP_KEY_H_
#define __BSP_KEY_H_
#include "main.h"




//#define KEY_POWER_GetValue()          sys_gpio_pin_get(KEY_POWER_GPIO_Port,KEY_POWER_Pin)    //HAL_GPIO_ReadPin(KEY_POWER_GPIO_Port ,KEY_POWER_Pin)

#define KEY_POWER_GetValue()      (((KEY_POWER_GPIO_Port->IDR & KEY_POWER_Pin)==0U)? 1U:0U)

#define KEY_MODE_GetValue()           sys_gpio_pin_get(KEY_MODE_GPIO_Port,KEY_MODE_Pin)   // HAL_GPIO_ReadPin(KEY_MODE_GPIO_Port,KEY_MODE_Pin)


#define KEY_DEC_GetValue()              sys_gpio_pin_get(KEY_DEC_GPIO_Port,KEY_DEC_Pin)// HAL_GPIO_ReadPin(KEY_DEC_GPIO_Port,KEY_DEC_Pin)
#define KEY_ADD_GetValue()              sys_gpio_pin_get(KEY_ADD_GPIO_Port,KEY_ADD_Pin)// HAL_GPIO_ReadPin(KEY_ADD_GPIO_Port,KEY_ADD_Pin)

//#define VK36N4D_INT_VALUE()          HAL_GPIO_ReadPin(VK36N4D_INT_GPIO_Port,VK36N4D_INT_Pin) 



#define		_KEY_ALL_OFF				 0XFF

//normal times be pressed key
#define		_KEY_POWER     		0x01  //key power
#define		_KEY_MODE     		0x02  //key mode
#define		_KEY_DEC     			0x04  //key dec
#define		_KEY_ADD     			0x08  //key add



//long times be pressed
#define		_KEY_CONT_1_POWER     	     0x81    //??????--???
#define		_KEY_CONT_2_MODE   		     0x82    //??????--???
#define		_KEY_CONT_3_DEC    	         0x84    //?????? ?????
#define		_KEY_CONT_4_ADD     	     0x88    //??? --?????

#define KEY0_PRES 	0x01
#define KEY1_PRES	0x02
#define KEY2_PRES	0x04
#define WKUP_PRES   0x08



#define HIGH_LEVEL                   1  /* 根据原理图设计，KEY1按下时引脚为低电平，所以这里设置为0 */


typedef enum{

	
	POWER_ON_ITEM=0xe0,
	LINK_WIFI_ITEM=0x81,
	MODE_KEY_ITEM=0x02,
	MODE_KEY_LONG_TIME_KEY=0x82,
	ADD_KEY_ITEM=0x08,
	DEC_KEY_ITEM=0x04,
	POWER_OFF_ITEM=0x01,
	KEY_NULL=0xff,
}key_state;

typedef enum{

  POWER_OFF,
  POWER_ON,
  POWER_ID_FLAG,
  POWER_NULL


}power_state;





typedef enum
{
  KEY_UP   = 0,
  KEY_DOWN = 1,
}KEYState_TypeDef;

typedef enum _key_value{

  KEY_POWER= 0X80,KEY_MODE=0X40,KEY_DEC=0X20,KEY_ADD=0X10,KEY_LONG_POWER=0X81

}key_value;


typedef  struct  _state_
{
 unsigned char         read;
 unsigned char         buffer;
 unsigned long          value;
 unsigned char        off_time;
 unsigned long        on_time;

 enum{
  start  = 0,
  first  = 1,
  second = 2,
  cont   = 3,
  end    = 4,
  finish = 5,
 }state;
}key_types;



void SplitDispose_Key(uint8_t value);
uint8_t KEY_Scan(void);


uint8_t  KEY_GPIO_Scan(uint8_t mode);

void WaitingPowerOn_Fun(void);
void Wifi_Detected_KeyScan(void);

void Key_TheSecond_Scan(void);

void sys_gpio_pin_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t status);

uint8_t sys_gpio_pin_get(GPIO_TypeDef *p_gpiox, uint16_t pinx);


#endif 


