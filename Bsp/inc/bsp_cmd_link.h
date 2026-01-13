#ifndef __BSP_CMD_LINK_H_
#define __BSP_CMD_LINK_H_

#include "main.h"
#include "bsp.h"

#define  MAX_BUFFER_SIZE   48
#define  RECEIVE_DATA_USART1  24

typedef enum PROTOCOL{

  //cmd:
  protocol_power = 0x01,
  protocol_ptc_cmd =0x02,
  protocol_plasma_cmd=0x03,
  protocol_ultrasonic_cmd=0x04,
  protocol_wifi_cmd=0x05,
  protocol_buzzer_cmd=0x06,
  protocol_ai_cmd=0x07,
  protocol_warning_ptc_cmd=0x08,
  protocol_warning_fan_cmd=0x09,

  protocol_answer_power_cmd=0x0B, //main board ansower POWER ON or OFF.

  protocol_buzzer_answer_cmd=0x16,   //has a answer cmd with mainboard return answer order.
  protocol_read_temp_value=0x1A,  //read temperature value .
  protocol_read_humidity_value=0x1B,  //read humidity value 
  protocol_read_time_value=0x1C,  //read time--hours,minutes,sencod.
  protocol_read_data_value=0x1D,  //read  year,month,week,data.
  protocol_read_fan_value=0x1E, //read fan speed value 
  protocol_read_wiifi_value=0x1F, //read wifi link net state

  protocol_app_transfer_cmd=0x20,
  protocol_app_power_cmd=0x21,
  portocol_notice_ptc_cmd= 0x22,
  protocol_app_ai_cmd=0x27,
  protocol_set_temp_value=0x2A,

  protocol_answer_cmd=0xFF, // cmd of copy return send as the same command.

 
  
}protocol_t;



//extern uint8_t inputBuf[MAX_BUFFER_SIZE];
extern uint8_t outputBuf[8];


void SendData_PowerOnOff(uint8_t index);



void SendData_Set_Command(uint8_t cmd,uint8_t hdata);
void SendData_Temp_Data(uint8_t tdata);
void SendData_Time_Data(uint8_t tdata);
void SendWifiData_Answer_Cmd(uint8_t cmd ,uint8_t data);

void SendData_twoHours_Data(uint8_t tdata);

void SendData_Buzzer_Has_Ack(void);


void SendData_Buzzer(void);


void USART1_Cmd_Error_Handler(void);

void SendCmd_fun(uint8_t cmd,uint8_t data);

void sendNotice_toMainBoard(uint8_t notice,uint8_t tdata);

void SendData_Data(uint8_t cmd ,uint8_t tdata);


#endif 




