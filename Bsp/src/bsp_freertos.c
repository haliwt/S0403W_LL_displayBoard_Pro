#include "bsp.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"


#define POWER_KEY_BIT_0	       (1 << 0)
#define MODE_KEY_1	        (1 << 1)
#define DEC_KEY_2           (1 << 2)
#define ADD_KEY_3           (1 << 3)

#define POWER_OFF_BIT_4        (1 << 4)
#define POWER_ON_BIT_5         (1<< 5)



#define MODE_LONG_KEY_8         (1 << 8)
#define DECODER_BIT_9          (1<< 9)


/***********************************************************************************************************
											ĺ˝ć°ĺŁ°ć
***********************************************************************************************************/
//static void vTaskDecoderPro(void *pvParameters);
#if 0
static void vTaskRunPro(void *pvParameters);

static void vTaskStart(void *pvParameters);

#else

/*------------------ 静态任务内存定义 ------------------*/

/* CommTask */ 
static StaticTask_t xTaskCommProTCB; 
static StackType_t xTaskCommProStack[128];


/* vTaskMsgPro 任务 */
static StaticTask_t xTaskUiProTCB;
static StackType_t xTaskUiProStack[128];

/* vTaskStart 任务 */
static StaticTask_t xTaskKeyProTCB;
static StackType_t xTaskKeyProStack[128];

/* 静态缓冲区和句柄（文件作用域或模块静态） */ 
//static StaticSemaphore_t xBinarySemaphoreBuffer; 
//static SemaphoreHandle_t xBinarySemaphore = NULL;



#endif 
static void AppTaskCreate(void);
//static void AppObjCreate(void);


/* 定义静态内存块 */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];



static TaskHandle_t xHandleTaskCommPro = NULL;


static TaskHandle_t xHandleTaskUiPro = NULL;

static TaskHandle_t xHandleTaskKeyPro = NULL;



/* 内核会自动调用这个回调函数来获取 Idle 任务的内存 */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}



/* creat task communication  */
//static void AppObjCreate(void);
static void key_handler(void);
static void power_run_handler(void);





//TimerHandle_t           Timer1Timer_Handler;/* ĺŽćśĺ1¤71ĺĽć */
//TimerHandle_t           Timer2Timer_Handler;/* ĺŽćśĺ1¤72ĺĽć */

//void Timer1Callback(TimerHandle_t xTimer);  /* ĺŽćśĺ1¤71čśćśĺč°ĺ˝ć° */
//void Timer2Callback(TimerHandle_t xTimer);  /* ĺŽćśĺ1¤72čśćśĺč°ĺ˝ć° */
//static  QueueHandle_t xUartRxQueue = NULL;


typedef struct GL_TASK{

   uint8_t  long_key_mode_counter;
   uint8_t  long_key_power_counter;
   uint8_t  key_long_power_flag;
   uint8_t  key_long_mode_flag;
   uint8_t key_mode_short_flag ;
   uint8_t  key_power_flag;
   uint8_t  key_mode_flag ;
   uint8_t  key_dec_flag;
   uint8_t  key_add_flag;
   uint8_t   stopTwoHours_flag;

   uint8_t smart_phone_app_timer_power_on_flag;



}gl_task;

gl_task gl_ref;
uint8_t error_counter;


