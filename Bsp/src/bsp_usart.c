#include "bsp.h"

// 为协议中的魔术字节定义常量，提高可读性
#define FRAME_HEADER          0x5A        //main board header  
#define FRAME_NUM           0x10          //main deviece number is 0x10    
#define FRAME_ACK_NUM       0x80          //from main answer singnal 0x80 new version . 
#define FRAME_END_BYTE              0xFE
#define DATA_FRAME_TYPE_INDICATOR   0x0F
#define FRAME_COPY_NUM              0xFF   //this is older version .

#define ACK_SUCCESS 0x00U
#define ACK_FAILURE 0x01U

//extern QueueHandle_t xUartRxQueue = NULL;

typedef enum {
    UART_STATE_WAIT_HEADER = 0,
    UART_STATE_NUM=1,
    UART_STATE_CMD_NOTICE=2,
    UART_STATE_EXEC_CMD_OR_LEN=3,
	 UART_STATE_FRAME_CMD_0X0=4,
    UART_STATE_FRAME_END=5,
    UART_STATE_BCC_CHECK,
    UART_STATE_DATA_LEN,
    UART_STATE_DATA,
    UART_STATE_DATA_END,
    UART_STATE_DATA_BCC
} uart_parse_state_t;

typedef enum{

    power_on_off=1,
    ptc_on_off=2,
    plasma_on_off=3,
    ultrasonic_on_off=4,
    wifi_link=5,
    buzzer_sound=6,
    ai_mode=7,
    temp_high_warning=8,
    fan_warning=9,
    fan_on_off = 0x0B,

     //notice no sound 
    ack_power_on_off = 0x10,
    ack_ptc_on_off = 0x12,
    ack_plasma_on_ff= 0x13,
    ack_ultrasonic_on_off = 0x14,
}signal_parase_t;

typedef struct Msg
{
    uint8_t   tx_data_success;
	uint8_t   cmd_notice;
	uint8_t   copy_cmd_notice;
    uint8_t   execuite_cmd_notice;
	uint8_t   rx_data_flag;
    uint8_t   bcc_check_code;
    uint8_t   receive_data_length;
    uint8_t   data_length;
	uint8_t   rc_data_length;
	uint8_t   rx_data[4];
	uint8_t   usData[12];

}MSG_T;

MSG_T   gl_tMsg; 

 static void receive_cmd_or_data_handler(void);
 static void receive_copy_cmd_or_data_handler(void);



 volatile uint8_t rx_data_counter=0;

