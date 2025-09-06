#include "bsp.h"


volatile static uint8_t transOngoingFlag; //interrupt Transmit flag bit , 1---stop,0--run
uint8_t outputBuf[8];
static uint8_t transferSize;
//static uint8_t state;
uint8_t inputBuf[MAX_BUFFER_SIZE];


/****************************************************************************************************
**
*Function Name:static void selectLight(uint8_t index)
*Function: UART2 transmit interrupt process ---4D 58 4C 53 32 30 32 
*Input Ref: LED number 
*Return Ref:NO
*
****************************************************************************************************/
void SendData_PowerOnOff(uint8_t index)
{
	
   //crc=0x55;
	outputBuf[0]=0xA5; //display board head = 0xA5
	outputBuf[1]= 0x01; //display device Number:is 0x01
	outputBuf[2]=0x01; // command type = 0x01 ->power on or power off 
	outputBuf[3]=index; // command order -> 01 - power on , 00- power off
	outputBuf[4]=0x00; // data is length: 00 ->don't data 
	outputBuf[5]=0xFE; // frame of end code -> 0xFE.
	
	outputBuf[6] = bcc_check(outputBuf,6);
	transferSize=7;
	usart1_dma_send(outputBuf,transferSize);
	// if(transferSize)
	// {
	// 	while(transOngoingFlag);
	// 	transOngoingFlag=1;
	// 	//HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
	// }
	
}
void SendData_Buzzer(void)
{
	
    outputBuf[0]=0xA5; //display board head = 0xA5
	outputBuf[1]= 0x01; //display device Number:is 0x01
	outputBuf[2]=0x06; // command type = 0x06 ->buzzer sound open or not
	outputBuf[3]=0x01; // command order -> 01 - buzzer sound done, 00- don't buzzer sound 
	outputBuf[4]=0x00; // data is length: 00 ->don't data 
	outputBuf[5]=0xFE; // frame of end code -> 0xFE.
	
	outputBuf[6] = bcc_check(outputBuf,6);
	transferSize=7;
	usart1_dma_send(outputBuf,transferSize);
	// if(transferSize)
	// {
	// 	while(transOngoingFlag);
	// 	transOngoingFlag=1;
	// 	//HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
	// }
	
}

void SendData_Buzzer_Has_Ack(void)
{
	
    outputBuf[0]=0xA5; //display board head = 0xA5
	outputBuf[1]= 0x01; //display device Number:is 0x01
	outputBuf[2]=0x16; // command type = 0x06 ->buzzer sound open or not
	outputBuf[3]=0x01; // command order -> 01 - buzzer sound done, 00- don't buzzer sound 
	outputBuf[4]=0x00; // data is length: 00 ->don't data 
	outputBuf[5]=0xFE; // frame of end code -> 0xFE.
	
	outputBuf[6] = bcc_check(outputBuf,6);
	transferSize=7;
	usart1_dma_send(outputBuf,transferSize);
	// if(transferSize)
	// {
	// 	while(transOngoingFlag);
	// 	transOngoingFlag=1;
	// //	HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
	// }
	
}


void SendData_Set_Command(uint8_t cmd,uint8_t data)
{
    outputBuf[0]=0xA5; //display board head = 0xA5
	outputBuf[1]= 0x01; //display device Number:is 0x01
	outputBuf[2]=cmd; // command type = 0x06 ->buzzer sound open or not
	outputBuf[3]= data; // command order -> 01 - buzzer sound done, 00- don't buzzer sound 
	outputBuf[4]=0x00; // data is length: 00 ->don't data 
	outputBuf[5]=0xFE; // frame of end code -> 0xFE.
    outputBuf[6] = bcc_check(outputBuf,6);


		transferSize=7;
		usart1_dma_send(outputBuf,transferSize);
		// if(transferSize)
		// {
		// 	while(transOngoingFlag);
		// 	transOngoingFlag=1;
		// //	HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
		// }
	
}