/**********************************************************************************************************
*	
*   Function Name:
*	Funciton:
*	Input Ref:
*   Return Ref:
*
**********************************************************************************************************/
void freeRTOS_Handler(void)
{
     /* ĺĺťşäťťĺĄ */
	  AppTaskCreate();
	  
	  /*  */
	 //  AppObjCreate();
	  
	  /*  */
	   vTaskStartScheduler();


}
/**********************************************************************************************************
*	
*   Function Name:
*	Funciton:  proritiy class is small and protity is low
*	Input Ref:
*   Return Ref:
*
**********************************************************************************************************/
static void vTaskCommPro(void *pvParameters)
{
    BaseType_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(3000); /* 设置最大等待时间为300ms */
	uint32_t ulValue;
	
	 while(1)
	 {
          xResult = xTaskNotifyWait(0x00000000,      
						           0xFFFFFFFF,      
						          &ulValue,        /* ??ulNotifiedValue???ulValue? */
						          xMaxBlockTime);  /* ????????,????-block portMAX_DELAY */
        if(xResult == pdPASS){
             if((ulValue & DECODER_BIT_9 ) != 0)
             {
   			   parse_handler();
			  
			  
	         }
		 }
		
	 }
}
/**********************************************************************************************************
*	
*   Function Name:
*	Funciton:  proritiy class is small and protity is low
*	Input Ref:
*   Return Ref:
*
**********************************************************************************************************/
static void vTaskUiPro(void *pvParameters)
{
   // BaseType_t xResult;
	//const TickType_t xMaxBlockTime = pdMS_TO_TICKS(30); //40//30/* čŽžç˝Žć˘ďż˝ĺ¤§ç­ĺžćśé´ä¸ş30ms */
	//uint32_t ulValue;
    
    while(1)
    {
		key_handler();
        power_run_handler();
	
	
        vTaskDelay(60);
    }
 }

