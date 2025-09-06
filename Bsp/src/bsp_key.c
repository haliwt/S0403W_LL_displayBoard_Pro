#include "bsp.h"


key_types key_t;
uint8_t power_key_detected;
uint16_t key_mode_counter;

//static uint8_t Key_Mode_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin,uint8_t number);


/**
 * @brief       设置GPIO某个引脚的输出状态
 * @param       p_gpiox: GPIOA~GPIOG, GPIO指针
 * @param       0X0000~0XFFFF, 引脚位置, 每个位代表一个IO, 第0位代表Px0, 第1位代表Px1, 依次类推. 比如0X0101, 代表同时设置Px0和Px8.
 *   @arg       SYS_GPIO_PIN0~SYS_GPIO_PIN15, 1<<0 ~ 1<<15
 * @param       status: 0/1, 引脚状态(仅最低位有效), 设置如下:
 *   @arg       0, 输出低电平
 *   @arg       1, 输出高电平
 * @retval      无
 */
void sys_gpio_pin_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t status)
{
    if (status & 0X01)
    {
        p_gpiox->BSRR |= pinx;  /* 设置GPIOx的pinx为1 */
    }
    else
    {
        p_gpiox->BRR |= pinx ;  /* 设置GPIOx的pinx为0 */
    }
}

/**
 * @brief       读取GPIO某个引脚的状态
 * @param       p_gpiox: GPIOA~GPIOG, GPIO指针
 * @param       0X0000~0XFFFF, 引脚位置, 每个位代表一个IO, 第0位代表Px0, 第1位代表Px1, 依次类推. 比如0X0101, 代表同时设置Px0和Px8.
 *   @arg       SYS_GPIO_PIN0~SYS_GPIO_PIN15, 1<<0 ~ 1<<15
 * @retval      返回引脚状态, 0, 低电平; 1, 高电平
 */
uint8_t sys_gpio_pin_get(GPIO_TypeDef *p_gpiox, uint16_t pinx)
{
    if (p_gpiox->IDR & pinx)
    {
        return 1;   /* pinx的状态为1 */
    }
    else
    {
        return 0;   /* pinx的状态为0 */
    }
}



#if 0

uint8_t KEY_Scan(void)
{

   uint8_t  reval = 0;
 
  key_t.read = _KEY_ALL_OFF; //0xFF 
  
  
    if(POWER_KEY_VALUE() ==KEY_DOWN )// high level
    {
        key_t.read &= ~0x01; // 0xff & 0xfe =  0xFE
    }
//    if(DEC_KEY_VALUE()==KEY_DOWN )
//	{
//		  key_t.read &= ~0x04; // 0xFf & 0xfB =  0xFB
//	}
//    else if(ADD_KEY_VALUE() ==KEY_DOWN )
//	{
//		  key_t.read &= ~0x08; // 0x1f & 0xf7 =  0xF7
//	 }
    else if(MODE_KEY_VALUE() ==KEY_DOWN )
	{
		key_t.read &= ~0x02; // 0xFf & 0xfd =  0xFD
	}
    
   

    switch(key_t.state )
	{
		case start:
		{
			if(key_t.read != _KEY_ALL_OFF)
			{
				key_t.buffer   = key_t.read; //??:key.buffer = 0xFE  POWER KEY 
				key_t.state    = first;
				key_t.on_time  = 0;
				key_t.off_time = 0;
                
			}
			break;
		}
		case first:
		{
			if(key_t.read == key_t.buffer) // adjust key be down ->continunce be pressed key
			{

			 if(++key_t.on_time>59 ){

					key_t.value = key_t.buffer^_KEY_ALL_OFF; // key.value = 0xFE ^ 0xFF = 0x01
					key_t.on_time = 0;                        //key .value = 0xEF ^ 0XFF = 0X10
                   
					key_t.state   = second;


				 }
			}
			else
			{
				key_t.on_time = 0; 
				key_t.state   = start;
			}
			break;
		}
		case second:
		{
			if(key_t.read == key_t.buffer) //again adjust key if be pressed down 
			{
				if(++key_t.on_time> 1000 && run_t.power_on==1)// 500 long key be down
				{
					
					key_t.value = key_t.value|0x80; //key.value(power_on) = 0x01 | 0x80  =0x81  
					key_t.on_time = 0;               //key.vaule(model_key) = 0x01 | 0x80 = 0x82
					key_t.state   = finish; 
                   
				}
			}
			else if(key_t.read == _KEY_ALL_OFF)  // loose hand 
				{
					if(++key_t.off_time>1) //8 //30 don't holding key dithering
					{
						key_t.value = key_t.buffer^_KEY_ALL_OFF; // key.value = 0x1E ^ 0x1f = 0x01
						
						key_t.state   = finish; // loose hand
					}
					
				}
		   
			break;
		}
		case finish:
		{
			
			reval = key_t.value; // is short time  TIMER_KEY = 0x01  2. long times TIMER_KEY = 0X81
			key_t.state   = end;
         
			break;
		}
		case end:
		{
			if(key_t.read == _KEY_ALL_OFF)
			{
				if(++key_t.off_time>0)//50 //100
				{
					key_t.state   = start;
                   
                  
				}
				
			}
			break;
		}
		default:
		{
			key_t.state   = start;
            
			break;
		}
	}
	return  reval;


}

#endif 

/********************************************************************
*
*Function Name:
*Function: interrupt of GPIO of call back function
*
*
*
*********************************************************************/


