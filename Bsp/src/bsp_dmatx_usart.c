#include "bsp.h"


void usart1_dma_send(uint8_t *txbuf,uint16_t txlen)
{

   if(txbuf == NULL || txlen ==0)   return ;

	if(gpro_t.usart1_dma_sending_flag == 1) return ;
	
	gpro_t.usart1_dma_sending_flag =1;
   

	LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_2);

    LL_DMA_ConfigAddresses(DMA1,LL_DMA_CHANNEL_2,
                            (uint32_t)txbuf,
                            (uint32_t)&USART1->TDR,
                            LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetDataLength(DMA1,LL_DMA_CHANNEL_2,txlen);


    
	LL_DMA_ClearFlag_GI2(DMA1);   // 清除全局中断标志
	LL_DMA_ClearFlag_TC2(DMA1);   // 清除传输完成  TC1 -> clear transfer complete flag
	LL_DMA_ClearFlag_TE2(DMA1);   // 清除传输错误


	
    LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_2);

    LL_USART_EnableDMAReq_TX(USART1);

}



void usart1_dma_receive_config(void)
{
//  LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_1);
//  LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_1, 
//                         LL_USART_DMA_GetRegAddr(USART1, LL_USART_DMA_REG_DATA_RECEIVE), 
//                         (uint32_t)rx_buf, 
//                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY); 
//  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, MAX_BUFFER_SIZE); 
//  // 4. 使能 USART1 DMA 接收 
//  // 使能传输完成中断 
//  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1); 
//  // 使能 DMA 通道 
//  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
//  LL_USART_EnableDMAReq_RX(USART1);


}
