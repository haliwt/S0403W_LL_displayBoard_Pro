#include "bsp.h"

#define KEY_MODE_SHORT   (1 << 0)
#define KEY_MODE_LONG    (1 << 1)

#define KEY_UP_SHORT     (1 << 2)
#define KEY_UP_LONG      (1 << 3)

#define KEY_DOWN_SHORT   (1 << 4)
#define KEY_DOWN_LONG    (1 << 5)

#define KEY_POWER_SHORT  (1 << 6)
#define KEY_POWER_LONG   (1 << 7)




#define STACK_SIZE_UI     1536//1024 
#define STACK_SIZE_KEY    256//512
#define STACK_SIZE_DEC    512//
#define STACK_SIZE_EVENT  512

__attribute__((aligned(8)))  static UCHAR stack_ui_pro[STACK_SIZE_UI];
__attribute__((aligned(8)))  static UCHAR stack_key_pro[STACK_SIZE_KEY];
__attribute__((aligned(8)))  static UCHAR stack_decoder_pro[STACK_SIZE_DEC];
__attribute__((aligned(8)))  static UCHAR stack_key_event[STACK_SIZE_EVENT];





static TX_THREAD  thread_ui;
static TX_THREAD  thread_key;
static TX_THREAD  thread_decoder;
static TX_THREAD  thread_key_event;

static void vTaskUiPro(ULONG thread_input);
static void vTaskKeyPro(ULONG thread_input);
static void vTaskDecoderPro(ULONG thread_input);
static void vTaskKeyEvent(ULONG thread_input);


TX_EVENT_FLAGS_GROUP key_event;

TX_SEMAPHORE      decoder_semaphore;
//TX_SEMAPHORE      uart1_tx_semaphore;


typedef struct GL_TASK{

   uint8_t  long_key_mode_counter;
   uint8_t  long_key_power_counter;
   uint8_t  key_long_power_flag;
  // uint8_t  key_long_mode_flag;
   uint8_t key_mode_short_flag ;
   uint8_t  key_power_flag;
   uint8_t  key_mode_flag ;
   uint8_t  key_dec_flag;
   uint8_t  key_add_flag;


   uint8_t smart_phone_app_timer_power_on_flag;



}gl_task;

gl_task gl_ref;
uint8_t error_counter;
/**
*@brief 
*@param
*@notice
*@retval
**/
void tx_application_define(VOID * first_unused_memory)
{

  app_threadx_handler();

}


/**
*@brief 
*@param
*@notice
*@retval
**/
static void vTaskDecoderPro(ULONG thread_input)
{
   (void)thread_input;

   while(1){
   #if 0
    tx_event_flags_get(&commEventFlags,
						(1<<9),
						TX_OR_CLEAR,   /*获取后清除标志*/
						&actual_flags,
						TX_WAIT_FOREVER);

   if(actual_flags & (1<< 9)){
      counter ++ ;
      decoder_handler();
   }
   #else 
   if(tx_semaphore_get(&decoder_semaphore,TX_WAIT_FOREVER)==TX_SUCCESS){
	  

	   	
	      decoder_handler();
	
   }

   #endif


   }

}
/**
*@brief 
*@param
*@notice
*@retval
**/
static void vTaskUiPro(ULONG thread_input)
{
  (void)thread_input;
  while(1){
   		
        power_run_handler();
		tx_thread_sleep(2);//10ms
   
  }


}
/**
*@brief 
*@param
*@notice
*@retval
**/
static void vTaskKeyEvent(ULONG thread_input)
{
  (void)thread_input;
  ULONG flags;
  UINT status;
  while(1){


     status = tx_event_flags_get(&key_event,
                           0xFFFFFFFF,
                           TX_OR_CLEAR,
                           &flags,
                           TX_WAIT_FOREVER);//TX_NO_WAIT);//TX_WAIT_FOREVER);//
                           
     if(status == TX_SUCCESS){

	    if(flags & KEY_POWER_SHORT){

           power_on_off_handler();

		}
	    else if(flags & KEY_POWER_LONG){

           SendData_Set_Command(0x05,0x01); // link wifi of command .
	       tx_thread_sleep(2); //receive tx oxff command of run 

		}
	  /* MODE 键 */
        else if(flags & KEY_MODE_SHORT){
			SendData_Buzzer();
		    tx_thread_sleep(2);
		    mode_key_short_fun();
           

		}
	    else if(flags & KEY_MODE_LONG){
            SendData_Buzzer();
			tx_thread_sleep(2);
			mode_key_long_fun();

		}
        else if(flags & KEY_UP_SHORT){
           
		        SendData_Buzzer();//SendData_Buzzer_Has_Ack();//SendData_Buzzer();
				tx_thread_sleep(2);
		
				add_key_fun();
		}  
	    else if(flags & KEY_DOWN_SHORT){
          SendData_Buzzer();
		  tx_thread_sleep(2);
		  dec_key_fun();

		}
	   
     }

       
   }
}