/**

  * @brief   ¼ì²âÊÇ·ñÓÐ°´¼ü°´ÏÂ

  * @param   ¾ßÌåµÄ¶Ë¿ÚºÍ¶Ë¿ÚÎ»

  *		@arg GPIOx: x¿ÉÒÔÊÇ£¨A...G£© 

  *		@arg GPIO_PIN ¿ÉÒÔÊÇGPIO_PIN_x£¨x¿ÉÒÔÊÇ1...16£©

  * @retval  °´¼üµÄ×´Ì¬

  *		@arg KEY_ON:°´¼ü°´ÏÂ

  *		@arg KEY_OFF:°´¼üÃ»°´ÏÂ

  */
#if 0
static uint8_t Key_Mode_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin,uint8_t number)

{			
      uint8_t  reval = 0;
 
     key_t.read = _KEY_ALL_OFF; //0xFF 
  
  
    if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) ==KEY_DOWN )
	{
		key_t.read &= ~number; // 0xFf & 0xfd =  0xFD
	}
    
   

    switch(key_t.state )
	{
		case start:
		{
			if(key_t.read != _KEY_ALL_OFF)
			{
				key_t.buffer   = key_t.read; //??:key.buffer = 0xFE  POWER KEY 
				key_t.state    = first;
				key_t.on_time  = 0;
				key_t.off_time = 0;
                
			}
			break;
		}
		case first:
		{
			if(key_t.read == key_t.buffer) // adjust key be down ->continunce be pressed key
			{

			 if(++key_t.on_time>54 ){

					key_t.value = key_t.buffer^_KEY_ALL_OFF; // key.value = 0xFE ^ 0xFF = 0x01
					key_t.on_time = 0;                        //key .value = 0xEF ^ 0XFF = 0X10
                   
					key_t.state   = second;


				 }
			}
			else
			{
				key_t.on_time = 0; 
				key_t.state   = start;
			}
			break;
		}
		case second:
		{
			if(key_t.read == key_t.buffer) //again adjust key if be pressed down 
			{
				if(++key_t.on_time> 1000 && run_t.power_on==1)// 500 long key be down
				{
					
					key_t.value = key_t.value|0x80; //key.value(power_on) = 0x01 | 0x80  =0x81  
					key_t.on_time = 0;               //key.vaule(model_key) = 0x01 | 0x80 = 0x82
					key_t.state   = finish; 
                   
				}
			}
			else if(key_t.read == _KEY_ALL_OFF)  // loose hand 
				{
					if(++key_t.off_time>2) //8 //30 don't holding key dithering
					{
						key_t.value = key_t.buffer^_KEY_ALL_OFF; // key.value = 0x1E ^ 0x1f = 0x01
						
						key_t.state   = finish; // loose hand
					}
					
				}
		   
			break;
		}
		case finish:
		{
			
			reval = key_t.value; // is short time  TIMER_KEY = 0x01  2. long times TIMER_KEY = 0X81
			key_t.state   = end;
         
			break;
		}
		case end:
		{
			if(key_t.read == _KEY_ALL_OFF)
			{
				if(++key_t.off_time>2)//50 //100
				{
					key_t.state   = start;
                   
                  
				}
				
			}
			break;
		}
		default:
		{
			key_t.state   = start;
            
			break;
		}
	}
	return  reval;

}

#if 0
void Key_TheSecond_Scan(void)
{
	uint8_t keyValue_model;

    switch(run_t.gPower_On){


    case POWER_OFF:

    run_t.power_key_interrupt_start_counter_flag=POWER_NULL;
    break;

    case POWER_ON:

        switch(run_t.power_key_interrupt_start_counter_flag){

        case 1:
           if(POWER_KEY_VALUE() ==KEY_UP){
			if(run_t.gTimer_key_counter < 2){
                SendData_PowerOnOff(0);
                HAL_Delay(10);
               run_t.wifi_led_fast_blink_flag=0;
               run_t.power_key_detected=0;
        
				run_t.gKey_command_tag = POWER_OFF_ITEM; 
                run_t.power_on= RUN_POWER_OFF;
				run_t.power_key_interrupt_start_counter_flag=POWER_NULL;
                run_t.power_key_interrupt_counter=0;
             
          
                return ;
			}
		
          }

    
        if(POWER_KEY_VALUE() ==KEY_DOWN){
           
			if(run_t.gTimer_key_counter> 2 || run_t.gTimer_key_counter==2){
            SendData_Set_Wifi(0x01);
            HAL_Delay(10);
            run_t.gTimer_key_counter=0;
			run_t.gKey_command_tag = LINK_WIFI_ITEM;
			run_t.power_key_interrupt_start_counter_flag=POWER_NULL;
			run_t.wifi_link_flag=0;

        
            run_t.power_key_detected=0;
      

			 return ;
            }
		}
    break;

    case 0:
      if(POWER_KEY_VALUE() ==KEY_DOWN){
        run_t.power_key_interrupt_counter++;
         if(run_t.gTimer_key_counter==0 &&  run_t.power_key_detected==0){
            run_t.power_key_detected++;
             run_t.gTimer_detected_power_key =0; 
         }
         if(run_t.gTimer_detected_power_key >=2 && run_t.power_key_detected==1){
             run_t.power_key_detected++;

            SendData_Set_Wifi(0x01);
            HAL_Delay(10);

			run_t.gKey_command_tag = LINK_WIFI_ITEM;
			run_t.power_key_interrupt_start_counter_flag=POWER_ID_FLAG;
         
			run_t.wifi_link_flag=0;
         
           

         }

      }



    break;

  
    }
 



	
		keyValue_model = Key_Mode_Scan(KEY_MODE_GPIO_Port,KEY_MODE_Pin,0x02);
		if(keyValue_model == 0X02) run_t.gKey_command_tag = MODE_KEY_ITEM;
		if(keyValue_model==0x82)run_t.gKey_command_tag = MODE_KEY_LONG_TIME_KEY;
	
    break;

   }

}
#endif 
#endif 