/**
* @brief
* @note
*
*/
void SendCmd_fun(uint8_t cmd,uint8_t excmd)
{
    outputBuf[0]=0xA5; //display board head = 0xA5
	outputBuf[1]= 0x01; //display device Number:is 0x01
	outputBuf[2]=cmd; // command type = 0x06 ->buzzer sound open or not
	outputBuf[3]= excmd; // command order -> 01 - buzzer sound done, 00- don't buzzer sound 
	outputBuf[4]=0x00; // data is length: 00 ->don't data 
	outputBuf[5]=0xFE; // frame of end code -> 0xFE.
    outputBuf[6] = bcc_check(outputBuf,6);


	transferSize=7;
	usart1_dma_send(outputBuf,transferSize);
		// if(transferSize)
		// {
		// 	while(transOngoingFlag);
		// 	transOngoingFlag=1;
		// //	HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
		// }
	
}

/*********************************************************
 * 
 * Function Name:void SendData_Temp_Data(uint8_t tdata)
 * Function:send temperature value 
 * 
*********************************************************/
void SendData_Temp_Data(uint8_t tdata)
{

    outputBuf[0]=0xA5; //display board head = 0xA5
	outputBuf[1]= 0x01; //display device Number:is 0x01
	outputBuf[2]=0x1A; // command type = 0x1A -> temperature of value 
	outputBuf[3]=0x0f; // command order -> 0x0f -> is data , don't order.
	outputBuf[4]=0x01; // data is length: 00 ->don't data 
	outputBuf[5]=tdata; // frame of end code -> 0xFE.
	outputBuf[6]=0xFE; // frame of end code -> 0xFE.
    outputBuf[7] = bcc_check(outputBuf,7);
		
	transferSize=8;
	usart1_dma_send(outputBuf,transferSize);
		// if(transferSize)
		// {
		// 	while(transOngoingFlag);
		// 	transOngoingFlag=1;
		// 	//HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
		// }

}
/*********************************************************
 * 
 * Function Name:void SendData_Time_Data(uint8_t tdata)
 * Function:send to set  timer timing value
 * 
*********************************************************/
void SendData_Time_Data(uint8_t tdata)
{

        outputBuf[0]='T'; //4D
		outputBuf[1]='K'; //58
		outputBuf[2]='T'; //"T"->temperature
		outputBuf[3]=tdata; //53	//
		
		transferSize=4;
		usart1_dma_send(outputBuf,transferSize);
		// if(transferSize)
		// {
		// 	while(transOngoingFlag);
		// 	transOngoingFlag=1;
		// 	//HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
		// }

}

/********************************************************************************
    **
    *Function Name
    *Function : commad order , data -command type
    *Input Ref: commad order , data -command type
    *Return Ref:NO
    *
*******************************************************************************/
void SendWifiData_Answer_Cmd(uint8_t cmd ,uint8_t data)
{
        outputBuf[0]=0x5A; //display board head = 0xA5
        outputBuf[1]=0x01; //display device Number:is 0x01
        outputBuf[2]=0xFF; // answer or copy command
        outputBuf[3]= cmd; // 0x0F : is data ,don't command order.
        outputBuf[4]= data; // don't data ,onlay is command order,recieve data is 1byte .
       
        outputBuf[5] = 0xFE; //frame is end of byte.
        outputBuf[6] = bcc_check(outputBuf,6);
        
        transferSize=7;
		usart1_dma_send(outputBuf,transferSize);
        // if(transferSize)
        // {
        //     while(transOngoingFlag); //UART interrupt transmit flag ,disable one more send data.
        //     transOngoingFlag=1;
        //    // HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
        // }
	
}


/********************************************************************************
**
*Function Name:void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
*Function :UART callback function  for UART interrupt for transmit data
*Input Ref: structure UART_HandleTypeDef pointer
*Return Ref:NO
*
*******************************************************************************/
//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
//{
////	if(huart==&huart1)
////	{
////		transOngoingFlag=0; //UART Transmit interrupt flag =0 ,RUN
////	}
//	
//}

