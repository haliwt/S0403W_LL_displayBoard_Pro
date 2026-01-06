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

/* vTaskMsgPro 任务 */
static StaticTask_t xTaskRunProTCB;
static StackType_t xTaskRunProStack[256];

/* vTaskStart 任务 */
static StaticTask_t xTaskStartTCB;
static StackType_t xTaskStartStack[128];


#endif 
static void AppTaskCreate(void);

/* 定义静态内存块 */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];


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



/***********************************************************************************************************
											ĺéĺŁ°ć
***********************************************************************************************************/
//static TaskHandle_t xHandleTaskDecoderPro= NULL;

static TaskHandle_t xHandleTaskRunPro = NULL;

static TaskHandle_t xHandleTaskStart = NULL;


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
   uint8_t  key_mode_short_flag ;
   uint8_t  key_power_flag;
   uint8_t  key_mode_flag ;
   uint8_t  key_dec_flag;
   uint8_t  key_add_flag;
   uint8_t   stopTwoHours_flag;

   uint8_t smart_phone_app_timer_power_on_flag;



}gl_task;

gl_task gl_ref;

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
#if 0
static void vTaskDecoderPro(void *pvParameters)
{
	//BaseType_t xResult;
	//const TickType_t xMaxBlockTime = pdMS_TO_TICKS(300); /* �?0�?7���?0�?0�?0�?1���?0�?6�?0�?7���?0�?8�?0�?6�?0�?7�?0�?5�?0�?8���?0�?4�?0�?1�?0�?2�?0�?9300ms */
	//uint8_t ucQueueMsgValue[10];
	// uint32_t ulValue;
	
    while(1)
    {
//	  xResult = xTaskNotifyWait(0x00000000,      
//						          0xFFFFFFFF,      
//						          &ulValue,        /*  */
//						          portMAX_DELAY);  /* block times,releas cpu power right */
		
//        if( xResult == pdPASS )
//        {
//            if((ulValue & POWER_KEY_BIT_0) != 0)
//            {
//            //run_t.power_on= power_on;
           
//            gl_ref.key_long_power_flag=0;
//            gl_ref.long_key_power_counter=0;

//			 run_t.power_on= power_on;
//             power_on_key_short_fun();

//            }
//            else if((ulValue & POWER_ON_BIT_5) != 0){

//                gl_ref.smart_phone_app_timer_power_on_flag=1;

//            }
//            else if((ulValue & POWER_OFF_BIT_4) != 0){

            
//				run_t.power_on= power_off;
//			    gl_ref.key_long_power_flag=0;
//				gl_ref.long_key_power_counter=0;


//            }
//            else 

//			if((ulValue & DECODER_BIT_9) != 0){
//               parse_recieve_data_handler();
//            }
         
//        }
//	}

	if( gpro_t.decoder_flag ==1 ){
	        
			parse_decoder_handler();
		 
		 //   parse_handler();
			
			gpro_t.decoder_flag=0;
	 }

	vTaskDelay(50);
   
    }	
}
#endif 

/**********************************************************************************************************
*	
*   Function Name:
*	Funciton:  proritiy class is small and protity is low
*	Input Ref:
*   Return Ref:
*
**********************************************************************************************************/
static void vTaskRunPro(void *pvParameters)
{
   // BaseType_t xResult;
	//const TickType_t xMaxBlockTime = pdMS_TO_TICKS(30); //40//30/* čŽžç˝Žć˘ďż˝ĺ¤§ç­ĺžćśé´ä¸ş30ms */
	//uint32_t ulValue;
    
    while(1)
    {
		key_handler();
        power_run_handler();
		//waiting_ack_handler();
		if( gpro_t.decoder_flag ==1 ){
	        
			parse_decoder_handler();
		 
		 //   parse_handler();
			
			gpro_t.decoder_flag=0;
	    }
        vTaskDelay(10);
    }
 }

