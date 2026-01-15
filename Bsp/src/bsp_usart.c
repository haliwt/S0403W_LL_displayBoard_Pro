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

#define NEW_BUF_SIZE 12 // 需要复制的字节数 
#define FRAME_HEAD1 0x5A 
#define FRAME_ADDR 0x10

#define S03_MAX_DATA_LEN  4


// 环形缓冲区
uint8_t rx_buf[12];
volatile uint32_t write_pos = 0;
//volatile uint32_t last_pos  = 0;



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


typedef struct
{
    uint8_t header;      // 帧头 0x5A / 0xA5
    uint8_t dev_addr;    // 设备地址/类型
    uint8_t cmd_type;    // 命令类型：指令/应答/通知...
    uint8_t func_code;   // 功能码：0x01 开关机等
    uint8_t copy_type;   // 复制类型
    uint8_t data_len;    // 数据长度 N
    uint8_t data_id_end; //
    uint8_t data[S03_MAX_DATA_LEN]; // 数据区
    uint8_t tail;        // 帧尾 0xFE
    uint8_t rx_data_num;
    uint8_t bcc;         // BCC 校验

} S03Frame_t;


S03Frame_t frame;

 typedef enum {
	 S03_STATE_WAIT_HEADER = 0,
	 S03_STATE_DEV_ADDR,
	 S03_STATE_CMD_TYPE,
	 S03_STATE_FUNC_CODE,
	 S03_STATE_DATA_LEN,
	 S03_STATE_DATA,
	 s03_STATE_CMD_END,
	 S03_STATE_TAIL,
	 S03_STATE_COPY_MODE,
	 S03_STATE_COPY_FUNC_CODE,
	 S03_STATE_COPY_TAIL,
	 S03_STATE_BCC
 } S03_State_e;

 S03_State_e s_state;


 static void receive_cmd_or_data_handler(void);
 static void receive_copy_cmd_or_data_handler(void);
 static void read_usart1_data(uint8_t data);



 volatile uint8_t rx_data_counter=0,rx_numbers ;

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
#if 0
void usart1_isr_callback_handler(uint8_t data)
{
     static uint8_t state;
    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	// memcpy(inputBuf,rx_buf,sizeof(rx_buf)) ;       
	static uint8_t inputBuf[2];
	inputBuf[0]=data;
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
				gl_tMsg.usData[0]=0;
				gl_tMsg.usData[1]=1;
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
			   	 gl_tMsg.copy_cmd_notice = 0;
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
                state = UART_STATE_FRAME_CMD_0X0;; //receive data.
            }


        break;

		case UART_STATE_FRAME_CMD_0X0: //0x04 //receive comd and notice frame  0x00
			rx_data_counter++;
			gl_tMsg.usData[rx_data_counter] = inputBuf[0];
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
            gl_tMsg.usData[rx_data_counter] = inputBuf[0];
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
            gl_tMsg.usData[rx_data_counter] = inputBuf[0];
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
             gl_tMsg.usData[rx_data_counter] = inputBuf[0];

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
        gl_tMsg.usData[rx_data_counter] = inputBuf[0];
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
            gl_tMsg.usData[rx_data_counter] = inputBuf[0];
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
            gl_tMsg.usData[rx_data_counter] = inputBuf[0];
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