/**********************************************************************************************************
*	Function Name: vTaskStart
*	Function: 
*	Input Ref: pvParameters
*	
*   
**********************************************************************************************************/
static void vTaskKeyPro(void *pvParameters)
{
  static uint8_t power_on_key;
   while(1)
    {
      
    if(KEY_POWER_GetValue()  ==KEY_DOWN && gl_ref.key_long_power_flag !=1){

          if(power_on_key ==0){
              power_on_key ++;
             
          }
          else{
           gl_ref.long_key_mode_counter =0;
           gl_ref.long_key_power_counter++;

         if(gl_ref.long_key_power_counter >75 && run_t.power_on== power_on ){//65
            gl_ref.long_key_power_counter =0;
            gl_ref.key_long_power_flag =1;
            gpro_t.gTimer_mode_key_long = 0;

			

             
              SendData_Set_Command(0x05,0x01); // link wifi of command .
              vTaskDelay(100);
              gpro_t.gTimer_mode_key_long=0;
			  gl_ref.key_power_flag = 0;
			
         }
        else{
           
               gl_ref.key_power_flag = 1;
            
			//printf("key_power_on !!! \r\n");
             }
        }
     }
    else if(KEY_MODE_GetValue() ==KEY_DOWN){

           gl_ref.long_key_power_counter=0;
         
          gl_ref.long_key_mode_counter ++ ;

          if(gl_ref.long_key_mode_counter > 65  && run_t.power_on== power_on &&  run_t.ptc_warning ==0 && run_t.fan_warning ==0){
             gl_ref.long_key_mode_counter=0;   
         
             gl_ref.key_long_mode_flag =1;
               gpro_t.gTimer_mode_key_long = 0;
            
                SendData_Buzzer();
				vTaskDelay(100);
           }

         if(run_t.power_on== power_on){
            gl_ref.key_mode_flag  =  1;

          }


     }
     else if(KEY_DEC_GetValue() == KEY_DOWN){
          gl_ref.long_key_power_counter=0;
          gl_ref.long_key_mode_counter =0 ;
           if(run_t.power_on== power_on){ //WT.EDIT .2025.01.15
               gl_ref.key_dec_flag = 1;
            }
     }
     else if(KEY_ADD_GetValue() ==KEY_DOWN){

          gl_ref.long_key_power_counter=0;
         
          gl_ref.long_key_mode_counter =0 ;
         if(run_t.power_on== power_on){ //WT.EDIT .2025.01.15
             gl_ref.key_add_flag = 1;
         }

    }

	

    
	 
    vTaskDelay(20);
     
    }

}
/**********************************************************************************************************
* Function Name:AppTaskCreate
*	
*	
*	
**********************************************************************************************************/
void AppTaskCreate (void)
{

 #if 0
   xTaskCreate( vTaskDecoderPro,    		/* fucntion name  */
                 "vTaskDecoderPro",  		/* alias name   */
                 128,         		    /* stack heap capacity */
                 NULL,        		    /* param  */
                 2,           		    /* priority  */
                 &xHandleTaskDecoderPro);   /* task handler  */


   xTaskCreate( vTaskCommPro,    		/* fucntion name  */
                 "vTaskCommPro",  		/* alias name   */
                 128,         		    /* stack heap capacity */
                 NULL,        		    /* param  */
                 2,           		    /* priority  */
                 &xHandleTaskCommPro);   /* task handler  */



	xTaskCreate( vTaskUiPro,    		/* fucntion name  */
                 "vTaskUiPro",  		/* alias name   */
                 128,         		    /* stack heap capacity */
                 NULL,        		    /* param  */
                 1,           		    /* priority  */
                 &xHandleTaskUiPro);   /* task handler  */

	
	xTaskCreate( vTaskKeyPro,     		/* function name*/
                 "vTaskKeyPro",   		/* alias */
                 128,            		/* stack heap capacity*/
                 NULL,           		/* param*/
                 2,              		/* priority */
                 &xHandleTaskKeyPro );   /* task handler  */

	#else

	/*------------------ 静态任务创建 ------------------*/

	xHandleTaskCommPro = xTaskCreateStatic(
			vTaskCommPro,			/* 任务函数 */
			"vTaskCommPro",			/* 任务名 */
			256,					/* 栈大小（word） */
			NULL,					/* 参数 */
			2,						/* 优先级 */
			xTaskCommProStack,		/* 栈数组 */
			&xTaskCommProTCB 		/* TCB */
	);

	
	
	xHandleTaskUiPro = xTaskCreateStatic(
			vTaskUiPro,			/* 任务函数 */
			"vTaskUiPro",			/* 任务名 */
			512,					/* 栈大小（word） */
			NULL,					/* 参数 */
			1,						/* 优先级 */
			xTaskUiProStack,		/* 栈数组 */
			&xTaskUiProTCB 		/* TCB */
	);
	
	xHandleTaskKeyPro = xTaskCreateStatic(
			vTaskKeyPro, 			/* 任务函数 */
			"vTaskKeyPro",			/* 任务名 */
			128,					/* 栈大小（word） */
			NULL,					/* 参数 */
			2,						/* 优先级 */
			xTaskKeyProStack,		/* 栈数组 */
			&xTaskKeyProTCB			/* TCB */
	);



	#endif 
}
/**********************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
***********************************************************************/
//static void AppObjCreate (void)
//{
//	/* 创建后信号量处于“空”状态（计数为0） */ 
//	xBinarySemaphore = xSemaphoreCreateBinaryStatic(&xBinarySemaphoreBuffer); 
//	if (xBinarySemaphore == NULL) {
//		/* 创建失败处理 */ 
//	    error_counter++;
//	}
//}
/*************************************************************************
*
*	Funtion Name: static void key_handler(void)
*	Function: 
*	Input Ref: 
*	Return Ref:
*
**************************************************************************/
static void key_handler(void)
{
  if(gl_ref.key_power_flag == 1 && gl_ref.long_key_power_counter!=1 && KEY_POWER_GetValue()  ==KEY_UP){ //key power key

                
               gl_ref.key_power_flag++;
			
               gl_ref.long_key_power_counter=0;
               power_on_off_handler();
             

        
   }
   else if(gl_ref.key_mode_flag == 1){
                
		if(KEY_MODE_GetValue() == KEY_UP){
			gl_ref.key_mode_flag++;

			if(gl_ref.key_long_mode_flag ==1){

			gpro_t.gTimer_mode_key_long=0;
			gl_ref.long_key_mode_counter=0;


			mode_key_long_fun();


		}
		else{

		gl_ref.key_long_mode_flag =0;
		gl_ref.long_key_mode_counter=0;
		
		SendData_Buzzer();
		vTaskDelay(100);
		
		gl_ref.key_mode_short_flag =1;


		}
	}
   }
   else if((gl_ref.key_add_flag ==1 || gl_ref.key_dec_flag ==1)&&run_t.power_on== power_on){
                

              if(gl_ref.key_add_flag == 1){

                 
               if(KEY_ADD_GetValue() == KEY_UP){
                  gl_ref.key_add_flag ++;
              
                  
                   SendData_Buzzer_Has_Ack();//SendData_Buzzer();
                   vTaskDelay(100);
                   add_key_fun();
				   
                }
              

              }
              else if(gl_ref.key_dec_flag == 1){
               
                if(KEY_DEC_GetValue()==KEY_UP){
                    gl_ref.key_dec_flag ++;
               
                   SendData_Buzzer_Has_Ack();//SendData_Buzzer();
                   vTaskDelay(100);
   
                
                   dec_key_fun();
				  
				   
                }
            } 
    }


}
/*************************************************************************
*
*	Funtion Name: static void power_run_handler(void)
*	Function: 
*	Input Ref: 
*	Return Ref:
*
**************************************************************************/
static void power_run_handler(void)
{
     static uint8_t power_counter;
     switch(run_t.power_on){

	 case power_on:
         

          if(gl_ref.key_mode_short_flag ==1){
            gl_ref.key_mode_short_flag ++ ;
            mode_key_short_fun();
            display_ai_icon(run_t.gModel) ;
		  }
          else if( gpro_t.gTimer_mode_key_long > 1 && (gl_ref.key_long_mode_flag  ==1 ||gl_ref.key_long_power_flag ==1)){
                 gl_ref.long_key_mode_counter =0;
                 gl_ref.long_key_power_counter =0;
         
                if(gl_ref.key_long_power_flag ==1){

                     gl_ref.key_long_power_flag=0;
                     
                }
                if(gl_ref.key_long_mode_flag==1){
                    gl_ref.key_long_mode_flag=0;

                 }

            }

          
	       power_on_handler();
		   
	       disp_fan_leaf_run_icon(); //Display time and fan of leaf integration
            //data:2026.01.19 wt.edit 
		   	if(gpro_t.gTimer_disp_dry_counter> 0 && gpro_t.temp_key_set_value==0 && gpro_t.set_up_temp_value_done != 1){
			   
		 	   gpro_t.gTimer_disp_dry_counter=0;

               if(gpro_t.power_on_counter_temp == 0){
                  gpro_t.power_on_counter_temp++;
				   gpro_t.temp_real_value= 25;
			   }
		        Display_Kill_Dry_Ster_Icon();

              
              
            }
	     
       
	 break;
	 
	 case power_off:
           power_counter=0;
           gl_ref.long_key_power_counter =0;
           gl_ref.key_long_power_flag =0;
           run_t.power_on_disp_smg_number = 0;
		   gpro_t.gTimer_two_hours_conter=0; //WT.EDIT 2025.10.30
		   gpro_t.stopTwoHours_flag=0;
           power_off_handler();
	     
		   
		    if(gpro_t.again_confirm_power_off_flag == 1){
				
				SendData_Set_Command(0x30,0); //mainboard.WT.EDIT 2026.01.04
                vTaskDelay(pdMS_TO_TICKS(100)); //WT.EDIT 2026.01.04
			    gpro_t.again_confirm_power_off_flag++;

		    }

       
	 break;

     }
}





void semaphore_isr(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	#if 0
	/* 发送同步信号 */
	xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);

	/* 如果xHigherPriorityTaskWoken = pdTRUE，那么退出中断后切到当前最高优先级任务执行 */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//	#if DEBUG_FLAG
//    printf("ISR fired!\r\n");
	#endif
	 xTaskNotifyFromISR(xHandleTaskCommPro,  /* ???? */
                DECODER_BIT_9,     /* ???????????bit0  */
                eSetBits,  /* ????????????BIT_0?????, ??????????? */
                &xHigherPriorityTaskWoken);

	 portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}





