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

#define NEW_BUF_SIZE 11 // 需要复制的字节数 


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
    UART_STATE_DATA_BCC,
    UART_STATE_FRAME_DATA_END,
    UART_STATE_DATA_BCC_CHECK
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
    wifi_power_off = 0x31, //WT.EDIT 2025.12.15

     //notice no sound 
    ack_power_on_off = 0x10,
    ack_ptc_on_off = 0x12,
    ack_plasma_on_ff= 0x13,
    ack_ultrasonic_on_off = 0x14,
}signal_parase_t;

typedef struct Msg
{
    
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

// BCC校验函数
uint8_t bcc_check(const unsigned char *data, int len) {
    unsigned char bcc = 0;
    for (int i = 0; i < len; i++) {
        bcc ^= data[i];
    }
    return bcc;
}

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
	 //inputBuf[0] = data;
	 memcpy(inputBuf,rx_buf,sizeof(rx_buf)) ;       
	
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

             if(inputBuf[1] == FRAME_NUM ||inputBuf[0] == FRAME_ACK_NUM){  // 0x5A --main board singla or copy cmd
               rx_data_counter++;
               gl_tMsg.usData[rx_data_counter] = inputBuf[rx_data_counter];
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
				gl_tMsg.usData[0]=0;
				gl_tMsg.usData[1]=1;
            }

        break;

        case UART_STATE_CMD_NOTICE://2 -> odler version 1. 0xFF ->COPY CMD
               rx_data_counter++;
               gl_tMsg.usData[rx_data_counter] = inputBuf[rx_data_counter];
			   gl_tMsg.cmd_notice = inputBuf[0];
			   if(gl_tMsg.cmd_notice == 0xFF){//this is older version protocol 0x02 -> copy command.
				   gl_tMsg.copy_cmd_notice = 0xFF;
				   gl_tMsg.cmd_notice=0;
				   state=UART_STATE_EXEC_CMD_OR_LEN; //1
			   }
			   else{
			   	 gl_tMsg.copy_cmd_notice = 0;
                 state=UART_STATE_EXEC_CMD_OR_LEN; //1
			   }
    
        break;

        case UART_STATE_EXEC_CMD_OR_LEN://3 -> 1. execuite cmd or ontice . 2. copy cmd or notice .3. data id = 0x0F
            rx_data_counter++;
            gl_tMsg.usData[rx_data_counter] = inputBuf[rx_data_counter];
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
                state = UART_STATE_FRAME_CMD_0X0;; //receive data.
            }


        break;

		case UART_STATE_FRAME_CMD_0X0: //0x04 //receive comd and notice frame  0x00
			rx_data_counter++;
			gl_tMsg.usData[rx_data_counter] = inputBuf[rx_data_counter];
			if(inputBuf[0] == 0x0){  // frame is tail of end "0xFE"

				state=UART_STATE_FRAME_END; //=1

			}
			else if(gl_tMsg.copy_cmd_notice == 0xFF){
			    gl_tMsg.execuite_cmd_notice =  gl_tMsg.usData[rx_data_counter];
			    state=UART_STATE_FRAME_END;


			}
			else{
				state=0;
				rx_data_counter=0;
				gl_tMsg.usData[0]=0;
				gl_tMsg.usData[1]=1;
				gl_tMsg.usData[2]=0;
				gl_tMsg.usData[3]=1;
			}

		break;

    case  UART_STATE_FRAME_END://5 //receive comd and notice frame  end
            rx_data_counter++;
            gl_tMsg.usData[rx_data_counter] = inputBuf[rx_data_counter];
            if(inputBuf[0] == 0xFE){  // frame is tail of end "0xFE"
             
			   state=UART_STATE_BCC_CHECK; //=1

             }
			 else{
                state=0;
                rx_data_counter=0;
				gl_tMsg.usData[0]=0;
				gl_tMsg.usData[1]=1;
				gl_tMsg.usData[2]=0;
				gl_tMsg.usData[3]=1;
				gl_tMsg.usData[4]=0;
			 }
         

        break;


        case UART_STATE_BCC_CHECK: //frem end bcc check code
            rx_data_counter++;
            gl_tMsg.usData[rx_data_counter] = inputBuf[rx_data_counter];
	        gl_tMsg.bcc_check_code =  gl_tMsg.usData[rx_data_counter];
			gl_tMsg.data_length = rx_data_counter;
	        //if(gl_tMsg.bcc_check_code == bcc_check(gl_tMsg.usData, gl_tMsg.data_length))
	       // {
	                state=0;
	                rx_data_counter=0; 
					 gpro_t.decoder_flag = 1;
					// memset(gl_tMsg.usData,0,(gl_tMsg.data_length+1));
               
	               // xtask_decoder_task_isr_handler();
				// gl_tMsg.usData[0] = 0;
				 //gl_tMsg.usData[1] = 0;
				 ///parse_recieve_data_handler();

	       // }
	        // else{
	        //         state=0;
	        //         rx_data_counter=0;
			// 		gl_tMsg.usData[0]=0;
			// 		gl_tMsg.usData[1]=1;
			// 		gl_tMsg.usData[2]=0;
			// 		gl_tMsg.usData[3]=1;
			// 		gl_tMsg.usData[4]=0;
	        // }
			

        break;


        //this is receive data or copy command or notice
		case UART_STATE_DATA_LEN: //receive is data ->"0x04"

             rx_data_counter++;
             gl_tMsg.usData[rx_data_counter] = inputBuf[rx_data_counter];

              gl_tMsg.receive_data_length = gl_tMsg.usData[rx_data_counter];
             
           // 根据数据长度判断是否需要接收载荷
            if(gl_tMsg.receive_data_length > 0 && gl_tMsg.copy_cmd_notice != 0xFF){
				  gl_tMsg.data_length=0;
                 state = UART_STATE_DATA;
            } 
			else {
                // 如果数据长度为0，直接跳到帧尾
                rx_data_counter=0;
                state = 0;
            }
        break;

        case UART_STATE_DATA:

        rx_data_counter++;
        gl_tMsg.usData[rx_data_counter] = inputBuf[rx_data_counter];
		gl_tMsg.rx_data[gl_tMsg.data_length]=inputBuf[rx_data_counter]; //receive data be save rx_data[]
		 gl_tMsg.data_length ++;
		//gl_tMsg.rc_data_length++;
         
        if(gl_tMsg.data_length >= gl_tMsg.receive_data_length){
              
             //state = UART_STATE_DATA_END;
			 state = UART_STATE_FRAME_DATA_END;

        }
		else{
			state=UART_STATE_DATA;
		}


        break;

		case UART_STATE_FRAME_DATA_END:

		    rx_data_counter++;
            gl_tMsg.usData[rx_data_counter] = inputBuf[rx_data_counter];
            if(inputBuf[0] == 0xFE){  // frame is tail of end "0xFE"
             
			   state=UART_STATE_BCC_CHECK; //=1

             }
			 else{
                state=0;
                rx_data_counter=0;
				gl_tMsg.usData[0]=0;
				gl_tMsg.usData[1]=1;
				gl_tMsg.usData[2]=0;
				gl_tMsg.usData[3]=1;
				gl_tMsg.usData[4]=0;
			 }


		break;

	     case UART_STATE_DATA_BCC_CHECK: //frem end bcc check code
            rx_data_counter++;
            gl_tMsg.usData[rx_data_counter] = inputBuf[rx_data_counter];
	        gl_tMsg.bcc_check_code =  gl_tMsg.usData[rx_data_counter];
			gl_tMsg.data_length = rx_data_counter;
	        //if(gl_tMsg.bcc_check_code == bcc_check(gl_tMsg.usData, gl_tMsg.data_length))
	       // {
	                state=0;
	                rx_data_counter=0; 
					
					// memset(gl_tMsg.usData,0,(gl_tMsg.data_length+1));
                    gpro_t.decoder_flag = 1;
	              //  xtask_decoder_task_isr_handler();
				// gl_tMsg.usData[0] = 0;
				 //gl_tMsg.usData[1] = 0;
				 ///parse_recieve_data_handler();

	       // }
	        // else{
	        //         state=0;
	        //         rx_data_counter=0;
			// 		gl_tMsg.usData[0]=0;
			// 		gl_tMsg.usData[1]=1;
			// 		gl_tMsg.usData[2]=0;
			// 		gl_tMsg.usData[3]=1;
			// 		gl_tMsg.usData[4]=0;
	        // }
			

        break;

      
     	}

}


