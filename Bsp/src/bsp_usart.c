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




// 环形缓冲区


//volatile uint32_t last_pos  = 0;

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
   // uint8_t cmd_type;    // 命令类型：指令/应答/通知...
    uint8_t bcc_code;   // 功能码：0x01 开关机等
    uint8_t check_code;
   // uint8_t copy_type;   // 复制类型
    uint8_t data_len;    // 数据长度 N
    uint8_t count_numbers;
   // uint8_t data_id_end; //
    uint8_t data[12]; // 数据区
   // uint8_t tail;        // 帧尾 0xFE
    uint8_t rx_data_num;
   // uint8_t bcc;         // BCC 校验

} S03Frame_t;


S03Frame_t frame;

 typedef enum {
	 S03_STATE_WAIT_HEADER = 0,
	 S03_STATE_DATA,
	 S03_STATE_BCC
 } S03_State_e;

 S03_State_e s_state;


 static void parse_cmd_or_data_(uint8_t *pddata);
 static void parse_copy_cmd_or_data_handler(uint8_t *pdata);

 static void read_usart1_data(uint8_t data);

// BCC校验函数
uint8_t bcc_check(const unsigned char *data, int len) {
    unsigned char bcc = 0;
    for (int i = 0; i < len; i++) {
        bcc ^= data[i];
    }
    return bcc;
}


/**
* @brief receive cmd or data from mainboard .
* @note  处理 ACK 帧
* @param
* @return 
*
*/
 uint8_t temp_run_flag;