/**
*@brief 
*@param
*@notice
*@retval
**/
static void vTaskKeyPro(ULONG thread_input)
{
  (void)thread_input;
  static uint8_t power_on_key;

     static uint16_t mode_cnt = 0;
    static uint16_t up_cnt = 0;
    static uint16_t down_cnt = 0;
    static uint16_t power_cnt = 0;

    const uint16_t LONG_PRESS_TIME = 120;   //20ms * 100= 2000ms
    
  while(1){

	   /* ================= POWER 键 ================= */
      if(KEY_POWER_GetValue() == KEY_DOWN)
        {
            power_cnt++;
            if(power_cnt == LONG_PRESS_TIME && run_t.power_on== power_on){
                tx_event_flags_set(&key_event, KEY_POWER_LONG, TX_OR);
            }
        }
        else if(power_cnt > 1){
			
			   if(power_cnt < LONG_PRESS_TIME)
			  	    tx_event_flags_set(&key_event, KEY_POWER_SHORT, TX_OR);

            power_cnt = 0;
        }


		 /* ================= MODE 键 ================= */
        if(KEY_MODE_GetValue() == KEY_DOWN && run_t.power_on== power_on)
        {
            mode_cnt++;
            if(mode_cnt == LONG_PRESS_TIME){
			
                tx_event_flags_set(&key_event, KEY_MODE_LONG, TX_OR);
               
            }
        }
        else if(mode_cnt > 1){
			 if(mode_cnt < LONG_PRESS_TIME){
                tx_event_flags_set(&key_event, KEY_MODE_SHORT, TX_OR);
			 	}
            mode_cnt = 0;
			 
        }

      /* ================= UP 键 ================= */
        if(KEY_ADD_GetValue() == KEY_DOWN && run_t.power_on== power_on)
        {
            up_cnt++;
            
        }
        else if(up_cnt > 1){

			   if(up_cnt < LONG_PRESS_TIME)
                   tx_event_flags_set(&key_event, KEY_UP_SHORT, TX_OR);

            up_cnt = 0;
        }

        /* ================= DOWN 键 ================= */
        if(KEY_DEC_GetValue() == KEY_DOWN && run_t.power_on== power_on)
        {
            down_cnt++;
           
        }
        else  if(down_cnt > 1){
			    if(down_cnt < LONG_PRESS_TIME)
                tx_event_flags_set(&key_event, KEY_DOWN_SHORT, TX_OR);

            down_cnt = 0;
        }


   tx_thread_sleep(2);//2*10ms =20ms
   }
}

/**
*@brief 
*@param prority numbers small this power high.
*@notice
*@retval
**/
void app_threadx_handler(void)
{

  //tx_event_flags_create(&commEventFlags,"commEventFlags");
  tx_semaphore_create(&decoder_semaphore,"decoderSemaphore",0);
  tx_event_flags_create(&key_event, "key_event");

  tx_thread_create(&thread_decoder,
  					"DecoderPro",
  					vTaskDecoderPro,
  					0,
  					stack_decoder_pro,
  					STACK_SIZE_DEC,
  					2,
  					2,
  					TX_NO_TIME_SLICE,
  					TX_AUTO_START);


   tx_thread_create(&thread_ui,
   					"MsgPro",
   					vTaskUiPro,
   					0,
   					stack_ui_pro,
   					STACK_SIZE_UI,
   					4,
   					4,
   					TX_NO_TIME_SLICE,
   					TX_AUTO_START);

	tx_thread_create(&thread_key,
					"KeyPro",
					vTaskKeyPro,
					0,
					stack_key_pro,
					STACK_SIZE_KEY,
					1,
					1,
					TX_NO_TIME_SLICE,
					TX_AUTO_START);
	
	 tx_thread_create(&thread_key_event, 		   /* 任务控制块地址 */	  
					 "KeyEvent",				    /* 任务名 */
					  vTaskKeyEvent,				/* 启动任务函数地址 */
					  0,							/* 传递给任务的参数 */
					  stack_key_event,				/* 堆栈基地址 */
					  STACK_SIZE_EVENT,				/* 堆栈空间大小 */  
					  3,							/* 任务优先级*/
					  3,							/* 任务抢占阀值 */
					  TX_NO_TIME_SLICE, 			/* 不开启时间片 */
					  TX_AUTO_START);				/* 创建后立即启动 */
   
}



/*************************************************************************
*
*	Funtion Name:
*	Function: 
*	Input Ref: 
*	Return Ref:
*
**************************************************************************/
void semaphore_isr(void)
{
   //tx_event_flags_set(&commEventFlags,(1<<9),TX_OR);
     tx_semaphore_put(&decoder_semaphore);
}


void tx_application_stack_error_handler(TX_THREAD *thread_ptr)
{
  printf("stack overflow in thread:%s \n", thread_ptr->tx_thread_name );
}