/**
* @brief receive cmd or data from mainboard .
* @note  处理 ACK 帧
* @param
* @return 
*
*/
static void receive_cmd_or_data_handler(void)
{

   switch(frame.cmd_type){
	case power_on_off:

	if(frame.func_code == 0x01){//power on
		run_t.power_on = power_on;
	}
	else{//power off 
		run_t.power_on = power_off;

	}
	break;

	case ptc_on_off: //PTC 
	if(frame.func_code == 0x01){//ptc on
		    run_t.dry = open;
			run_t.ptc_on_off_flag = 0;
			SendData_Set_Command(0x12,0x01); //close ptc 
			vTaskDelay(100);
			gpro_t.gTimer_copy_cmd_counter=0; 
			gpro_t.receive_copy_buff[2]=copy_null;
			Display_Kill_Dry_Ster_Icon();
		}
		else{//power off 
			run_t.dry = close;
			run_t.ptc_on_off_flag = 1;
			SendData_Set_Command(0x12,0x0); //close ptc 
		    vTaskDelay(100);
			gpro_t.gTimer_copy_cmd_counter=0; 
	        gpro_t.receive_copy_buff[2]=copy_null;
			Display_Kill_Dry_Ster_Icon();
	    }

	break;

	case plasma_on_off://plasma
	if(frame.func_code == 0x01){//ptc on
		 run_t.plasma = open;
		 SendData_Set_Command(0x13,0x01); //close ptc 
		 vTaskDelay(100);
		 gpro_t.gTimer_copy_cmd_counter=0; 
	     gpro_t.receive_copy_buff[3]=copy_null;
		 Display_Kill_Dry_Ster_Icon();

		}
		else{//power off 
		run_t.plasma = close;
		SendData_Set_Command(0x13,0x0); //close ptc 
		 vTaskDelay(100);
		 gpro_t.gTimer_copy_cmd_counter=0; 
	     gpro_t.receive_copy_buff[3]=copy_null;
		 Display_Kill_Dry_Ster_Icon();
	}

	break;

	case ultrasonic_on_off:
	if(frame.func_code == 0x01){//ptc on
		run_t.ultrasonic = open;
		SendData_Set_Command(0x14,0x01); //close ptc 
		vTaskDelay(100);
		 gpro_t.gTimer_copy_cmd_counter=0; 
	     gpro_t.receive_copy_buff[4]=copy_null;
		 Display_Kill_Dry_Ster_Icon();

		}
		else{//power off 
		run_t.ultrasonic = close;
		SendData_Set_Command(0x13,0x0); //close ptc 
		 vTaskDelay(100);
		 gpro_t.gTimer_copy_cmd_counter=0; 
	     gpro_t.receive_copy_buff[4]=copy_null;
		 Display_Kill_Dry_Ster_Icon();
	}

	break;

	case 0x05:

	    power_key_long_fun();

	break;

	case fan_on_off:
	if(frame.func_code == 0x01){//ptc on
		run_t.fan = open;

		}
		else{//power off 
		run_t.fan = close;
	}

	break;

	

    case 0x08: //temperature of high warning.

	if(frame.func_code == 0x01){  //warning 


		//run_t.setup_timer_timing_item =  PTC_WARNING; //ptc warning 
		run_t.display_set_timer_or_works_time_mode = PTC_WARNING;

		run_t.dry=0;
		//SendData_Set_Command(0x08,0x01); //high temperatue warning .

		SendWifiData_Answer_Cmd(0x08,0x01);
	   vTaskDelay(100);

	}
	else if(frame.func_code== 0x0){ //close 

		run_t.ptc_warning = 0;


	}
	break;

	case 0x09: //fan of default of warning.

	if(frame.func_code == 0x01){  //warning 

		run_t.fan_warning = 1;

		run_t.display_set_timer_or_works_time_mode =FAN_WARNING;  //run_t.display_set_timer_or_works_time_mode

		run_t.dry =0;
		//SendData_Set_Command(0x09,0x01); //
		SendWifiData_Answer_Cmd(0x09,0x01);
		vTaskDelay(100);

	}
	else if(frame.func_code == 0x0){ //close 

		run_t.fan_warning = 0;
	}

	break;

    case 0x20: //compatablie older version .
	case 0x15: //传递三个参数 ---通知
      
	   if(frame.data_len==0x03){ //power on by smart phone APP
	
		run_t.dry =frame.data[0];
		if(run_t.dry == 0){


		}
		else{

		}

		run_t.plasma=frame.data[1];
		if(run_t.plasma ==1){

		}
		else{
		 

		}

		run_t.ultrasonic =frame.data[2];
		if(run_t.ultrasonic==1){
		 
		}
		else{


		}
	  }
	
   break;
		


   case 0x1A: //read sensor "DHT11" temperature and humidity value .



	    gpro_t.humidity_real_value = frame.data[0];
		gpro_t.temp_real_value = frame.data[1];

		

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

	if(frame.data_len == 0x03){ //鏁版嵁

		if(frame.data[0] < 24){ //WT.EDIT 2024.11.23

		lcd_t.display_beijing_time_flag= 1;

		run_t.dispTime_hours  =  frame.data[0];
		run_t.dispTime_minutes = frame.data[1];
		run_t.gTimer_disp_time_seconds =  frame.data[2];
	}


	}
	break;

	case 0x1E: //fan of speed is value 

	if( frame.data[0] < 34){
		run_t.wifi_link_net_success=1;
		run_t.disp_wind_speed_grade = 10;


	}
	else if( frame.data[0] < 67 &&  frame.data[0] > 33){
		run_t.wifi_link_net_success=1;
		run_t.disp_wind_speed_grade = 60;

	}
	else if( frame.data[0] > 66){
		run_t.wifi_link_net_success=1;
		run_t.disp_wind_speed_grade =100;
	}



	break;


	case 0x1F: //link wifi if success data don't command and notice.

	if(frame.data[0] == 0x01){  // link wifi 

		run_t.wifi_link_net_success =1 ;      

	}
	else{ //don't link wifi 

		run_t.wifi_link_net_success =0 ;     

	}



	break;

	

    //smart phone data
//	case 0x21: //new version smart phone normal power on and off command.

//	if(frame.func_code== 0x01){ //open
//		run_t.wifi_link_net_success=1;
	
//	    run_t.power_on = power_on;
//		SendWifiData_Answer_Cmd(0x31,0x01);
//	    vTaskDelay(100);
	   
//		//xTask_PowerOn_Handler() ; 

//	}
//	else if(frame.func_code == 0x0){ //close 
//		run_t.wifi_link_net_success=1;
//		 run_t.power_on = power_off;
//		 Lcd_PowerOff_Fun();
//		 SendWifiData_Answer_Cmd(0x031,0x0);
//		 vTaskDelay(100);
//		//xTask_PowerOff_Handler() ; 
//	}

//	break;


	case 0x21: //APP smart phone Timer power on or off that App timer ---new .
	if(frame.func_code==0x01){ //power on by smart phone APP
		gpro_t.smart_phone_app_timer_power_on_flag =1;
		run_t.wifi_link_net_success=1;
	   
        run_t.power_on= power_on; //WT.EDIT 2025.12.18
        gpro_t.power_on_step=0;


		  SendWifiData_Answer_Cmd(0x021,0x01);
		  vTaskDelay(100);
		
         
       

	}
	else if(frame.func_code==0x0){  //power off by smart phone APP
		run_t.wifi_link_net_success=1;
		run_t.power_on= power_off;
	    Lcd_PowerOff_Fun();
		SendWifiData_Answer_Cmd(0x021,0x0);
		vTaskDelay(100);
		   
	}

	break;


	case 0x07:


	case 0x27 : //AI mode by smart phone of APP be control.

     
	if(frame.func_code==2){
		//timer time + don't has ai item
	
			run_t.display_set_timer_or_works_time_mode = timer_time;
			run_t.gTimer_again_switch_works = 0; //WT.EDIT ,if don't define timer_time,wait 3s switch to works_time.
			run_t.gModel=0;
		    gpro_t.switch_not_ai_mode=1;
		    display_not_ai_timer_mode();
		
	}
	else{ //AI mode 
		//beijing time + ai item
	
			run_t.display_set_timer_or_works_time_mode = works_time;
		
			run_t.gTimer_again_switch_works = 0;
			gpro_t.switch_not_ai_mode=0;
            display_ai_icon(1) ;
			run_t.gModel=1;
		

	    }


	break;

	

	case 0x2A: // set up temperature value 

        run_t.ptc_on_off_flag = 0; //WT.EDIT 2025.10.31
	   

		run_t.wifi_set_temperature = frame.data[0];
	
 		lcd_t.number1_low=run_t.wifi_set_temperature / 10 ;
		lcd_t.number1_high =run_t.wifi_set_temperature / 10 ;

		lcd_t.number2_low = run_t.wifi_set_temperature % 10; //
		lcd_t.number2_high = run_t.wifi_set_temperature % 10; //

		gpro_t.gTimer_set_temp_times=0;//WT.EDIT 2025.10.31
		gpro_t.temp_key_set_value =1;
        run_t.smart_phone_set_temp_value_flag =1;
       

	break;

	case 0x2B: //set up timer timing value .
		
           if(frame.data[0]>0 && run_t.ptc_warning==0 && run_t.fan_warning==0){
		   run_t.timer_time_hours = frame.data[0];
		  
		   lcd_t.number5_low =	run_t.timer_time_hours / 10;
		   lcd_t.number5_high = run_t.timer_time_hours / 10;
		   lcd_t.number6_low =	run_t.timer_time_hours % 10;
		   lcd_t.number6_high = run_t.timer_time_hours % 10;
		   
		   lcd_t.number7_low = 0 ;
		   lcd_t.number7_high =0;
		   lcd_t.number8_low = 0 ;
		   lcd_t.number8_high= 0;
	
		 
		   display_digits(0x0F, 1);
		   vTaskDelay(1000);
	       display_digits(0xFF, 0);
	 
		   run_t.timer_timing_define_flag = timing_success;
		   run_t.display_set_timer_or_works_time_mode=timer_time;
		   run_t.gTimer_again_switch_works = 0;
		   gpro_t.switch_not_ai_mode=1;
		   run_t.gModel = 0;
		  // display_not_ai_timer_mode();
		 
	
		   
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
static void receive_copy_cmd_or_data_handler(void)
{
    switch(frame.copy_type){

        case power_on_off:
			
		if(frame.func_code == 0x01){//power on
		
	      
		   

		}
		else if(run_t.power_on == power_off){
	

			

		}


	   break;

	   case  ptc_on_off:
	   	
		if(frame.func_code == 0x01){//ptc on
		
			
			
		}
		else {
			
		
              
		 }

		

	   break;

	   case plasma_on_off:
	   	
		if(frame.func_code== 0x01){//ptc on
			
			
			}
			else {
			

			}

	   break;

	   case ultrasonic_on_off:
	   	
		if(frame.func_code == 0x01){//ptc on
			

		}
		else {
	
		}

	   break;

	   case wifi_link://0x05
	   	
	      if(frame.func_code ==1){
            
			   power_key_long_fun();
   
		  }
	   
	  break;

	   case fan_on_off:

		if(frame.func_code == 0x01){//ptc on
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

	       if(frame.func_code == 0x01){
	   	
                 
	       	 }
			 else{
	             
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

void waiting_ack_handler(void)
{
  
#if 0
  //power ack 
  switch(){
              

   case 0x01: //power on or off

   
   
	   gpro_t.ack_cp_repeat_counter++;
   if(gpro_t.ack_cp_repeat_counter < 4 && gpro_t.gTimer_copy_cmd_counter > 0 ){
	       gpro_t.gTimer_copy_cmd_counter =0;
	       SendData_PowerOnOff(1);
		   osDelay(5);

   }
   }

         gpro_t.ack_cp_repeat_counter++;
	     if(gpro_t.ack_cp_repeat_counter < 10 && gpro_t.gTimer_copy_cmd_counter > 0){
	       gpro_t.gTimer_copy_cmd_counter =0;
	       SendData_PowerOnOff(0);
		   osDelay(5);
         }

     }

	 if(gpro_t.ack_cp_repeat_counter > 5){
	  	gpro_t.ack_cp_repeat_counter=6;

	  }


  

  break;

  case 0x02: //PTC ON OR OFF ACK

  

      gpro_t.ack_cp_repeat_counter++;
     if(gpro_t.ack_cp_repeat_counter < 4 && gpro_t.gTimer_copy_cmd_counter > 0){
			gpro_t.gTimer_copy_cmd_counter =0;
			SendData_Set_Command(0x02,0x01); //close ptc 
			osDelay(5);
	 }
  
	}
    

      gpro_t.ack_cp_repeat_counter++;
	 if(gpro_t.ack_cp_repeat_counter < 4 && gpro_t.gTimer_copy_cmd_counter > 0){
		   gpro_t.gTimer_copy_cmd_counter =0;
		   SendData_Set_Command(0x02,0x00); //close ptc 
		   osDelay(5);
  
  
	   }
  
	  if(gpro_t.ack_cp_repeat_counter > 5){
	  	gpro_t.ack_cp_repeat_counter=6;
		

	  }

  
  }
  break;

  case 0x22:
  //PTC ON OR OFF BY compare value 


      gpro_t.ack_cp_repeat_counter++;
      if(gpro_t.ack_cp_repeat_counter < 20 && gpro_t.gTimer_copy_cmd_counter > 0){
			gpro_t.gTimer_copy_cmd_counter =0;
			SendData_Set_Command(0x22,0x01); //close ptc 
			osDelay(5);
	  }
  
	

  }


      gpro_t.ack_cp_repeat_counter++;
	 if(gpro_t.ack_cp_repeat_counter < 20 && gpro_t.gTimer_copy_cmd_counter > 0){
		   gpro_t.gTimer_copy_cmd_counter =0;
		   SendData_Set_Command(0x22,0x00); //close ptc 
		   osDelay(5);
  
  
	   }
   }
  
	 if(gpro_t.ack_cp_repeat_counter > 20){//WT.EDIT 2025.11.19
	  	gpro_t.ack_cp_repeat_counter=20;
	

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


	  }

  
  

  break;

  case ack_wifi_on:


       gpro_t.ack_cp_repeat_counter++;
      if(gpro_t.ack_cp_repeat_counter < 4 && gpro_t.gTimer_copy_cmd_counter > 0){

       gpro_t.gTimer_cp_timer_counter =0;
       SendData_Set_Command(0x05,0x01); // link wifi of command .
       osDelay(3);
      
    }
    }
    if(gpro_t.ack_cp_repeat_counter > 5){
	
		gpro_t.ack_cp_repeat_counter=6;

    }

  break;

}	
#endif 
}




