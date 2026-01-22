#ifndef __BSP_LCD_H_
#define __BSP_LCD_H_
#include "main.h"

#if 0
#define TM1723_POWER_ON()			   HAL_GPIO_WritePin(LCD_POWER_GPIO_Port, LCD_POWER_Pin,GPIO_PIN_SET)
#define TM1723_POWER_OFF()             HAL_GPIO_WritePin(LCD_POWER_GPIO_Port, LCD_POWER_Pin,GPIO_PIN_RESET)


#define LCD_BACK_LIGHT_ON()				HAL_GPIO_WritePin(BACK_LIGHT_GPIO_Port,BACK_LIGHT_Pin,GPIO_PIN_SET)
#define LCD_BACK_LIGHT_OFF()            HAL_GPIO_WritePin(BACK_LIGHT_GPIO_Port,BACK_LIGHT_Pin,GPIO_PIN_RESET)




#define TM1723_CLK_SetHigh()            HAL_GPIO_WritePin(TM1723_SCLK_GPIO_Port,TM1723_SCLK_Pin ,GPIO_PIN_SET)    // ����ߵ�ƽ
#define TM1723_CLK_SetLow()             HAL_GPIO_WritePin(TM1723_SCLK_GPIO_Port,TM1723_SCLK_Pin ,GPIO_PIN_RESET)  // ����͵�ƽ

#define TM1723_DIO_SetHigh()            HAL_GPIO_WritePin(TM1723_DIO_GPIO_Port,TM1723_DIO_Pin,GPIO_PIN_SET)    // ����ߵ�ƽ
#define TM1723_DIO_SetLow()             HAL_GPIO_WritePin(TM1723_DIO_GPIO_Port,TM1723_DIO_Pin,GPIO_PIN_RESET)    // output low level

#define TM1723_STB_SetHigh()            HAL_GPIO_WritePin(TM1723_STB_GPIO_Port,TM1723_STB_Pin,GPIO_PIN_SET)
#define TM1723_STB_SetLow()             HAL_GPIO_WritePin(TM1723_STB_GPIO_Port,TM1723_STB_Pin,GPIO_PIN_RESET)

#endif 
//extern volatile uint8_t DispData[3];//??LED??

#define LCD_BACK_LIGHT_ON()				LL_GPIO_SetOutputPin(BACK_LIGHT_GPIO_Port,BACK_LIGHT_Pin)
#define LCD_BACK_LIGHT_OFF()            LL_GPIO_ResetOutputPin(BACK_LIGHT_GPIO_Port,BACK_LIGHT_Pin)

#define		BitSET(x,y)		x|=(1<<y)				//?1
#define		BitCLR(x,y)		x&=~(1<<y)				//?0


 
#define WIFI_Symbol     		0x01 //addr 0xC5
#define WIFI_NO_Symbol          0x0
 
#define LINE_Symbol             0x01 //addr  0xC2
 
 
#define AI_Symbol    			0x01 //addr 0xC3
#define AI_NO_Symbol            0x0
 
 
#define DRY_Symbol              0x02  //addr 0xC2 ->T4
#define DRY_NO_Symbol           0x0
 
#define KILL_Symbol             0x04  //addr 0xC2 ->T5
#define KILL_NO_Symbol           0x0
 
#define BUG_Symbol            0x08   //addr 0xC2 ->T6
#define BUG_NO_Symbol         0x0
 
#define TEMP_Symbol              0x01     //addr 0xC4
#define HUMI_Symbol              0x01      //addr  0xC9
 
#define TWO_DOT_Symbol           0x01       //addr 0xCB


#define T8_HUM                  0x01

#define TIME_COLON              0x01
#define TIME_NO_COLON           0X0

 
#define T10                      0x08      //addr 0xCF
#define T11                      0x04      //addr 0xCF
#define T12                      0x02      //addr 0xCF
#define T16                      0x01     //addr 0xCF
 
#define T13                      0x01      //addr 0xCE
#define T13_NO                   0
#define T14                      0x01      //addr 0xCC
#define T14_NO                   0
#define T15                      0x01      //addr 0xCA
#define T15_NO                   0X0
#define WIND_T16                 0x01      //addr 0xCF
#define WIND_T16_NO              0x0      //addr 0xCF

 
#define T17_T18_T19				 0xE0  
#define WIND_SPEED_ONE           0x20
#define WIND_SPEED_TWO           0x60
#define WIND_SPEED_FULL          0xE0

 
#define LUM_VALUE            0x92//WT.EDIT 2025.03.29 0x94// 0x97



#define DisplayReg           0x40// to write display register 
  
#define AddrFixed 	         0x44// 
#define AddrAutoAdd         0x40 
 
   
  
#define OpenDispTM1723          0x97 // full screen is tunr on light
#define OpenDispTM1723_5        0X9f
#define TM1723Default          	0x87
#define CloseDispTM1723		    0x80// 



 //Display Address 
#define Addr00H  		0xC0//??00H
#define Addr01H  		0xC1//??01H

#define Addr02H  		0xC2//??02H
#define Addr03H  		0xC3//??03H
//digital 3
#define Addr04H  		0xC4//??03H
#define Addr05H  		0xC5//??03H
//digital 4
#define Addr06H  		0xC6//??00H
#define Addr07H  		0xC7//??01H


