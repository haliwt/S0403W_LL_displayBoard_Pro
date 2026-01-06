#include "bsp.h"


#define S03_MAX_DATA_LEN   8
//#define S03_HEADER_MAIN    0x5A
#define S03_HEADER_DISPLAY 0x5A //main board of head 
#define S03_MAIN_ADDR      0x10
#define S03_TAIL           0xFE


typedef enum {
    S03_STATE_WAIT_HEADER = 0,
    S03_STATE_DEV_ADDR,
    S03_STATE_CMD_TYPE,
    S03_STATE_FUNC_CODE,
    S03_STATE_DATA_LEN,
    S03_STATE_DATA,
    S03_STATE_TAIL,
    S03_STATE_BCC
} S03_State_e;



typedef struct
{
    uint8_t header;      // 帧头 0x5A / 0xA5
    uint8_t dev_addr;    // 设备地址/类型
    uint8_t cmd_type;    // 命令类型：指令/应答/通知...
    uint8_t func_code;   // 功能码：0x01 开关机等
    uint8_t copy_type;   // 复制类型
    uint8_t data_len;    // 数据长度 N
    uint8_t data[S03_MAX_DATA_LEN]; // 数据区
    uint8_t tail;        // 帧尾 0xFE
    uint8_t bcc;         // BCC 校验
} S03Frame_t;

S03Frame_t frame;

static S03_State_e s_state = S03_STATE_WAIT_HEADER;
static uint8_t     s_expected_data_len = 0;


static void S03_Frame_Dispatch(const S03Frame_t f);
static void receive_cmd_or_data_handler(const S03Frame_t f);
static void receive_copy_cmd_or_data_handler(const S03Frame_t f);