/**
 * @brief  UART中断服务程序（ISR）
 * @param  data 接收到的数据字节
 */
//void usart1_isr_callback_handler(uint8_t data) {
//   app_xusart1_queue_isr_handler(data);
//}
volatile uint8_t parse_decoder_flag;


/******************************************************************************
	*
	*Function Name
	*Funcion: handle of tall process 
	*Input Ref:
	*Return Ref:
	*
******************************************************************************/
//#define RX_BUF_SIZE 64 // 假设接收缓冲区大小 
// 定义全局或静态变量记录读位置
static uint32_t last_pos = 0;
uint8_t inputBuf[12];

#if 0
bool extract_frame(void) 
{
   

   #if 0
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
              //  memset(rx_buf,0,128);
                // 重点：更新读指针，跳过这完整的一帧数据
                last_pos = (i + NEW_BUF_SIZE) % MAX_BUFFER_SIZE;
                return true; // 提取成功，返回 true
            }
            else 
            {
                // 找到了帧头，但后面的数据还没收齐
                // 此时不要移动 last_pos，等下一次中断补齐数据后再处理
              /// memset(rx_buf,0,128);
                return false; 
            }
        }
        
        // 没匹配到帧头，移动到下一个字节继续找
        i = (i + 1) % MAX_BUFFER_SIZE;
    }

    // 如果遍历了一圈都没找到帧头，说明这段数据是垃圾数据
    // 同步指针到当前位置，清空搜索范围
    //memset(rx_buf,0,sizeof(rx_buf));
    last_pos = curr_pos;
    return false;

	#else 

   uint32_t curr_pos = write_pos; 
	while (last_pos != curr_pos) { 
	uint8_t byte = rx_buf[last_pos]; 
	last_pos = (last_pos + 1) % MAX_BUFFER_SIZE; 
	switch (frame_state) { 
		case 0: // 等待第一个字节 
		if (byte == FRAME_HEAD1) { 
			frame_state = 1; 
		} 
	     break; 

		case 1: // 等待第二个字节 
		if (byte == FRAME_HEAD2) { 
			frame_buf[0] = FRAME_HEAD1; 
			frame_buf[1] = FRAME_HEAD2; 
		     frame_index = 2; 
			 frame_state = 2; // 进入收集数据状态 

			 } 
            else {
			 	frame_state = 0; 
				// 回到初始状态
				} break; 
			case 2: // 收集剩余字节 
			frame_buf[frame_index++] = byte; 
			if (frame_index >= NEW_BUF_SIZE) {
				memcpy(inputBuf, frame_buf, NEW_BUF_SIZE); 
				frame_state = 0; 
			   return true; // 完整帧提取成功 
			   } 
			 break; 
			} 
		} return false;

   #endif 

} 

