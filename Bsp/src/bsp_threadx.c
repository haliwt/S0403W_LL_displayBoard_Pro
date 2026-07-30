#include "bsp.h"

#define KEY_MODE_SHORT   (1 << 0)
#define KEY_MODE_LONG    (1 << 1)

#define KEY_UP_SHORT     (1 << 2)
#define KEY_UP_LONG      (1 << 3)

#define KEY_DOWN_SHORT   (1 << 4)
#define KEY_DOWN_LONG    (1 << 5)

#define KEY_POWER_SHORT  (1 << 6)
#define KEY_POWER_LONG   (1 << 7)




#define STACK_SIZE_UI     768//512//1792//1536//1024//1664 
#define STACK_SIZE_KEY    256//512
#define STACK_SIZE_DEC    512//
#define STACK_SIZE_EVENT  512

#define DEBUG_ENABLE       0


__attribute__((aligned(8)))  static UCHAR stack_ui_pro[STACK_SIZE_UI];
__attribute__((aligned(8)))  static UCHAR stack_key_pro[STACK_SIZE_KEY];
__attribute__((aligned(8)))  static UCHAR stack_decoder_pro[STACK_SIZE_DEC];
__attribute__((aligned(8)))  static UCHAR stack_key_event[STACK_SIZE_EVENT];




static TX_THREAD  thread_ui;
static TX_THREAD  thread_key;
static TX_THREAD  thread_decoder;
static TX_THREAD  thread_key_event;

static void ui_thread_entry(ULONG thread_input);
static void key_thread_entry(ULONG thread_input);
static void decoder_thread_entry(ULONG thread_input);
static void key_event_thread_entry(ULONG thread_input);


TX_EVENT_FLAGS_GROUP key_event;

TX_SEMAPHORE      decoder_semaphore;
//TX_SEMAPHORE      uart1_tx_semaphore;

/* 创建任务通信机制 */
static void tx_thread_stack_error_handler(TX_THREAD *thread_ptr);


#if DEBUG_ENABLE
static void debug_ui_check(void);
static void debug_key_check(void);
static void debug_decoder_check(void);
static void debug_key_event_check(void);


volatile uint32_t ui=0,key=0,dec=0,event=0;

#endif 


uint8_t error_counter;
/**
*@brief 
*@param
*@notice
*@retval
**/
void tx_application_define(VOID * first_unused_memory)
{

	  /* 1. 消除参数未引用警告 */
		(void)first_unused_memory;


  #if DEBUG_ENABLE
	  // --- 关键点：在创建任务之前填充魔术字 ---
	  memset(stack_ui_pro,0xEF,sizeof(stack_ui_pro));

	  memset(stack_key_pro,0xEF,sizeof(stack_key_pro));
	  memset(stack_decoder_pro,0xEF,sizeof(stack_decoder_pro));
	  memset(stack_key_event,0xEF,sizeof(stack_key_event));
  #endif 

  /* 3. 注册堆栈错误回调（推荐保持） */
 
    tx_thread_stack_error_notify(tx_thread_stack_error_handler);

    app_threadx_handler();

}


/**
*@brief 
*@param
*@notice
*@retval
**/
static void decoder_thread_entry(ULONG thread_input)
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
	    
   #if DEBUG_ENABLE
    debug_decoder_check();
   #endif 
	
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
static void ui_thread_entry(ULONG thread_input)
{
  (void)thread_input;
  while(1){
   		
        power_run_handler();

      #if DEBUG_ENABLE
		    debug_ui_check();
	  #endif 
		//tx_thread_sleep(1);//10ms
   
  }


}
/**
*@brief 
*@param
*@notice
*@retval
**/
static void key_event_thread_entry(ULONG thread_input)
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
	else if(flags & KEY_MODE_SHORT){/* MODE 键 */
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
	     gpro_t.buzzer_sound_f =1;
	    add_key_fun();	

		
	}  
    else if(flags & KEY_DOWN_SHORT){
         
		  //SendData_Buzzer_Has_Ack();//
		  SendData_Buzzer();
		  tx_thread_sleep(2);
	      gpro_t.buzzer_sound_f =1;
		  dec_key_fun();

	}

	 #if DEBUG_ENABLE
             debug_key_event_check();
       #endif 
	   
	   
     }

    }
}