/*****************************************************************************
	**
	*Function Name
	*Function :  逐字节状态机（核心）,每来一个字节就进一次
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
void S03_Protocol_ByteHandler(uint8_t *pdch)
{

  uint8_t recv_bcc,calc_bcc ;


	
   
            if(pdch[0] == S03_HEADER_DISPLAY && pdch[1]==S03_MAIN_ADDR)
            {
               
                s_state = S03_STATE_CMD_TYPE;
            }
			else{

                return ;
			}
         

      

          if(s_state ==S03_STATE_CMD_TYPE){

		     frame.cmd_type = pdch[2]; 
             if(frame.cmd_type == 0xFF){
                 frame.copy_type = pdch[3];
                 frame.func_code = pdch[4];
			     S03_Frame_Dispatch(frame);
				 return ;
             }
			 else{

                frame.func_code = pdch[3]; 
		        if(frame.func_code ==0x0F){ //是数据
					frame.data_len = pdch[4]; //数据的长度
		            if(frame.data_len == 1)  // 0~4头信息 + N 数据
		            {
		                frame.data[0] = pdch[5]; //第一个数据
						frame.tail  = pdch[6];
						recv_bcc = pdch[7];
						s_state = S03_STATE_BCC;
						 S03_Frame_Dispatch(frame);
						 return;
		            }
					else if(frame.data_len == 2){
		                 frame.data[0] = pdch[5]; //第一个数据
		                 frame.data[1] = pdch[6]; //第二个数据
		                frame.tail = pdch[7];
						 recv_bcc = pdch[8];
						s_state = S03_STATE_BCC;
						 S03_Frame_Dispatch(frame);
						 return;
		            }
					else if(frame.data_len == 3){
		                frame.data[0] = pdch[5]; //第一个数据
		                frame.data[1]=  pdch[6]; //第一个数据
		                frame.data[2] = pdch[7]; //第三个数据
		                frame.tail   = pdch[8] ;
						 recv_bcc = pdch[9];
						s_state = S03_STATE_BCC;
						S03_Frame_Dispatch(frame);
						return;
		            }


				}
				 S03_Frame_Dispatch(frame);
				 return ;

			  }
              
			}

      
    
}

/**
*@brief
*@notice 
*@param
*/
/*****************************************************************************
	**
	*Function Name:
	*Function :  
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
static void S03_Frame_Dispatch(const S03Frame_t f)
{
    // 先按命令类型分大类（指令/通知/应答）
  
       if(frame.cmd_type == 0xFF){// 举例：应答
               
          receive_copy_cmd_or_data_handler(f);
           // S03_Handle_Ack(f);
        }
        else{
          
          
          receive_cmd_or_data_handler(f);    
                 
        }

}

/**
* @brief receive cmd or data from mainboard .
* @note  处理 ACK 帧
* @param
* @return 
*
*/
static void receive_cmd_or_data_handler(const S03Frame_t f)
{

   switch(frame.cmd_type){

    case 0:

    break;

	case power_on_off:

	if(frame.func_code == 0x01){//power on
		//run_t.power_on = power_on;
		}
		else{//power off 
		//run_t.power_on = power_off;

		}
	break;

	case ptc_on_off: //PTC 
	if( frame.func_code == 0x01){//ptc on
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

		run_t.ptc_warning = 1;
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
	else if(frame.func_code  == 0x0){ //close 

		run_t.fan_warning = 0;
	}

	break;

	case 0x1A: //read sensor "DHT11" temperature and humidity value .



	    gpro_t.humidity_real_value = frame.data[0];//gl_tMsg.rx_data[0];
		gpro_t.temp_real_value =frame.data[1] ;//gl_tMsg.rx_data[1];

		

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

	//if(gl_tMsg.receive_data_length == 0x03){ //鏁版嵁

	if(frame.data[0] < 24){ //WT.EDIT 2024.11.23

	lcd_t.display_beijing_time_flag= 1;

	run_t.dispTime_hours  =  frame.data[0];//gl_tMsg.rx_data[0];
	run_t.dispTime_minutes = frame.data[1];//gl_tMsg.rx_data[1];
	run_t.gTimer_disp_time_seconds =  frame.data[2];//gl_tMsg.rx_data[2];
	}


	///}
	break;

	case 0x1E: //fan of speed is value 

	if( frame.data[0] < 34){
		run_t.wifi_link_net_success=1;
		run_t.disp_wind_speed_grade = 10;


	}
	else if( frame.data[0]< 67 &&  frame.data[0] > 33){
		run_t.wifi_link_net_success=1;
		run_t.disp_wind_speed_grade = 60;

	}
	else if(frame.data[0] > 66){
		run_t.wifi_link_net_success=1;
		run_t.disp_wind_speed_grade =100;
	}



	break;

	case 0x1F: //link wifi if success data don't command and notice is data.

	if(frame.data[0] == 0x01){  // link wifi 

		run_t.wifi_link_net_success =1 ;      

	}
	else{ //don't link wifi 

		run_t.wifi_link_net_success =0 ;     

	}



	break;

	case 0x27 : //AI mode by smart phone of APP be control.

	if(frame.func_code==2){
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

	if(frame.func_code == 0x01){ //open
		run_t.wifi_link_net_success=1;
	
	    run_t.power_on = power_on;
		SendWifiData_Answer_Cmd(0x31,0x01);
	    vTaskDelay(100);
	   
		//xTask_PowerOn_Handler() ; 

	}
	else if(frame.func_code == 0x0){ //close 
		run_t.wifi_link_net_success=1;
		 run_t.power_on = power_off;
		 Lcd_PowerOff_Fun();
		 SendWifiData_Answer_Cmd(0x031,0x0);
		 vTaskDelay(100);
		//xTask_PowerOff_Handler() ; 
	}

	break;


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

	case 0x2A: //new version main board or smart phone app set temperature value


	case 0x3A: // smart phone APP set temperature value 

		run_t.wifi_link_net_success=1;
	    run_t.ptc_on_off_flag = 0; //WT.EDIT 2025.10.31
	   

		run_t.wifi_set_temperature = frame.data[0];//gl_tMsg.rx_data[0];

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
static void receive_copy_cmd_or_data_handler(const S03Frame_t f)
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

	   case wifi_link:
	   	
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