#endif 

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
   if(gpro_t.decoder_flag == 1){
		 gpro_t.decoder_flag =0; 
	
		receive_cmd_or_data_handler();
       
	//	printf("CommPro task started!\r\n");
		
	
   }
   else if(gpro_t.decoder_flag == 2){
	    gpro_t.decoder_flag =0; 
        receive_copy_cmd_or_data_handler();

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
void USART1_IRQHandler(void)
{
   volatile uint8_t data ;

	if (LL_USART_IsActiveFlag_RXNE_RXFNE(USART1))
    {
        data = LL_USART_ReceiveData8(USART1); 

        // 写入环形缓冲区
      
        read_usart1_data(data);

       
     
    }

    // 清除错误标志
    if (LL_USART_IsActiveFlag_ORE(USART1)) LL_USART_ClearFlag_ORE(USART1);
    if (LL_USART_IsActiveFlag_FE(USART1))  LL_USART_ClearFlag_FE(USART1);
    if (LL_USART_IsActiveFlag_NE(USART1))  LL_USART_ClearFlag_NE(USART1);
}

/**
*@brief
*@notice 
*@param
*/
static void read_usart1_data(uint8_t data)
{
    volatile  static uint8_t step_flag,frame_index;

	
        switch (s_state)
        {
            case S03_STATE_WAIT_HEADER: // 等待第一个字节
                if (data == FRAME_HEAD1) {
                    rx_buf[0] = FRAME_HEAD1;
				
					s_state= S03_STATE_DEV_ADDR;
                }
				else{
					
					
					s_state= S03_STATE_WAIT_HEADER;
				

				}
            break;

            case S03_STATE_DEV_ADDR: // 等待第二个字节
                if (data == FRAME_ADDR){
                  
                   rx_buf[1] = data;
				  
                   s_state =S03_STATE_CMD_TYPE;
                } 
				else{
			
					
                   s_state =S03_STATE_WAIT_HEADER; // 回到初始状态
                }
            break;

            case S03_STATE_CMD_TYPE: // 收集剩余字节 2
                frame.cmd_type = data;
                rx_buf[2] = data;
			    
			   if(frame.cmd_type == 0xFF){
				 s_state =  S03_STATE_COPY_MODE;
               }
			   else{

			     s_state =	S03_STATE_FUNC_CODE;

			  }

		    break;

		 case S03_STATE_FUNC_CODE:

           frame.func_code = data; 
				
		   rx_buf[3] = data;
		   
		 
			if(frame.func_code == 0x0F){
                   
                  s_state = S03_STATE_DATA_LEN;
            }
			else{
			    s_state = s03_STATE_CMD_END;//S03_STATE_TAIL ;

               
			}
				

		  break;

		 case s03_STATE_CMD_END:
		      frame.data_id_end = data;
			  rx_buf[4] = data;
		      if(frame.data_id_end ==0) { 
		      s_state = S03_STATE_TAIL;

			 }
			 else{
                 s_state =S03_STATE_WAIT_HEADER; // 回到初始状态

			 }

		 break;

		 case S03_STATE_TAIL:

              frame.tail = data;
			  rx_buf[5] = data;
			 
			  if(frame.tail == 0xFE){//data is tail is 0xFE
			  	frame.rx_data_num =0;
                s_state = S03_STATE_WAIT_HEADER ;
				parse_decoder_flag= 1;
			    gpro_t.decoder_flag = 1;
				semaphore_isr();
              
			  }
			  else{
		

			      s_state = S03_STATE_WAIT_HEADER ;

			  }

		 break;


		  case S03_STATE_DATA_LEN :

		      frame.data_len = data;
			  rx_buf[4] = data;
			  frame.rx_data_num =0;
			  s_state = S03_STATE_DATA ;


		 break;

		 case S03_STATE_DATA:

             if(frame.data_len == 1)  // 0~4头信息 + N 数据
		     {
		        frame.data[0] = data; //第一个数据
		          
			     s_state = S03_STATE_TAIL;
						
				
		      }
			  else if(frame.data_len == 2){

		           frame.rx_data_num++;
				   if(frame.rx_data_num==1){
	                   frame.data[0] = data; //第一个数据
				   }
	               else if(frame.rx_data_num ==2){
				   	frame.rx_data_num=0;
				    frame.data[1] = data; //第二个数据
	                s_state = S03_STATE_TAIL;
	               }
						
		       }
			   else if(frame.data_len == 3){
		               frame.rx_data_num++;
					   if(frame.rx_data_num==1){
		                   frame.data[0] = data; //第一个数据
					   }
					   else if(frame.rx_data_num ==2){
					     frame.data[1] = data; //第一个数据
                       }
		               else if(frame.rx_data_num ==3){
					   	frame.rx_data_num=0;
					    frame.data[2] = data; //第二个数据
		                s_state = S03_STATE_TAIL;
		               }
		       }
			   else{
                   s_state = S03_STATE_WAIT_HEADER ;

			   }

		 break;



		 //copy command 
		 case  S03_STATE_COPY_MODE:

		      frame.copy_type = data; 
		      s_state = S03_STATE_COPY_FUNC_CODE;

          break;

		  case  S03_STATE_COPY_FUNC_CODE:
                frame.func_code = data;
				
                s_state = S03_STATE_COPY_TAIL;
		  break;

		  case  S03_STATE_COPY_TAIL:

		        if(frame.tail == 0xFE){
				   s_state = S03_STATE_WAIT_HEADER;
				  parse_decoder_flag= 2;
				   
				   gpro_t.decoder_flag = 2;
				   semaphore_isr();
				   return;

				}
				else{
				   s_state = S03_STATE_WAIT_HEADER;

				   return ;


				}

		  break;


		}

		  

 }

     
 

  