/********************************************************************************
	**
	*Function Name:void usart1_isr_callback_handler(void)
	*Function :  this is receive data from mainboard.
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
#if 1
void usart1_isr_callback_handler(uint8_t data)
{
     static uint8_t state;
    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	 inputBuf[0] = data;
	 //if(gl_tMsg.tx_data_success ==0){
     switch(state)
		{
		case UART_STATE_WAIT_HEADER:  //#0
			if(inputBuf[0] == FRAME_HEADER){  // 0x5A --main board singla
               rx_data_counter=0;
               gl_tMsg.usData[rx_data_counter] = inputBuf[0];
				state=UART_STATE_NUM; //=1
				gl_tMsg.copy_cmd_notice=0;
				 gl_tMsg.cmd_notice=0;

             }
            
		break;

        case UART_STATE_NUM: //0x01

             if(inputBuf[0] == FRAME_NUM ||inputBuf[0] == FRAME_ACK_NUM){  // 0x5A --main board singla or copy cmd
               rx_data_counter++;
               gl_tMsg.usData[rx_data_counter] = inputBuf[0];
			   if(inputBuf[0] == FRAME_ACK_NUM){
                  gl_tMsg.copy_cmd_notice  = 0x80; //new version protocol is copy cmd notice.
                   state=UART_STATE_CMD_NOTICE; //=1
			   }
			   else{ 
			   	  gl_tMsg.copy_cmd_notice = 0;
			      state=UART_STATE_CMD_NOTICE; //=1
			   	}

            }
            else{
                state=0;
                rx_data_counter=0;
            }

        break;

        case UART_STATE_CMD_NOTICE://2 -> odler version 1. 0xFF ->COPY CMD
               rx_data_counter++;
               gl_tMsg.usData[rx_data_counter] = inputBuf[0];
			   gl_tMsg.cmd_notice = inputBuf[0];
			   if(gl_tMsg.cmd_notice == 0xFF){//this is older version protocol 0x02 -> copy command.
				   gl_tMsg.copy_cmd_notice = 0xFF;
				   gl_tMsg.cmd_notice=0;
				   state=UART_STATE_EXEC_CMD_OR_LEN; //1
			   }
			   else{
                 state=UART_STATE_EXEC_CMD_OR_LEN; //1
			   }
    
        break;

        case UART_STATE_EXEC_CMD_OR_LEN://3 -> 1. execuite cmd or ontice . 2. copy cmd or notice .3. data id = 0x0F
            rx_data_counter++;
            gl_tMsg.usData[rx_data_counter] = inputBuf[0];
            if(gl_tMsg.usData[rx_data_counter] !=0x0F && gl_tMsg.copy_cmd_notice != 0xFF){
                gl_tMsg.execuite_cmd_notice =  gl_tMsg.usData[rx_data_counter];
				gl_tMsg.rx_data_flag =  0x0;
				gl_tMsg.copy_cmd_notice = 0;
                state = UART_STATE_FRAME_CMD_0X0;

            }
            else if(gl_tMsg.usData[rx_data_counter] ==0x0F){ //is data frame
               gl_tMsg.rx_data_flag =  0x0F;
			   gl_tMsg.copy_cmd_notice = 0;
               state = UART_STATE_DATA_LEN; //receive data.
           }
		   else if(gl_tMsg.copy_cmd_notice == 0xFF){ //this is older compatibility 
		        gl_tMsg.cmd_notice = gl_tMsg.usData[rx_data_counter];
				gl_tMsg.rx_data_flag =  0x0;
                state = UART_STATE_DATA_LEN; //receive data.
            }


        break;

		case UART_STATE_FRAME_CMD_0X0: //0x04 //receive comd and notice frame  0x00
			rx_data_counter++;
			gl_tMsg.usData[rx_data_counter] = inputBuf[0];
			if(inputBuf[0] == 0x0){  // frame is tail of end "0xFE"

				state=UART_STATE_FRAME_END; //=1

			}
			else{
				state=0;
				rx_data_counter=0;
			}

		break;

    case  UART_STATE_FRAME_END://5 //receive comd and notice frame  end
            rx_data_counter++;
            gl_tMsg.usData[rx_data_counter] = inputBuf[0];
            if(inputBuf[0] == 0xFE){  // frame is tail of end "0xFE"
             
			   state=UART_STATE_BCC_CHECK; //=1

             }
			 else{
                state=0;
                rx_data_counter=0;
			 }
         

        break;


        case UART_STATE_BCC_CHECK: //frem end bcc check code
            rx_data_counter++;
            gl_tMsg.usData[rx_data_counter] = inputBuf[0];
	        gl_tMsg.bcc_check_code =  gl_tMsg.usData[rx_data_counter];
			gl_tMsg.data_length = rx_data_counter;
	        if(gl_tMsg.bcc_check_code == bcc_check(gl_tMsg.usData, gl_tMsg.data_length))
	        {
	                state=0;
	                rx_data_counter=0; 
					// gl_tMsg.tx_data_success = 1;
					// memset(gl_tMsg.usData,0,(gl_tMsg.data_length+1));

	                // xtask_decoder_task_isr_handler();
				 gl_tMsg.usData[0] = 0;
				 gl_tMsg.usData[1] = 0;
				 parse_recieve_data_handler();

	        }
	        else{
	                state=0;
	                rx_data_counter=0;
					 gl_tMsg.usData[0] = 0;
				    gl_tMsg.usData[1] = 0;
	        }
			

        break;


        //this is receive data or copy command or notice
		case UART_STATE_DATA_LEN: //receive is data ->"0x04"

             rx_data_counter++;
             gl_tMsg.usData[rx_data_counter] = inputBuf[0];

              gl_tMsg.receive_data_length = gl_tMsg.usData[rx_data_counter];
             
           // 根据数据长度判断是否需要接收载荷
            if(gl_tMsg.receive_data_length > 0 && gl_tMsg.copy_cmd_notice != 0xFF) {
				  gl_tMsg.data_length=0;
                 state = UART_STATE_DATA;
            } 
            else if(gl_tMsg.copy_cmd_notice == 0xFF){ //this is older compatibility copy command
                 gl_tMsg.execuite_cmd_notice =  gl_tMsg.usData[rx_data_counter];
				 state = UART_STATE_FRAME_END; //receive data.
			}
			else {
                // 如果数据长度为0，直接跳到帧尾
                rx_data_counter=0;
                state = 0;
            }
        break;

        case UART_STATE_DATA:

        rx_data_counter++;
        gl_tMsg.usData[rx_data_counter] = inputBuf[0];
		gl_tMsg.rx_data[gl_tMsg.data_length]=inputBuf[0]; //receive data be save rx_data[]
		 gl_tMsg.data_length ++;
		//gl_tMsg.rc_data_length++;
         
        if(gl_tMsg.data_length >= gl_tMsg.receive_data_length){
              
             //state = UART_STATE_DATA_END;
			 state = UART_STATE_FRAME_END;

        }
		else{
			state=UART_STATE_DATA;
		}


        break;

      
     	}

}


#endif 
/********************************************************************************
	**
	*Function Name:void usart1_isr_callback_handler(void)
	*Function : parse this is receive data from mainboard.
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
void parse_recieve_data_handler(void)
{
  
    
	switch(gl_tMsg.copy_cmd_notice){ //cmd or notice .

	case 0:
      
       receive_cmd_or_data_handler();
	  // gl_tMsg.tx_data_success = 0;

   break;

   case 0x0FF: //copy cmd or notice,this is older version protocol.
		receive_copy_cmd_or_data_handler();
		// gl_tMsg.tx_data_success = 0;

   case 0x80:
       receive_copy_cmd_or_data_handler();
//gl_tMsg.tx_data_success = 0;
   break;

    }
 }

/**
* @brief receive cmd or data from mainboard .
* @note  处理 ACK 帧
* @param
* @return 
*
*/
static void receive_cmd_or_data_handler(void)
{


	switch(gl_tMsg.cmd_notice){
	case power_on_off:

	if(gl_tMsg.execuite_cmd_notice == 0x01){//power on
		//run_t.power_on = power_on;
		}
		else{//power off 
		//run_t.power_on = power_off;

		}
	break;

	case ptc_on_off: //PTC 
	if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
		run_t.dry = open;

		}
		else{//power off 
		run_t.dry = close;
	}

	break;

	case plasma_on_off://plasma
	if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
		run_t.plasma = open;

		}
		else{//power off 
		run_t.plasma = close;
	}

	break;

	case ultrasonic_on_off:
	if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
		run_t.ultrasonic = open;

		}
		else{//power off 
		run_t.ultrasonic = close;
	}

	break;

	case fan_on_off:
	if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
		run_t.fan = open;

		}
		else{//power off 
		run_t.fan = close;
	}

	break;


	case 0x08: //temperature of high warning.

	if(gl_tMsg.execuite_cmd_notice == 0x01){  //warning 

		run_t.ptc_warning = 1;
		//run_t.setup_timer_timing_item =  PTC_WARNING; //ptc warning 
		run_t.display_set_timer_or_works_time_mode = PTC_WARNING;

		run_t.dry=0;
		SendData_Set_Command(0x22,0x0); //close ptc ,but don't buzzer sound .

	}
	else if(gl_tMsg.execuite_cmd_notice== 0x0){ //close 

		run_t.ptc_warning = 0;


	}



	break;

	case 0x09: //fan of default of warning.

	if(gl_tMsg.execuite_cmd_notice == 0x01){  //warning 

		run_t.fan_warning = 1;

		run_t.display_set_timer_or_works_time_mode =FAN_WARNING;  //run_t.display_set_timer_or_works_time_mode

		run_t.dry =0;
		SendData_Set_Command(0x22,0x0); //close ptc ,but don't buzzer sound .

	}
	else if(gl_tMsg.execuite_cmd_notice == 0x0){ //close 

		run_t.fan_warning = 0;
	}

	break;

	case 0x1A: //娓╁害鏁版嵁

	if(gl_tMsg.receive_data_length == 0x02){ //鏁版嵁

	    gpro_t.humidity_real_value = gl_tMsg.rx_data[0];
		gpro_t.temp_real_value = gl_tMsg.rx_data[1];

		//humidity_value 

		//hum1 =  gpro_t.humidity_real_value/ 10;
		//hum2 =  gpro_t.humidity_real_value % 10;

		lcd_t.number3_low= gpro_t.humidity_real_value/ 10;
		lcd_t.number3_high =gpro_t.humidity_real_value/ 10;

		lcd_t.number4_low = gpro_t.humidity_real_value % 10;
		lcd_t.number4_high = gpro_t.humidity_real_value % 10;

	//temperature_value 
	//  temp = pdata[6];
	//temp1 =   gpro_t.temp_real_value/ 10;
	//temp2   = gpro_t.temp_real_value% 10;

	lcd_t.number1_low= gpro_t.temp_real_value/ 10;
	lcd_t.number1_high =gpro_t.temp_real_value/ 10;

	lcd_t.number2_low =  gpro_t.temp_real_value% 10;
	lcd_t.number2_high =  gpro_t.temp_real_value% 10;

	power_on_display_temp_handler(); //WT.EDIT 2025.03.28

	}
	
	break;

	case 0x1C: //琛ㄧず鏃堕棿锛氬皬鏃讹紝鍒嗭紝绉�

	if(gl_tMsg.receive_data_length == 0x03){ //鏁版嵁

	if(gl_tMsg.rx_data[0] < 24){ //WT.EDIT 2024.11.23

	lcd_t.display_beijing_time_flag= 1;

	run_t.dispTime_hours  =  gl_tMsg.rx_data[0];
	run_t.dispTime_minutes = gl_tMsg.rx_data[1];
	run_t.gTimer_disp_time_seconds =  gl_tMsg.rx_data[2];
	}


	}
	break;

	case 0x1E: //fan of speed is value 

	if( gl_tMsg.rx_data[0] < 34){
		run_t.wifi_link_net_success=1;
		run_t.disp_wind_speed_grade = 10;


	}
	else if( gl_tMsg.rx_data[0] < 67 &&  gl_tMsg.rx_data[0] > 33){
		run_t.wifi_link_net_success=1;
		run_t.disp_wind_speed_grade = 60;

	}
	else if( gl_tMsg.rx_data[0] > 66){
		run_t.wifi_link_net_success=1;
		run_t.disp_wind_speed_grade =100;
	}



	break;

	case 0x1F: //link wifi if success data 

	if(gl_tMsg.rx_data[0] == 0x01){  // link wifi 

		run_t.wifi_link_net_success =1 ;      

	}
	else if(gl_tMsg.rx_data[0] == 0x0){ //don't link wifi 

		run_t.wifi_link_net_success =0 ;     

	}



	break;

	case 0x27 : //AI mode by smart phone of APP be control.

	if(gl_tMsg.execuite_cmd_notice==2){
		//timer time + don't has ai item
		run_t.display_set_timer_or_works_time_mode = timer_time;
		run_t.gTimer_again_switch_works = 0; //WT.EDIT ,if don't define timer_time,wait 3s switch to works_time.
		run_t.gModel=0;
	}
	else{
		//beijing time + ai item
		run_t.display_set_timer_or_works_time_mode = works_time;

		run_t.gModel=1;

	}


	break;




	//smart phone data
	case 0x31: //smart phone app timer that power on command,is normal power on and off

	if(gl_tMsg.execuite_cmd_notice == 0x01){ //open
		run_t.wifi_link_net_success=1;
		SendWifiData_Answer_Cmd(0x31,0x01);
		xTask_PowerOn_Handler() ; 

	}
	else if(gl_tMsg.execuite_cmd_notice == 0x0){ //close 
		run_t.wifi_link_net_success=1;
		SendWifiData_Answer_Cmd(0x031,0x0);
		xTask_PowerOff_Handler() ; 


	}

	break;

	case 0x21: //smart phone power on or off that App timer .
	if(gl_tMsg.execuite_cmd_notice==0x01){ //power on by smart phone APP
		gpro_t.smart_phone_app_timer_power_on_flag =1;
		run_t.wifi_link_net_success=1;

		xTask_PowerOn_Handler() ; 

	}
	else{  //power off by smart phone APP
		run_t.wifi_link_net_success=1;
		xTask_PowerOff_Handler() ;     
	}

	break; 

	case 0x3A: // smart phone APP set temperature value 

		run_t.wifi_link_net_success=1;
		gpro_t.smart_phone_turn_off_ptc_flag=0; //smart phone app from setup temperature value .
		run_t.wifi_set_temperature = gl_tMsg.rx_data[0];

		//decade_temp =  run_t.wifi_set_temperature / 10 ;
		//unit_temp =	run_t.wifi_set_temperature % 10; //

		lcd_t.number1_low=run_t.wifi_set_temperature / 10 ;
		lcd_t.number1_high =run_t.wifi_set_temperature / 10 ;

		lcd_t.number2_low = run_t.wifi_set_temperature % 10; //
		lcd_t.number2_high = run_t.wifi_set_temperature % 10; //

		run_t.smart_phone_set_temp_value_flag =1;


	break;
	}

}
/**
* @brief receive copy cmd or data from mainboard .
* @note  处理 ACK 帧
* @param
* @return 
*
*/
static void receive_copy_cmd_or_data_handler(void)
{
    switch(gl_tMsg.cmd_notice){

        case power_on_off:
			
		if(gl_tMsg.execuite_cmd_notice == 0x01){//power on
			run_t.power_on = power_on;
			if(run_t.power_on == power_on){
			
			   gpro_t.receive_copy_cmd  = ok;

			}
			else{
			gpro_t.receive_copy_cmd = ng;
			}

		}
		else{//power off 
			run_t.power_on = power_off;
			if(run_t.power_on == power_off){
			gpro_t.receive_copy_cmd = ok;

			}
			else{
			gpro_t.receive_copy_cmd = ng;
			}

		}


	   break;

	   case  ptc_on_off:
	   	
		if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
			if(run_t.dry == open){
			gpro_t.receive_copy_cmd = ok;

			}
			else{
			gpro_t.receive_copy_cmd = ng;
			}

		}
		else{//power off 
			if(run_t.dry == close){
			gpro_t.receive_copy_cmd = ok;

			}
			else{
			gpro_t.receive_copy_cmd = ng;
			}

		}

	   break;

	   case plasma_on_off:
	   	
		if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
			if(run_t.plasma == open){
			gpro_t.receive_copy_cmd = ok;

			}
			else{
			gpro_t.receive_copy_cmd= ng;
			}

			}
			else{//power off 
			if(run_t.plasma == close){
			gpro_t.receive_copy_cmd = ok;

			}
			else{
			gpro_t.receive_copy_cmd= ng;
			}

			}

	   break;

	   case ultrasonic_on_off:
	   	
		if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
			if(run_t.plasma == open){
			gpro_t.receive_copy_cmd = ok;

			}
			else{
			gpro_t.receive_copy_cmd = ng;
			}

		}
		else{//power off 
			if(run_t.plasma == close){
			gpro_t.receive_copy_cmd = ok;

			}
			else{
			gpro_t.receive_copy_cmd = ng;
			}

		}

	   break;

	   case fan_on_off:

		if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
			if(run_t.fan == open){
			gpro_t.receive_copy_cmd = ok;

			}
			else{
			gpro_t.receive_copy_cmd = ng;
			}

		}
		else{//power off 
			if(run_t.plasma == close){
			gpro_t.receive_copy_cmd = ok;

			}
			else{
			gpro_t.receive_copy_cmd = ng;
			}

		}

	   break;
	   

     


       }
 


}
/**
* @brief receive copy cmd or data from mainboard .
* @note  处理 ACK 帧
* @param
* @return 
*
*/