#endif 
void clear_rx_buff(void)
{
	gl_tMsg.usData[0] = 0;
	gl_tMsg.usData[1] = 0;
	//gl_tMsg.usData[2] = 0;
	//gl_tMsg.usData[3] = 0;
    gl_tMsg.usData[6] = 0;

}
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
  
  // if(gl_tMsg.bcc_check_code == bcc_check(gl_tMsg.usData, gl_tMsg.data_length)){
	switch(gl_tMsg.copy_cmd_notice){ //cmd or notice .

	case 0:
      
       receive_cmd_or_data_handler();
	   clear_rx_buff();
	   memset(rx_buf,0,sizeof(rx_buf));
	 

   break;

  

   case 0x80:


   case 0xFF: //copy cmd or notice,this is older version protocol.
		receive_copy_cmd_or_data_handler();
		clear_rx_buff();
         memset(rx_buf,0,sizeof(rx_buf));

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
			run_t.ptc_on_off_flag = 0;
			SendData_Set_Command(0x12,0x01); //close ptc 
			osDelay(3);
			gpro_t.gTimer_copy_cmd_counter=0; 
			gpro_t.receive_copy_buff[2]=copy_null;
			Display_Kill_Dry_Ster_Icon();
		}
		else{//power off 
			run_t.dry = close;
			run_t.ptc_on_off_flag = 1;
			SendData_Set_Command(0x12,0x0); //close ptc 
		    osDelay(3);
			gpro_t.gTimer_copy_cmd_counter=0; 
	        gpro_t.receive_copy_buff[2]=copy_null;
			Display_Kill_Dry_Ster_Icon();
	    }

	break;

	case plasma_on_off://plasma
	if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
		run_t.plasma = open;
		SendData_Set_Command(0x13,0x01); //close ptc 
		 osDelay(3);
		 gpro_t.gTimer_copy_cmd_counter=0; 
	     gpro_t.receive_copy_buff[3]=copy_null;

		}
		else{//power off 
		run_t.plasma = close;
		SendData_Set_Command(0x13,0x0); //close ptc 
		 osDelay(3);
		 gpro_t.gTimer_copy_cmd_counter=0; 
	     gpro_t.receive_copy_buff[3]=copy_null;
	}

	break;

	case ultrasonic_on_off:
	if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
		run_t.ultrasonic = open;
		SendData_Set_Command(0x14,0x01); //close ptc 
		 osDelay(3);
		 gpro_t.gTimer_copy_cmd_counter=0; 
	     gpro_t.receive_copy_buff[4]=copy_null;

		}
		else{//power off 
		run_t.ultrasonic = close;
		SendData_Set_Command(0x13,0x0); //close ptc 
		 osDelay(3);
		 gpro_t.gTimer_copy_cmd_counter=0; 
	     gpro_t.receive_copy_buff[4]=copy_null;
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
		//SendData_Set_Command(0x08,0x01); //high temperatue warning .

		SendWifiData_Answer_Cmd(0x08,0x01);
	   osDelay(5);

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
		//SendData_Set_Command(0x09,0x01); //
		SendWifiData_Answer_Cmd(0x09,0x01);
		osDelay(5);

	}
	else if(gl_tMsg.execuite_cmd_notice == 0x0){ //close 

		run_t.fan_warning = 0;
	}

	break;

	case 0x1A: //read sensor "DHT11" temperature and humidity value .



	    gpro_t.humidity_real_value = gl_tMsg.rx_data[0];
		gpro_t.temp_real_value = gl_tMsg.rx_data[1];

		

		lcd_t.number3_low= gpro_t.humidity_real_value/ 10;
		lcd_t.number3_high =gpro_t.humidity_real_value/ 10;

		lcd_t.number4_low = gpro_t.humidity_real_value % 10;
		lcd_t.number4_high = gpro_t.humidity_real_value % 10;



	     lcd_t.number1_low= gpro_t.temp_real_value/ 10;
	     lcd_t.number1_high =gpro_t.temp_real_value/ 10;

	     lcd_t.number2_low =  gpro_t.temp_real_value% 10;
	     lcd_t.number2_high =  gpro_t.temp_real_value% 10;

	     power_on_display_temp_handler(); //WT.EDIT 2025.03.28

	
	
	break;

	case 0x1C: //time is hours,minutes,seconds value .

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

	case 0x1F: //link wifi if success data don't command and notice.

	if(gl_tMsg.rx_data[0] == 0x01){  // link wifi 

		run_t.wifi_link_net_success =1 ;      

	}
	else{ //don't link wifi 

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
	case 0x20: //new version smart phone normal power on and off command.

	case 0x31: //smart phone app timer that power on command,is normal power on and off

	if(gl_tMsg.execuite_cmd_notice == 0x01){ //open
		run_t.wifi_link_net_success=1;
		gpro_t.phone_power_on_flag = 1;
		SendWifiData_Answer_Cmd(0x31,0x01);
	    vTaskDelay(5);
	   
		//xTask_PowerOn_Handler() ; 

	}
	else if(gl_tMsg.execuite_cmd_notice == 0x0){ //close 
		run_t.wifi_link_net_success=1;
		 gpro_t.phone_power_on_flag = 2;
		SendWifiData_Answer_Cmd(0x031,0x0);
		vTaskDelay(5);
		//xTask_PowerOff_Handler() ; 
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

	case 0x2A: //new version main board or smart phone app set temperature value

	case 0x3A: // smart phone APP set temperature value 

		run_t.wifi_link_net_success=1;
	    run_t.ptc_on_off_flag = 0; //WT.EDIT 2025.10.31
	   
        gpro_t.smart_phone_turn_off_ptc_flag=0; //smart phone app from setup temperature value .
		run_t.wifi_set_temperature = gl_tMsg.rx_data[0];

		//decade_temp =  run_t.wifi_set_temperature / 10 ;
		//unit_temp =	run_t.wifi_set_temperature % 10; //

		lcd_t.number1_low=run_t.wifi_set_temperature / 10 ;
		lcd_t.number1_high =run_t.wifi_set_temperature / 10 ;

		lcd_t.number2_low = run_t.wifi_set_temperature % 10; //
		lcd_t.number2_high = run_t.wifi_set_temperature % 10; //

		gpro_t.gTimer_set_temp_times=0;//WT.EDIT 2025.10.31
		gpro_t.temp_key_set_value =1;
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
		
		
			gpro_t.send_ack_cmd=0;

			

		}
		else if(run_t.power_on == power_off){
	

			   gpro_t.send_ack_cmd=0;

		}


	   break;

	   case  ptc_on_off:
	   	
		if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
		
				gpro_t.send_ack_cmd=0;
			
		}
		else {
			
			 gpro_t.send_ack_cmd=0;
              
		 }

		

	   break;

	   case plasma_on_off:
	   	
		if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
				gpro_t.send_ack_cmd=0;
			
			}
			else {
				gpro_t.send_ack_cmd=0;

			}

	   break;

	   case ultrasonic_on_off:
	   	
		if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
			gpro_t.send_ack_cmd=0;

		}
		else {
		   gpro_t.send_ack_cmd=0;
		}

	   break;

	   case wifi_link:
	   	
	      if(gl_tMsg.execuite_cmd_notice ==1){
               gpro_t.send_ack_cmd=0;
			   power_key_long_fun();
   
		  }
	   
	  break;

	   case fan_on_off:

		if(gl_tMsg.execuite_cmd_notice == 0x01){//ptc on
			if(run_t.fan == open){
			  gpro_t.receive_copy_buff[11]  =copy_ok;

			}
			else{
			gpro_t.receive_copy_buff[11]  =copy_ng;
			}

		}
		else{//power off 
			if(run_t.plasma == close){
			gpro_t.receive_copy_buff[11]  =copy_ok;

			}
			else{
			gpro_t.receive_copy_buff[11]  =copy_ng;
			}

		}

	   break;


	   case 0x1C:

	       if(gl_tMsg.execuite_cmd_notice == 0x01){
	   	
                 gpro_t.send_ack_cmd=0;
	       	 }
			 else{
	             gpro_t.send_ack_cmd=0;
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

void ack_handler(void)
{
  
  //power ack 
  switch(gpro_t.send_ack_cmd){
              

   case 0x01: //power on or off

   
   if(gpro_t.ack_cp_cmd_flag==0x11){
	   gpro_t.ack_cp_repeat_counter++;
   if(gpro_t.ack_cp_repeat_counter < 4 && gpro_t.gTimer_copy_cmd_counter > 0 ){
	       gpro_t.gTimer_copy_cmd_counter =0;
	       SendData_PowerOnOff(1);
		   osDelay(5);

   }
   }
   else if(gpro_t.ack_cp_cmd_flag==0x10){ //be should is power off .
         gpro_t.ack_cp_repeat_counter++;
	     if(gpro_t.ack_cp_repeat_counter < 10 && gpro_t.gTimer_copy_cmd_counter > 0){
	       gpro_t.gTimer_copy_cmd_counter =0;
	       SendData_PowerOnOff(0);
		   osDelay(5);
         }

     }

	 if(gpro_t.ack_cp_repeat_counter > 5){
	  	gpro_t.ack_cp_repeat_counter=6;
		gpro_t.send_ack_cmd=0;

	  }


  

  break;

  case 0x02: //PTC ON OR OFF ACK

  
  if(gpro_t.ack_cp_cmd_flag == 0x21){// 0x2 ->
      gpro_t.ack_cp_repeat_counter++;
     if(gpro_t.ack_cp_repeat_counter < 4 && gpro_t.gTimer_copy_cmd_counter > 0){
			gpro_t.gTimer_copy_cmd_counter =0;
			SendData_Set_Command(0x02,0x01); //close ptc 
			osDelay(5);
	 }
  
	}
    else if(	gpro_t.ack_cp_cmd_flag == 0x20){ // 0x2 ->)

      gpro_t.ack_cp_repeat_counter++;
	 if(gpro_t.ack_cp_repeat_counter < 4 && gpro_t.gTimer_copy_cmd_counter > 0){
		   gpro_t.gTimer_copy_cmd_counter =0;
		   SendData_Set_Command(0x02,0x00); //close ptc 
		   osDelay(5);
  
  
	   }
  
	  if(gpro_t.ack_cp_repeat_counter > 5){
	  	gpro_t.ack_cp_repeat_counter=6;
		gpro_t.send_ack_cmd=0;

	  }

  
  }
  break;

  case 0x22:
  //PTC ON OR OFF BY compare value 

  if(gpro_t.ack_cp_cmd_flag == 0xE1){// 0x1XX ->0XDXX  0X2XX->0XEXX
      gpro_t.ack_cp_repeat_counter++;
      if(gpro_t.ack_cp_repeat_counter < 20 && gpro_t.gTimer_copy_cmd_counter > 0){
			gpro_t.gTimer_copy_cmd_counter =0;
			SendData_Set_Command(0x22,0x01); //close ptc 
			osDelay(5);
	  }
  
	

  }
  else if(	gpro_t.ack_cp_cmd_flag == 0xE0){ // 0x2 ->)

      gpro_t.ack_cp_repeat_counter++;
	 if(gpro_t.ack_cp_repeat_counter < 20 && gpro_t.gTimer_copy_cmd_counter > 0){
		   gpro_t.gTimer_copy_cmd_counter =0;
		   SendData_Set_Command(0x22,0x00); //close ptc 
		   osDelay(5);
  
  
	   }
   }
  
	 if(gpro_t.ack_cp_repeat_counter > 20){//WT.EDIT 2025.11.19
	  	gpro_t.ack_cp_repeat_counter=20;
		gpro_t.send_ack_cmd=0;

	  }

  
  
  break;

  case 0x1C:
  //TWO HOURS  ON OR OFF B

  if(gpro_t.two_hours_cp_flag == 0xC1){// 0x2 ->  // 0x1XX ->0XDXX  0X2XX->0XEXX
      gpro_t.ack_cp_repeat_counter++;
      if(gpro_t.ack_cp_repeat_counter < 4 && gpro_t.gTimer_copy_cmd_counter > 0){
			gpro_t.gTimer_copy_cmd_counter =0;
			SendData_twoHours_Data(0x78);//120 minutes
			osDelay(5);
  
  
		}
  
	 
  }
  else if(gpro_t.two_hours_cp_flag== 0xC0){ // 0x2 ->)

     gpro_t.ack_cp_repeat_counter++;
	 if(gpro_t.ack_cp_repeat_counter < 4 && gpro_t.gTimer_copy_cmd_counter > 0){
		   gpro_t.gTimer_copy_cmd_counter =0;
		   SendData_twoHours_Data(0x0A);//10 minutes
		   osDelay(5);
  
  
	   }
   }
	 if(gpro_t.ack_cp_repeat_counter > 5){
	  	gpro_t.ack_cp_repeat_counter=6;
		gpro_t.send_ack_cmd=0;

	  }

  
  

  break;

  case ack_wifi_on:

   if(gpro_t.ack_cp_cmd_flag == 0x31){// 0x1XX ->0XDXX  0X2XX->0XEXX
       gpro_t.ack_cp_repeat_counter++;
      if(gpro_t.ack_cp_repeat_counter < 4 && gpro_t.gTimer_copy_cmd_counter > 0){

       gpro_t.gTimer_cp_timer_counter =0;
       SendData_Set_Command(0x05,0x01); // link wifi of command .
       osDelay(3);
      
    }
    }
    if(gpro_t.ack_cp_repeat_counter > 5){
		gpro_t.send_ack_cmd=0;
		gpro_t.ack_cp_repeat_counter=6;

    }

  break;

}	

}




/**
 * @brief  UART中断服务程序（ISR）
 * @param  data 接收到的数据字节
 */
//void usart1_isr_callback_handler(uint8_t data) {
//   app_xusart1_queue_isr_handler(data);
//}
uint8_t parse_decoder_flag;


/******************************************************************************
	*
	*Function Name
	*Funcion: handle of tall process 
	*Input Ref:
	*Return Ref:
	*
******************************************************************************/
#define RX_BUF_SIZE 64 // 假设接收缓冲区大小 
// 定义全局或静态变量记录读位置
static uint32_t last_pos = 0;

bool extract_frame(void) 
{
    // 获取当前 DMA 写指针
    uint32_t curr_pos = MAX_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_1);
    
    // 如果读写指针一致，说明没有新数据
    if (curr_pos == last_pos) return false;

    uint32_t i = last_pos;

    // 搜索从上一次停止的地方到当前写位置之间的区域
    while (i != curr_pos) 
    {
        uint32_t head1 = i;
        uint32_t head2 = (i + 1) % MAX_BUFFER_SIZE;

        // 匹配帧头 0x5A 0x10
        if (rx_buf[head1] == 0x5A && rx_buf[head2] == 0x10) 
        {
            // 计算当前缓冲区内可用的数据字节数
            uint32_t available = (curr_pos >= i) ? (curr_pos - i) : (MAX_BUFFER_SIZE - i + curr_pos);

            // 检查是否够一帧 (11 字节)
            if (available >= NEW_BUF_SIZE) 
            {
                // 将数据拷贝到 inputBuf，确保从 inputBuf[0] 开始
                for (uint8_t j = 0; j < NEW_BUF_SIZE; j++) {
                    inputBuf[j] = rx_buf[(i + j) % MAX_BUFFER_SIZE];
                }

                // 重点：更新读指针，跳过这完整的一帧数据
                last_pos = (i + NEW_BUF_SIZE) % MAX_BUFFER_SIZE;
                return true; // 提取成功，返回 true
            }
            else 
            {
                // 找到了帧头，但后面的数据还没收齐
                // 此时不要移动 last_pos，等下一次中断补齐数据后再处理
                return false; 
            }
        }
        
        // 没匹配到帧头，移动到下一个字节继续找
        i = (i + 1) % MAX_BUFFER_SIZE;
    }

    // 如果遍历了一圈都没找到帧头，说明这段数据是垃圾数据
    // 同步指针到当前位置，清空搜索范围
    last_pos = curr_pos;
    return false;
} 