/**
*@brief 
*@param
*@notice
*@retval
**/
static void key_thread_entry(ULONG thread_input)
{
  (void)thread_input;
  static uint8_t power_on_key;

     static uint16_t mode_cnt = 0;
    static uint16_t up_cnt = 0;
    static uint16_t down_cnt = 0;
    static uint16_t power_cnt = 0;

    const uint16_t LONG_PRESS_TIME = 40;   //20ms * 100= 2000ms
    
  while(1){

	   /* ================= POWER 键 ================= */
      if(KEY_POWER_GetValue() == KEY_DOWN)
        {
            power_cnt++;
            if(power_cnt == LONG_PRESS_TIME && run_t.power_on== power_on){
                tx_event_flags_set(&key_event, KEY_POWER_LONG, TX_OR);
            }
        }
        else if(power_cnt > 0 && KEY_POWER_GetValue() == KEY_UP){
			
			   if(power_cnt < LONG_PRESS_TIME)
			  	    tx_event_flags_set(&key_event, KEY_POWER_SHORT, TX_OR);

            power_cnt = 0;
        }
        else if(KEY_MODE_GetValue() == KEY_DOWN && run_t.power_on== power_on)
        {
            mode_cnt++;
            if(mode_cnt == LONG_PRESS_TIME){
			
                tx_event_flags_set(&key_event, KEY_MODE_LONG, TX_OR);
               
            }
        }
        else if(mode_cnt > 0 && KEY_MODE_GetValue() == KEY_UP){
			 if(mode_cnt < LONG_PRESS_TIME){
                tx_event_flags_set(&key_event, KEY_MODE_SHORT, TX_OR);
			 }
            mode_cnt = 0;
			 
        }
        else if(KEY_ADD_GetValue() == KEY_DOWN && run_t.power_on== power_on)
        {
            up_cnt++;
            
        }
        else if(up_cnt > 0 && KEY_ADD_GetValue() == KEY_UP){

			   if(up_cnt < LONG_PRESS_TIME)
                   tx_event_flags_set(&key_event, KEY_UP_SHORT, TX_OR);

            up_cnt = 0;
        }
        else if(KEY_DEC_GetValue() == KEY_DOWN && run_t.power_on== power_on)
        {
            down_cnt++;
           
        }
        else  if(down_cnt > 0 && KEY_DEC_GetValue() == KEY_UP){
			    if(down_cnt < LONG_PRESS_TIME)
                tx_event_flags_set(&key_event, KEY_DOWN_SHORT, TX_OR);

            down_cnt = 0;
        }

        
   #if DEBUG_ENABLE
    debug_key_check();
   #endif 
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
  					decoder_thread_entry,
  					0,
  					stack_decoder_pro,
  					STACK_SIZE_DEC,
  					2,
  					2,
  					TX_NO_TIME_SLICE,
  					TX_AUTO_START);


   tx_thread_create(&thread_ui,
   					"MsgPro",
   					ui_thread_entry,
   					0,
   					stack_ui_pro,
   					STACK_SIZE_UI,
   					3,
   					3,
   					TX_NO_TIME_SLICE,
   					TX_AUTO_START);

	tx_thread_create(&thread_key,
					"KeyPro",
					key_thread_entry,
					0,
					stack_key_pro,
					STACK_SIZE_KEY,
					0,
					0,
					TX_NO_TIME_SLICE,
					TX_AUTO_START);
	
	 tx_thread_create(&thread_key_event, 		   /* 任务控制块地址 */	  
					 "KeyEvent",				    /* 任务名 */
					  key_event_thread_entry,				/* 启动任务函数地址 */
					  0,							/* 传递给任务的参数 */
					  stack_key_event,				/* 堆栈基地址 */
					  STACK_SIZE_EVENT,				/* 堆栈空间大小 */  
					  1,							/* 任务优先级*/
					  1,							/* 任务抢占阀值 */
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
  //printf("stack overflow in thread:%s \n", thread_ptr->tx_thread_name );
}

/****************************************************************
*
*	Function Name: vTaskStart
*	Function:
*	Input Ref: pvParameters 是在创建该任务时传�?�的形参
*	Return Ref:
*	priority: 3  (数�?�越小优先级越低，这个跟uCOS相反)
*
******************************************************************/
void tx_thread_stack_error_handler(TX_THREAD *thread_ptr)
{
    /* 栈溢出处理：这里你可以做任何你想做的动作 */

    /* 1. 打印线程名（如果有 UART） */
    // printf("Stack overflow in thread: %s\n", thread_ptr->tx_thread_name);

    /* 2. 拉高故障指示灯 */
    // Fault_LED_On();
 
    /* 3. 记录日志（如果有 Flash/EEPROM） */
    // Log_Fault(FAULT_STACK_OVERFLOW, thread_ptr->tx_thread_name);

    /* 4. 触发系统复位（汽车级） */
    //NVIC_SystemReset();
    tx_thread_sleep(20);
    //while(1);  // 调试阶段可以卡住
}


#if DEBUG_ENABLE
static void debug_ui_check(void)
{
    ULONG i;
   // ULONG unused = 0;
   ULONG temp_unused = 0; // 使用局部变量进行统计
   
    // 从数组起始位置（栈底/低地址）开始数连续的 0xEF
    for (i = 0; i < STACK_SIZE_UI; i++)
    {
        if (stack_ui_pro[i] == 0xEF)
            temp_unused++;
        else
            break; 
    }
	ui= temp_unused;  // 统计完后再赋值给全局变量，方便 Watch 窗口查看
    // 剩下的 unused 就是你安全的“护城河”
    // 如果 unused < 100 字节，你的 G030 就危险了！
}

static void debug_key_check(void)
{
	  ULONG i;
   // ULONG unused = 0;
   ULONG temp_unused = 0; // 使用局部变量进行统计

    // 从数组起始位置（栈底/低地址）开始数连续的 0xEF
    for (i = 0; i < STACK_SIZE_KEY; i++)
    {
        if (stack_key_pro[i] == 0xEF)
            temp_unused++;
        else
            break; 
    }
	key = temp_unused;  // 统计完后再赋值给全局变量，方便 Watch 窗口查看
    // 剩下的 unused 就是你安全的“护城河”
    // 如果 unused < 100 字节，你的 G030 就危险了！

}

static void debug_decoder_check(void)
{
   ULONG i;
   // ULONG unused = 0;
   ULONG temp_unused = 0; // 使用局部变量进行统计

    // 从数组起始位置（栈底/低地址）开始数连续的 0xEF
    for (i = 0; i < STACK_SIZE_DEC; i++)
    {
        if (stack_decoder_pro[i] == 0xEF)
            temp_unused++;
        else
            break; 
    }
	dec= temp_unused;  // 统计完后再赋值给全局变量，方便 Watch 窗口查看
    // 剩下的 unused 就是你安全的“护城河”
    // 如果 unused < 100 字节，你的 G030 就危险了！
}

static void debug_key_event_check(void)
{
   ULONG i;
   // ULONG unused = 0;
   ULONG temp_unused = 0; // 使用局部变量进行统计


    // 从数组起始位置（栈底/低地址）开始数连续的 0xEF
    for (i = 0; i < STACK_SIZE_KEY; i++)
    {
        if (stack_key_event[i] == 0xEF)
            temp_unused++;
        else
            break; 
    }
	event= temp_unused;  // 统计完后再赋值给全局变量，方便 Watch 窗口查看
    // 剩下的 unused 就是你安全的“护城河”
    // 如果 unused < 100 字节，你的 G030 就危险了！

}

#endif 