//Humidity SMG address
#define Addr08H  		0xC8//??02H
#define Addr09H  		0xC9//??03H

#define Addr0AH  		0xCA//??03H
#define Addr0BH  		0xCB//??03H

//temperature SMG address
#define Addr0CH         0xCC//??0CH
#define Addr0DH         0xCD//??0DH

#define Addr0EH         0xCE//??0CH
#define Addr0FH         0xCF//??0DH


extern const uint8_t lcdNumber1_Low[] ;// ={0x0A,0x0A,0x06,0x0E,0x0E,0x0C,0x0C,0x0A,0x0E,0x0E};
extern const uint8_t  lcdNumber1_High[]; //={0xF0,0,0xD0,0x90,0x20,0xB0,0xF0,0x10,0xF0,0xB0,0x0};



extern const uint8_t lcdNumber2_Low[]  ;//={0x0A,0x0A,0x06,0x0E,0x0E,0x0C,0x0C,0x0A,0x0E,0x0E};
extern const uint8_t lcdNumber2_High[] ;//={0xF0,0,0xD0,0x90,0x20,0xB0,0xF0,0x10,0xF0,0xB0,0x0};

extern const uint8_t lcdNumber3_Low[] ;// ={0x0A,0x0A,0x06,0x0E,0x0E,0x0C,0x0C,0x0A,0x0E,0x0E};
extern const uint8_t lcdNumber3_High[];// ={0xF0,0,0xD0, 0x90,0x20,0xB0,0xF0,0x10,0xF0,0xB0,0x0};

extern const uint8_t lcdNumber4_Low[]  ;//={0x0A,0x0A,0x06,0x0E,0x0E,0x0C,0x0C,0x0A,0x0E,0x0E};
extern const uint8_t lcdNumber4_High[] ;//={0xF0,0,0xD0,0x90,0x20,0xB0,0xF0,0x10,0xF0,0xB0,0x0};

extern const uint8_t lcdNumber5_Low[]  ;//={0x0A,0x0A,0x06,0x0E,0x0E,0x0C,0x0C,0x0A,0x0E,0x0E};
extern const uint8_t lcdNumber5_High[];// ={0xF0,0,0xD0,0x90,0x20,0xB0,0xF0,0x10,0xF0,0xB0,0x0};

extern const uint8_t lcdNumber6_Low[]  ;//={0x0A,0x0A,0x06,0x0E,0x0E,0x0C,0x0C,0x0A,0x0E,0x0E,0x00};
extern const uint8_t lcdNumber6_High[] ;//={0xF0,0,0xD0,0x90,0x20,0xB0,0xF0,0x10,0xF0,0xB0,0x0};

extern const uint8_t lcdNumber7_Low[] ;// ={0x0A,0x0A,0x06,0x0E,0x0E,0x0C,0x0C,0x0A,0x0E,0x0E,0x00};
extern const uint8_t lcdNumber7_High[] ;//={0xF0,0,0xD0,0x90,0x20,0xB0,0xF0,0x10,0xF0,0xB0,0x0};

extern const uint8_t lcdNumber8_Low[] ;// ={0x0A,0x0A,0x06,0x0E,0x0E,0x0C,0x0C,0x0A,0x0E,0x0E,0x00};
extern const uint8_t lcdNumber8_High[];// ={0xF0,0,0xD0,0x90,0x20,0xB0,0xF0,0x10,0xF0,0xB0,0x0};

//"Er" ->digital positon '5''6'

extern const uint8_t lcdNumber5_Low_E[];//  ={0x04};
extern const uint8_t lcdNumber5_High_E[];// ={0xF0};

extern const uint8_t lcdNumber6_Low_r[];//={0x04};
extern const uint8_t lcdNumber6_High_r[];// ={0x40};


typedef struct _LCD_Ref{

     uint8_t wifi_glint_flag;

	 uint8_t gTimer_wifi_500ms;
	 uint8_t gTimer_fan_10ms;
     uint8_t display_beijing_time_flag ;

	 
	 uint8_t number1_low;
	 uint8_t number1_high;
	 uint8_t number2_low;
	 uint8_t number2_high;
	 uint8_t number3_low;
	 uint8_t number3_high;
	 uint8_t number4_low;
	 uint8_t number4_high;
	 
	 uint8_t number5_low;
	 uint8_t number5_high;

	 uint8_t number6_low;
	 uint8_t number6_high;

	 uint8_t number7_low;
	 uint8_t number7_high;

	 uint8_t number8_low;
	 uint8_t number8_high;

	 uint8_t  gTimer_colon_ms;
	 uint8_t  gTimer_leaf_counter;
	 uint8_t gTimer_colon_counter;
	 uint8_t gTimer_leaf_fast_counter;
	


}lcd_ref;



extern lcd_ref lcd_t; 


void TIM1723_Write_Cmd(uint8_t cmd);
void LCD_Display_Wind_Icon_Handler(void);
void TM1723_Write_Display_Data(uint8_t addr,uint8_t dat);

void lcd_donot_disp_screen(void);


#endif 