/******************************************************************************
	*
	*Function Name:void parse_decoder_handler(void)
	*Funcion: 
	*Input Ref:
	*Return Ref:
	*
******************************************************************************/
void parse_decoder_handler(void)
{
     
	uint8_t i;
	
	while(extract_frame()){


         if(inputBuf[2]==0xFF){ //copy command 

		     gl_tMsg.copy_cmd_notice = 0xFF;
			  
		     gl_tMsg.cmd_notice = inputBuf[3];
		
		
		     gl_tMsg.execuite_cmd_notice = inputBuf[4];
			
		  
			 parse_decoder_flag=1;

			 rx_data_counter=0;
			 return ;
			 
		 }
		 else{
		 	gl_tMsg.copy_cmd_notice = 0;
			gl_tMsg.cmd_notice = inputBuf[2];
            //gl_tMsg.usData[rx_data_counter] = inputBuf[3];
          
           if(inputBuf[3]==0x0F){ //is data frame ,don't is command 

               gl_tMsg.data_length =inputBuf[4]; //receive data of length
               gl_tMsg.execuite_cmd_notice=0;
               for(i=0;i<gl_tMsg.data_length;i++){
		          rx_data_counter++;
               
			      gl_tMsg.rx_data[i] = inputBuf[4+rx_data_counter];
         
                 
               }
			   rx_data_counter=0;
   
			    parse_decoder_flag=1;
			    memset(inputBuf,0,sizeof(inputBuf));
				 return ;
           }
		   else{
                gl_tMsg.execuite_cmd_notice =  inputBuf[3];
				 rx_data_counter=0;
				
                parse_decoder_flag=1;
		
		        memset(inputBuf,0,sizeof(inputBuf));
				return ;

            }
		  

		 }


	 }
  
 }
/******************************************************************************
	*
	*Function Name
	*Funcion: handle of tall process 
	*Input Ref:
	*Return Ref:
	*
******************************************************************************/
void parse_handler(void)
{
    if( parse_decoder_flag	== 1){
		 
		// receive_cmd_or_data_handler();
		parse_recieve_data_handler();

		 parse_decoder_flag ++;
	
	  }



}