/**********************************************************************************************************
*	Function Name: vTaskStart
*	Function: 
*	Input Ref: pvParameters
*	
*   
**********************************************************************************************************/
static void vTaskStart(void *pvParameters)
{
  static uint8_t power_on_key;
   while(1)
    {
      
    if(KEY_POWER_GetValue()  ==KEY_DOWN){

          if(power_on_key ==0){
              power_on_key ++;
             
          }
          else{
           gl_ref.long_key_mode_counter =0;
           gl_ref.long_key_power_counter++;

         if(gl_ref.long_key_power_counter >65 && run_t.power_on== power_on ){
            gl_ref.long_key_power_counter =0;
            gl_ref.key_long_power_flag =1;
            gpro_t.gTimer_mode_key_long = 0;
            
             SendData_Buzzer();
			
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
   #endif 

   #if 0

	xTaskCreate( vTaskRunPro,    		/* fucntion name  */
                 "vTaskRunPro",  		/* alias name   */
                 128,         		    /* stack heap capacity */
                 NULL,        		    /* param  */
                 1,           		    /* priority  */
                 &xHandleTaskRunPro);   /* task handler  */

	
	xTaskCreate( vTaskStart,     		/* function name*/
                 "vTaskStart",   		/* alias */
                 128,            		/* stack heap capacity*/
                 NULL,           		/* param*/
                 2,              		/* priority */
                 &xHandleTaskStart );   /* task handler  */

	#else

	/*------------------ 静态任务创建 ------------------*/
	
	xHandleTaskRunPro = xTaskCreateStatic(
			vTaskRunPro,			/* 任务函数 */
			"vTaskRunPro",			/* 任务名 */
			256,					/* 栈大小（word） */
			NULL,					/* 参数 */
			1,						/* 优先级 */
			xTaskRunProStack,		/* 栈数组 */
			&xTaskRunProTCB 		/* TCB */
	);
	
	xHandleTaskStart = xTaskCreateStatic(
			vTaskStart, 			/* 任务函数 */
			"vTaskStart",			/* 任务名 */
			128,					/* 栈大小（word） */
			NULL,					/* 参数 */
			2,						/* 优先级 */
			xTaskStartStack,		/* 栈数组 */
			&xTaskStartTCB			/* TCB */
	);



	#endif 
}
 

/*************************************************************************
*
*	Funtion Name: AppObjCreate
*	Function: 
*	Input Ref: 
*	Return Ref:
*
**************************************************************************/

//static void AppObjCreate (void)
//{
//	/* �?0�?7�?0�?7�?0�?5��10�?0�?0�?0�?2uint8_t�?0�?4�?0�?1�?0�?3�?0�?4�?0�?3�?0�?4�?0�?9�?0�?7�?0�?9�?0�?4 */
//	xUartRxQueue = xQueueCreate(20, sizeof(uint8_t));
//    if(xUartRxQueue == 0 )
//    {
//        /* creat quenu is fail !!! */
//    }
//	
//	/* �?0�?7�?0�?7�?0�?5��10�?0�?0�?0�?2�?0�?7�?0�?3�?0�?7�?0�?4�?0�?0�?0�?0�?0�?9�?0�?5���?0�?1�?0�?9�?0�?7�?0�?8�?0�?2�?0�?3�?0�?4�?0�?3�?0�?4�?0�?9�?0�?7�?0�?9�?0�?4�?0�?5�?0�?1�?0�?7�?0�?7�?0�?7�?0�?3CM3/CM4�?0�?2�?0�?3�?0�?2�?0�?9�?0�?8�?0�?532�?0�?2�?0�?3�?0�?3���?0�?5�?0�?1�?0�?6�?0�?3�?0�?0�?0�?2�?0�?0�?0�?0�?0�?9�?0�?5���?0�?1�?0�?9�?0�?7�?0�?9�?0�?4�?0�?7�?0�?14�?0�?0�?0�?2���?0�?0�?0�?5�?0�?3 */
//
//}


/****************************************************************************
 * 
 * Function Name: void App_PowerOnOff_Handler(void)
 * Function:
 * Input Ref: NO
 * Return Ref: NO
 * 
*****************************************************************************/
void xTask_PowerOff_Handler(void)
{
     
//     xTaskNotify(xHandleTaskDecoderPro, /*  */
//	 POWER_OFF_BIT_4 ,            /* bit0  */
//	 eSetBits);             /* BIT_*/
     

}

void xTask_PowerOn_Handler(void)
{
     
//     xTaskNotify(xHandleTaskDecoderPro,  
//	 POWER_KEY_BIT_0 ,            /*  */
//	 eSetBits);             /* */
     

}

void xtask_decoder_task_isr_handler(void)
{
//	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//	xTaskNotifyFromISR(xHandleTaskDecoderPro,  /*  */
//						DECODER_BIT_9,     /*   */
//						eSetBits,  /*  */
//						&xHigherPriorityTaskWoken);
//     portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


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
  if( gl_ref.key_power_flag == 1){ //key power key

            if(KEY_POWER_GetValue()  ==KEY_UP){
               gl_ref.key_power_flag++;
                

             if(gl_ref.key_long_power_flag ==1){ //WIFI KEY FUNCTION
        
                 gl_ref.long_key_power_counter=0; //WT.EDIT 2025.05.10
  
		
				  gpro_t.ack_cp_repeat_counter=0;
                 gpro_t.gTimer_cp_timer_counter =0;
                 SendData_Set_Command(0x05,0x01); // link wifi of command .
                 vTaskDelay(100);
                 gpro_t.gTimer_mode_key_long=0;
               


             }
             else{
			
                 gl_ref.long_key_power_counter=0;
                 power_on_off_handler();
             }

        }
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
						if(run_t.wifi_link_net_success == 0){	
                            SendData_Buzzer();
                            vTaskDelay(100);
                        }
                        gl_ref.key_mode_short_flag =1;
					   // mode_key_short_fun();
										 
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
	     
       
	 break;
	 
	 case power_off:
    
          gl_ref.long_key_power_counter =0;
           gl_ref.key_long_power_flag =0;
           run_t.power_on_disp_smg_number = 0;
		   gpro_t.gTimer_two_hours_conter=0; //WT.EDIT 2025.10.30
		    gpro_t.stopTwoHours_flag=0;
           power_off_handler();
	       waiting_ack_handler();

       
	 break;

     }
}










