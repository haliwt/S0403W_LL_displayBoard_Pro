/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

//#define RX_BUFFER_SIZE 20
//volatile uint8_t  usart1_rx_buffer[RX_BUFFER_SIZE];
//volatile uint16_t usart1_rx_index = 0;


/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// 定义一个临时的解析缓冲区
uint8_t processing_buf[MAX_BUFFER_SIZE];
volatile uint16_t rx_len = 0;
uint32_t last_pos = 0; // 记录上次处理到的位置


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim17;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel 1 interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */
  if(LL_DMA_IsActiveFlag_TC1(DMA1)){ //transfer complete -TC
         LL_DMA_ClearFlag_TC1(DMA1);
		 
		// gpro_t.decoder_flag =1;
  }

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */
  if(LL_DMA_IsActiveFlag_TE1(DMA1)){ //transfer Error Flag,TE)
		 LL_DMA_ClearFlag_TE1(DMA1);
	 }

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 2 and channel 3 interrupts.
  */
void DMA1_Channel2_3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 0 */
    if(LL_DMA_IsActiveFlag_TC2(DMA1)){ //transfer complete -TC
         LL_DMA_ClearFlag_TC2(DMA1);
		// gpro_t.decoder_flag =1;
    }

    if(LL_DMA_IsActiveFlag_TE2(DMA1)){ //transfer Error Flag,TE)
        LL_DMA_ClearFlag_TE2(DMA1);
    }

  /* USER CODE END DMA1_Channel2_3_IRQn 0 */
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 1 */

  /* USER CODE END DMA1_Channel2_3_IRQn 1 */
}

/**
  * @brief This function handles TIM16 global interrupt.
  */
void TIM16_IRQHandler(void)
{
  /* USER CODE BEGIN TIM16_IRQn 0 */
   if(LL_TIM_IsActiveFlag_UPDATE(TIM16)){
	   
	 LL_TIM_ClearFlag_UPDATE(TIM16);
	  tim16_int_callback_handler();
	   
   }
  /* USER CODE END TIM16_IRQn 0 */
  /* USER CODE BEGIN TIM16_IRQn 1 */

  /* USER CODE END TIM16_IRQn 1 */
}

/**
  * @brief This function handles TIM17 global interrupt.
  */
void TIM17_IRQHandler(void)
{
  /* USER CODE BEGIN TIM17_IRQn 0 */

  /* USER CODE END TIM17_IRQn 0 */
  HAL_TIM_IRQHandler(&htim17);
  /* USER CODE BEGIN TIM17_IRQn 1 */

  /* USER CODE END TIM17_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	volatile uint8_t data;

 #if 0
    volatile uint8_t data;
   if(LL_USART_IsActiveFlag_RXNE_RXFNE(USART1)){
   
    data = LL_USART_ReceiveData8(USART1);

        usart1_isr_callback_handler(data);
	   // 存入缓冲区（简单环形缓冲）
//        usart1_rx_buffer[usart1_rx_index++] = data;
//       if (usart1_rx_index >= RX_BUFFER_SIZE)
//       {
//          usart1_rx_index = 0; // 环回
//        }
        
     

   }
   #else
   if (LL_USART_IsActiveFlag_IDLE(USART1)) { 
   	   LL_USART_ClearFlag_IDLE(USART1); 

      gpro_t.decoder_flag = 1;
	 // 1. 获取当前 DMA 写入的位置 (写指针)
        // CNDTR 寄存器是递减的，所以用总长度减去剩余长度
        uint32_t curr_pos = MAX_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_1);

        if (curr_pos != last_pos) 
        {
            // 2. 计算本次收到的数据长度
            uint32_t length = 0;
            if (curr_pos > last_pos) {
                length = curr_pos - last_pos;
                // 数据连续：rx_buf[last_pos] 到 rx_buf[curr_pos-1]
                memcpy(processing_buf, &rx_buf[last_pos], length);
            } 
			else {
                // 发生了环形回卷 (Wrap around)
                uint32_t len1 = MAX_BUFFER_SIZE - last_pos;
                uint32_t len2 = curr_pos;
                length = len1 + len2;
                // 分两段拷贝
                memcpy(inputBuf, &rx_buf[last_pos], len1);
                memcpy(&inputBuf[len1], &rx_buf[0], len2);
            }

            // 3. 更新长度并通知主循环
            rx_len = length; 
            gpro_t.decoder_flag = 1;
            last_pos = curr_pos; // 更新读指针
        }
    }

   


   #endif 

  /* USER CODE END USART1_IRQn 0 */
  /* USER CODE BEGIN USART1_IRQn 1 */

   /* 2. 错误处理（绝对不能省） */
    if(LL_USART_IsActiveFlag_ORE(USART1))
    {
        LL_USART_ClearFlag_ORE(USART1);
        // 发生溢出通常意味着波特率极高且 CPU 被其他高优先级中断卡住了
    }
    
    if(LL_USART_IsActiveFlag_FE(USART1) || LL_USART_IsActiveFlag_NE(USART1))
    {
        LL_USART_ClearFlag_FE(USART1);
        LL_USART_ClearFlag_NE(USART1);
    }

  /* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
