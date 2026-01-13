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
#if 0
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
#endif 
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