//bool process_ack_frame(uint8_t *frame, uint8_t len)
//{
//    // 基本长度检查
//    if (len < 6) return false;

//    // 帧头检查
//    if (frame[0] != FRAME_START_BYTE)
//        return false;

//    // 命令字检查（ACK 成功或失败）
//    if (frame[1] != ACK_SUCCESS_CMD && frame[1] != ACK_FAILURE_CMD)
//        return false;

//    uint8_t data_len = frame[2];
//    if (data_len < 1) return false; // 至少有原命令字

//    // 帧尾检查
//    if (frame[3 + data_len] != FRAME_END_BYTE)
//        return false;

//    // BCC 校验
//    uint8_t bcc_calc = calc_bcc(frame, 4 + data_len); // 帧尾之前
//    uint8_t bcc_recv = frame[4 + data_len];
//    if (bcc_calc != bcc_recv)
//        return false;

//    // 数据区解析
//    uint8_t ack_cmd = frame[3]; // 原命令字

//    // 对比原命令
//    if (ack_cmd != last_cmd)
//    {
//        // 命令不匹配
//        return false;
//    }

//    // 判断 ACK 成功/失败
//    if (frame[1] == ACK_SUCCESS_CMD)
//    {
//        // ✅ 应答正确
//        return true;
//    }
//    else
//    {
//        // ❌ 应答失败
//        return false;
//    }
//}

// 假设这些在你的主文件中已经定义
//extern QueueHandle_t xUartRxQueue;
//extern uint8_t inputBuf[1];

/**
 * @brief  UART中断服务程序（ISR）
 * @param  data 接收到的数据字节
 */
//void usart1_isr_callback_handler(uint8_t data) {
//   app_xusart1_queue_isr_handler(data);
//}