static void parse_cmd_or_data_(uint8_t *pddata)
{
   static uint8_t temp_array[1],ptc_counter=0xff,plasma_counter = 0xff,ultr_counter= 0xff;
  
   switch(pddata[2]){
	case power_on_off:

	if(pddata[3] == 0x01){//power on
		run_t.power_on = power_on;
	}
	else{//power off 
		run_t.power_on = power_off;

	}
	break;

	case ptc_on_off: //PTC 
	if(pddata[3] == 0x01){//ptc on

	        if(ptc_counter != run_t.dry){
				ptc_counter = run_t.dry;
		    run_t.dry = open;

	
			run_t.ptc_on_off_flag = 0;
			SendWifiData_Answer_Cmd(0x02,0x01); //close ptc 
			vTaskDelay(100);
			
			//Display_Kill_Dry_Ster_Icon();
	        }
	}
	else{//power off 
	       if(ptc_counter != run_t.dry){
				ptc_counter = run_t.dry;
			run_t.dry = close;
			run_t.ptc_on_off_flag = 1;
			SendWifiData_Answer_Cmd(0x02,0x0); //close ptc 
		    vTaskDelay(100);
		
			//Display_Kill_Dry_Ster_Icon();
	        }
	 }

	break;

	case plasma_on_off://plasma
	if(pddata[3] == 0x01){//ptc on

	     if(plasma_counter != run_t.plasma ){
		 	plasma_counter  = run_t.plasma;
			 run_t.plasma = open;
			 SendWifiData_Answer_Cmd(0x03,0x01); //close ptc 
			 vTaskDelay(100);
			 gpro_t.gTimer_copy_cmd_counter=0; 
		     gpro_t.receive_copy_buff[3]=copy_null;
			// Display_Kill_Dry_Ster_Icon();
	     }

		}
		else{//power off 
		   if(plasma_counter != run_t.plasma ){
		   	    plasma_counter  = run_t.plasma;
				run_t.plasma = close;
			    SendWifiData_Answer_Cmd(0x03,0x0); //close ptc 
				vTaskDelay(100);
				 gpro_t.gTimer_copy_cmd_counter=0; 
			     gpro_t.receive_copy_buff[3]=copy_null;
				 //Display_Kill_Dry_Ster_Icon();
		   	}
	}

	break;

	case ultrasonic_on_off:
	if(pddata[3] == 0x01){//ptc on

	    if(ultr_counter != run_t.ultrasonic ){
			ultr_counter = run_t.ultrasonic;
			run_t.ultrasonic = open;
			SendWifiData_Answer_Cmd(0x04,0x01); //close ptc 
			vTaskDelay(100);
			
	    }

		}
		else{//power off 

		
	    if(ultr_counter != run_t.ultrasonic ){
			ultr_counter = run_t.ultrasonic;
		run_t.ultrasonic = close;
		SendWifiData_Answer_Cmd(0x04,0x0); //close ptc 
		vTaskDelay(100);
		 gpro_t.gTimer_copy_cmd_counter=0; 
	     gpro_t.receive_copy_buff[4]=copy_null;
		

	    }
	}

	break;

	case 0x05: //wifi link notice don't data.
        if(pddata[3] ==1)
	      power_key_long_fun();

	break;

	case fan_on_off:
	if(pddata[3] == 0x01){//ptc on
		run_t.fan = open;

		}
		else{//power off 
		run_t.fan = close;
	}

	break;

	

    case 0x08: //temperature of high warning.

	if(pddata[3] == 0x01){  //warning 


		//run_t.setup_timer_timing_item =  PTC_WARNING; //ptc warning 
		run_t.display_set_timer_or_works_time_mode = PTC_WARNING;

		run_t.dry=0;
	

		SendWifiData_Answer_Cmd(0x08,0x01);
	   vTaskDelay(100);

	}
	else if(pddata[3]== 0x0){ //close 

		run_t.ptc_warning = 0;


	}
	break;

	case 0x09: //fan of default of warning.

	if(pddata[3] == 0x01){  //warning 

		run_t.fan_warning = 1;

		run_t.display_set_timer_or_works_time_mode =FAN_WARNING;  //run_t.display_set_timer_or_works_time_mode

		run_t.dry =0;
	
		SendWifiData_Answer_Cmd(0x09,0x01);
		vTaskDelay(100);

	}
	else if(pddata[3] == 0x0){ //close 

		run_t.fan_warning = 0;
	}

	break;

    case 0x20: //compatablie older version .
	case 0x15: //传递三个参数 ---通知
      
	   if(pddata[4]==0x03){ //power on by smart phone APP
	
		run_t.dry =pddata[5];
		if(run_t.dry == 0){


		}
		else{

		}

		run_t.plasma=pddata[6];
		if(run_t.plasma ==1){

		}
		else{
		 

		}

		run_t.ultrasonic =pddata[7];
		if(run_t.ultrasonic==1){
		 
		}
		else{


		}
	  }
	
   break;
		


   case 0x1A: //read sensor "DHT11" temperature and humidity value .

     #if 1
        if(run_t.power_on  == power_on && gpro_t.temp_key_set_value==0 && gpro_t.power_on_counter_temp==2){
	   
		if(pddata[6]  < 60){
			 gpro_t.humidity_real_value = pddata[5];

		     gpro_t.temp_real_value = pddata[6];
			
		 
		 }
		 

        }

	  #endif 
        

	
	break;
	


	case 0x1C: //time is hours,minutes,seconds value .

	if(pddata[4] == 0x03){ //鏁版嵁

		if(frame.data[5] < 24){ //WT.EDIT 2024.11.23

		lcd_t.display_beijing_time_flag= 1;

		run_t.dispTime_hours  =  pddata[5];
		run_t.dispTime_minutes = pddata[6];
		run_t.gTimer_disp_time_seconds =  pddata[7];
	}


	}
	break;

	case 0x1E: //fan of speed is data 123435

	if( pddata[5] < 34){
		run_t.wifi_link_net_success=1;
		run_t.disp_wind_speed_grade = 10;


	}
	else if( pddata[5] < 67 &&  pddata[5] > 33){
		run_t.wifi_link_net_success=1;
		run_t.disp_wind_speed_grade = 60;

	}
	else if( pddata[5] > 66){
		run_t.wifi_link_net_success=1;
		run_t.disp_wind_speed_grade =100;
	}



	break;


	case 0x1F: //link wifi if success data don't command and notice.

	if(frame.data[3] == 0x01){  // link wifi 

		run_t.wifi_link_net_success =1 ;      

	}
	else{ //don't link wifi 

		run_t.wifi_link_net_success =0 ;     

	}



	break;

	
   case 0x21: //APP smart phone Timer power on or off that App timer ---new .
	if(pddata[3]==0x01){ //power on by smart phone APP
		gpro_t.smart_phone_app_timer_power_on_flag =1;
		run_t.wifi_link_net_success=1;
	   
        run_t.power_on= power_on; //WT.EDIT 2025.12.18
        gpro_t.power_on_step=0;


		 SendWifiData_Answer_Cmd(0x021,0x01);
		 vTaskDelay(100);
		
         
       

	}
	else if(pddata[3]==0x0){  //power off by smart phone APP
		run_t.wifi_link_net_success=1;
		run_t.power_on= power_off;
	    Lcd_PowerOff_Fun();
		SendWifiData_Answer_Cmd(0x021,0x0);
		vTaskDelay(100);
		   
	}

	break;


	case 0x07:


	case 0x27 : //AI mode by smart phone of APP be control.

    #if 0
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
     #endif 

	break;

	

	case 0x2A: // set up temperature data value

        run_t.ptc_on_off_flag = 0; //WT.EDIT 2025.10.31
	   
        if(pddata[4]==0x01){
		run_t.wifi_set_temperature = pddata[5];
	
 		lcd_t.number1_low=run_t.wifi_set_temperature / 10 ;
		lcd_t.number1_high =run_t.wifi_set_temperature / 10 ;

		lcd_t.number2_low = run_t.wifi_set_temperature % 10; //
		lcd_t.number2_high = run_t.wifi_set_temperature % 10; //

		gpro_t.gTimer_set_temp_times=0;//WT.EDIT 2025.10.31
		run_t.gTimer_timing =0;
		gpro_t.temp_key_set_value =1;
        run_t.smart_phone_set_temp_value_flag =1;

        }
       

	break;

	case 0x2B: //set up timer timing value .
		
           if(pddata[5]>0 && run_t.ptc_warning==0 && run_t.fan_warning==0){
		   run_t.timer_time_hours = pddata[5];
		  
		   lcd_t.number5_low =	run_t.timer_time_hours / 10;
		   lcd_t.number5_high = run_t.timer_time_hours / 10;
		   lcd_t.number6_low =	run_t.timer_time_hours % 10;
		   lcd_t.number6_high = run_t.timer_time_hours % 10;
		   
		   lcd_t.number7_low = 0 ;
		   lcd_t.number7_high =0;
		   lcd_t.number8_low = 0 ;
		   lcd_t.number8_high= 0;
	
		 
		   display_digits(0x0F, 1);
		   //vTaskDelay(1000);
	       //display_digits(0xFF, 0);
	 
		   run_t.timer_timing_define_flag = timing_success;
		   run_t.display_set_timer_or_works_time_mode=timer_time;
		   run_t.gTimer_again_switch_works = 0;
		   gpro_t.switch_not_ai_mode=1;
		   run_t.gModel = 0;
		  
		}
   	

	break;

	case 0xFF:
		
	    parse_copy_cmd_or_data_handler(pddata);

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
static void parse_copy_cmd_or_data_handler(uint8_t *pdata)
{
    switch(pdata[3]){

        case power_on_off:
				
			if(pdata[4] == 0x01){//power on
			
		      
			   

			}
			else if(pdata[4]==0){
		

				

			}


	   break;

	   case  ptc_on_off:
	   	
		if(pdata[4] == 0x01){//ptc on
		
			
			
		}
		else {
			
		
              
		 }

		

	   break;

	   case plasma_on_off:
	   	
		if(pdata[4] == 0x01){//ptc on
			
			
			}
			else {
			

			}

	   break;

	   case ultrasonic_on_off:
	   	
		if(pdata[4]  == 0x01){//ptc on
			

		}
		else {
	
		}

	   break;

	   case wifi_link://0x05
	   	
	      if(pdata[4]  ==1){
            
			   power_key_long_fun();
   
		  }
	   
	  break;

	   case fan_on_off:

		if(pdata[4] == 0x01){//ptc on
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

	       if(pdata[4]== 0x01){
	   	
                 
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

//#define RX_BUF_SIZE 64 // 假设接收缓冲区大小 
// 定义全局或静态变量记录读位置
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

				   frame.count_numbers=0;
					
                    frame.data[frame.count_numbers]= data;
				
					s_state= S03_STATE_DATA;
                }
				
            break;

            case S03_STATE_DATA: // 等待第二个字节
            
			  frame.count_numbers++;
							   
			  frame.data[frame.count_numbers]= data;
						   
               if(frame.data[frame.count_numbers] == 0xFE){
			     s_state= S03_STATE_BCC;


			   }
			     
			
            break;

            case S03_STATE_BCC: // 收集剩余字节 2
               frame.count_numbers++;
							   
			   frame.data[frame.count_numbers]= data;

			   frame.data_len =  frame.count_numbers;
			    
			   s_state= S03_STATE_WAIT_HEADER;

			   frame.bcc_code = data;
			 
                gpro_t.decoder_flag = 1;
				semaphore_isr();

		    break;

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
void decoder_handler(void)
{
    
	frame.check_code = bcc_check(frame.data,frame.data_len);

	if(frame.check_code == frame.bcc_code){
	
       parse_cmd_or_data_(frame.data);

	}
       
	
}

  



