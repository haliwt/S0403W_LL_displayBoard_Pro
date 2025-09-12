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
											ÄºÂ‡ËÄ‡Â•Â°ÄºÅÂ°Ä‡Â˜ÂŽ
***********************************************************************************************************/
static void vTaskRunPro(void *pvParameters);
static void vTaskDecoderPro(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void AppTaskCreate(void);



/* creat task communication  */
//static void AppObjCreate(void);


/***********************************************************************************************************
											ÄºÂÂ˜Ã©Â‡ÂÄºÅÂ°Ä‡Â˜ÂŽ
***********************************************************************************************************/
static TaskHandle_t xHandleTaskRunPro = NULL;
static TaskHandle_t xHandleTaskDecoderPro= NULL;
static TaskHandle_t xHandleTaskStart = NULL;


//TimerHandle_t           Timer1Timer_Handler;/* ÄºÅ½ÂšÄ‡Â—Å›ÄºÂ™Â„1Â¤71ÄºÂÄ½Ä‡ÂŸÂ„ */
//TimerHandle_t           Timer2Timer_Handler;/* ÄºÅ½ÂšÄ‡Â—Å›ÄºÂ™Â„1Â¤72ÄºÂÄ½Ä‡ÂŸÂ„ */

//void Timer1Callback(TimerHandle_t xTimer);  /* ÄºÅ½ÂšÄ‡Â—Å›ÄºÂ™Â„1Â¤71ÄÅ›Â…Ä‡Â—Å›ÄºÂ›ÂžÄÂ°ÂƒÄºÂ‡ËÄ‡Â•Â° */
//void Timer2Callback(TimerHandle_t xTimer);  /* ÄºÅ½ÂšÄ‡Â—Å›ÄºÂ™Â„1Â¤72ÄÅ›Â…Ä‡Â—Å›ÄºÂ›ÂžÄÂ°ÂƒÄºÂ‡ËÄ‡Â•Â° */
//static  QueueHandle_t xUartRxQueue = NULL;


typedef struct GL_TASK{

   uint8_t  long_key_mode_counter;
   uint8_t  long_key_power_counter;
   uint8_t  key_long_power_flag;
   uint8_t  key_long_mode_flag;
   uint8_t  key_power_flag;
   uint8_t  key_mode_flag ;
   uint8_t  key_dec_flag;
   uint8_t  key_add_flag;
   uint8_t  mode_sound;
   uint8_t smart_phone_app_timer_power_on_flag;
   uint8_t app_power_off_flag;


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
     /* ÄºÂˆÂ›ÄºÅ¥ÅŸÃ¤Å¥Å¥ÄºÂŠÄ„ */
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
static void vTaskDecoderPro(void *pvParameters)
{
	BaseType_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(300); /* ï¿?0ï¿?7ï¿½ï¿½ï¿?0ï¿?0ï¿?0ï¿?1ï¿½ï¿½ï¿?0ï¿?6ï¿?0ï¿?7ï¿½ï¿½ï¿?0ï¿?8ï¿?0ï¿?6ï¿?0ï¿?7ï¿?0ï¿?5ï¿?0ï¿?8ï¿½ï¿½ï¿?0ï¿?4ï¿?0ï¿?1ï¿?0ï¿?2ï¿?0ï¿?9300ms */
	//uint8_t ucQueueMsgValue[10];
	 uint32_t ulValue;
	
    while(1)
    {
	  xResult = xTaskNotifyWait(0x00000000,      
						          0xFFFFFFFF,      
						          &ulValue,        /*  */
						          portMAX_DELAY);  /* block times,releas cpu power right */
		
        if( xResult == pdPASS )
        {
            if((ulValue & POWER_KEY_BIT_0) != 0)
            {

            gpro_t.smart_phone_power_on = 1;
            gl_ref.key_long_power_flag=0;
            gl_ref.long_key_power_counter=0;

            }
            else if((ulValue & POWER_ON_BIT_5) != 0){

                gl_ref.smart_phone_app_timer_power_on_flag=1;

            }
            else if((ulValue & POWER_OFF_BIT_4) != 0){

                gl_ref.app_power_off_flag =1;


            }
            else if((ulValue & DECODER_BIT_9) != 0){
               parse_recieve_data_handler();
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
static void vTaskRunPro(void *pvParameters)
{
   // BaseType_t xResult;
	//const TickType_t xMaxBlockTime = pdMS_TO_TICKS(30); //40//30/* ÄÅ½Å¾Ã§ËÅ½Ä‡ÂœË˜ÄÅ¼ËÄºÂ¤Â§Ã§Â­Â‰ÄºÅ¾Â…Ä‡Â—Å›Ã©Â—Â´Ã¤Â¸ÅŸ30ms */
	//uint32_t ulValue;
    
    while(1)
    {
		if( gl_ref.key_power_flag == 1){ //key power key

            if(KEY_POWER_GetValue()  ==KEY_UP){
               gl_ref.key_power_flag++;
                

             if(gl_ref.key_long_power_flag ==1){ //WIFI KEY FUNCTION
                //  power_key_long_fun();
                 gl_ref.long_key_power_counter=0; //WT.EDIT 2025.05.10
                 gpro_t.send_ack_cmd = ack_wifi_on;
                 gpro_t.gTimer_again_send_power_on_off =0;
                 SendData_Set_Command(0x05,0x01); // link wifi of command .
                 osDelay(3);
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
                      

                        mode_key_long_fun();


                    }
					else{
					    gl_ref.long_key_mode_counter=0;
										
					    SendData_Buzzer();
										 
						gl_ref.mode_sound =1;

                              
					}
               

                 }
        }
        else if(gpro_t.smart_phone_power_on == 1){
                gpro_t.smart_phone_power_on ++;
                run_t.power_on= power_on;
                power_on_key_short_fun();

        }
        else if(gl_ref.smart_phone_app_timer_power_on_flag ==1){
                gl_ref.smart_phone_app_timer_power_on_flag++;
                 run_t.power_on= power_on;
                gl_ref.long_key_power_counter =0;
                run_t.power_on_disp_smg_number = 0;
                gpro_t.gTimer_again_send_power_on_off =0;
                power_on_key_short_fun();
                 

        }
        else if(gl_ref.app_power_off_flag ==1){
                gl_ref.app_power_off_flag++;
               
                run_t.power_on= power_off;
        }
        else if((gl_ref.key_add_flag ==1 || gl_ref.key_dec_flag ==1)&&run_t.power_on== power_on){
                

              if(gl_ref.key_add_flag == 1){

                 
               if(KEY_ADD_GetValue() == KEY_UP){
                  gl_ref.key_add_flag ++;
              
                  
                   SendData_Buzzer_Has_Ack();//SendData_Buzzer();
                   osDelay(3);//HAL_Delay(5);
                   add_key_fun();
                }
              

              }
              else if(gl_ref.key_dec_flag == 1){
               
                if(KEY_DEC_GetValue()==KEY_UP){
                    gl_ref.key_dec_flag ++;
               
                   SendData_Buzzer_Has_Ack();//SendData_Buzzer();
                   osDelay(3);
   
                
                   dec_key_fun();
                }
            } 
        }


        if(run_t.power_on== power_on){

         //  test_n++;

           if(gl_ref.mode_sound == 1){
               gl_ref.mode_sound++;

              //SendData_Buzzer_Has_Ack();//SendData_Buzzer(); 
              //osDelay(5);//20
             // gpro_t.send_ack_cmd = ack_buzzer_sound;
              gpro_t.gTimer_again_send_power_on_off=0;
              
             
            
              mode_key_short_fun();//SendData_Buzzer();

           }
           else if(gl_ref.mode_sound==2){
                 gl_ref.mode_sound ++ ;
                  
                 mode_key_ai_mode_handler();

           }
         

           if( gpro_t.gTimer_mode_key_long > 1 && (gl_ref.key_long_mode_flag  ==1 ||gl_ref.key_long_power_flag ==1)){
                 gl_ref.long_key_mode_counter =0;
                 gl_ref.long_key_power_counter =0;
         
                if(gl_ref.key_long_power_flag ==1){

                     gl_ref.key_long_power_flag=0;
                     
                }
                if(gl_ref.key_long_mode_flag==1){
                    gl_ref.key_long_mode_flag=0;

                 }

            }

          
       power_on_ref_init();
       disp_temp_humidity_wifi_icon_handler();

       display_timer_and_beijing_time_handler();
       disp_fan_leaf_run_icon(); //Display time and fan of leaf integration

       set_temperature_compare_value_fun();

	   
      
	
      // printf("xTaskRunPro !!!\r\n");
       }
       else if(run_t.power_on== power_off){
          gl_ref.long_key_power_counter =0;
           gl_ref.key_long_power_flag =0;
           run_t.power_on_disp_smg_number = 0;
           power_off_handler();

       }
       send_cmd_ack_hanlder() ; 
     
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

         if(gl_ref.long_key_power_counter > 80 && run_t.power_on== power_on ){
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

          if(gl_ref.long_key_mode_counter > 80  && run_t.power_on== power_on &&  run_t.ptc_warning ==0 && run_t.fan_warning ==0){
             gl_ref.long_key_mode_counter=0;   
         
             gl_ref.key_long_mode_flag =1;
               gpro_t.gTimer_mode_key_long = 0;
            
                SendData_Buzzer();
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
   xTaskCreate( vTaskDecoderPro,    		/* fucntion name  */
                 "vTaskDecoderPro",  		/* alias name   */
                 128,         		    /* stack heap capacity */
                 NULL,        		    /* param  */
                 3,           		    /* priority  */
                 &xHandleTaskDecoderPro);   /* task handler  */


	xTaskCreate( vTaskRunPro,    		/* fucntion name  */
                 "vTaskRunPro",  		/* alias name   */
                 256,         		    /* stack heap capacity */
                 NULL,        		    /* param  */
                 1,           		    /* priority  */
                 &xHandleTaskRunPro);   /* task handler  */

	
	xTaskCreate( vTaskStart,     		/* function name*/
                 "vTaskStart",   		/* alias */
                 128,            		/* stack heap capacity*/
                 NULL,           		/* param*/
                 2,              		/* priority */
                 &xHandleTaskStart );   /* task handler  */
}
 

/*
*********************************************************************************************************
*	Funtion Name: AppObjCreate
*	Function: 
*	Input Ref: 
*	Return Ref: 
*********************************************************************************************************
*/
//static void AppObjCreate (void)
//{
//	/* ï¿?0ï¿?7ï¿?0ï¿?7ï¿?0ï¿?5ï¿½ï¿½10ï¿?0ï¿?0ï¿?0ï¿?2uint8_tï¿?0ï¿?4ï¿?0ï¿?1ï¿?0ï¿?3ï¿?0ï¿?4ï¿?0ï¿?3ï¿?0ï¿?4ï¿?0ï¿?9ï¿?0ï¿?7ï¿?0ï¿?9ï¿?0ï¿?4 */
//	xUartRxQueue = xQueueCreate(20, sizeof(uint8_t));
//    if(xUartRxQueue == 0 )
//    {
//        /* creat quenu is fail !!! */
//    }
//	
//	/* ï¿?0ï¿?7ï¿?0ï¿?7ï¿?0ï¿?5ï¿½ï¿½10ï¿?0ï¿?0ï¿?0ï¿?2ï¿?0ï¿?7ï¿?0ï¿?3ï¿?0ï¿?7ï¿?0ï¿?4ï¿?0ï¿?0ï¿?0ï¿?0ï¿?0ï¿?9ï¿?0ï¿?5ï¿½ï¿½ï¿?0ï¿?1ï¿?0ï¿?9ï¿?0ï¿?7ï¿?0ï¿?8ï¿?0ï¿?2ï¿?0ï¿?3ï¿?0ï¿?4ï¿?0ï¿?3ï¿?0ï¿?4ï¿?0ï¿?9ï¿?0ï¿?7ï¿?0ï¿?9ï¿?0ï¿?4ï¿?0ï¿?5ï¿?0ï¿?1ï¿?0ï¿?7ï¿?0ï¿?7ï¿?0ï¿?7ï¿?0ï¿?3CM3/CM4ï¿?0ï¿?2ï¿?0ï¿?3ï¿?0ï¿?2ï¿?0ï¿?9ï¿?0ï¿?8ï¿?0ï¿?532ï¿?0ï¿?2ï¿?0ï¿?3ï¿?0ï¿?3ï¿½ï¿½ï¿?0ï¿?5ï¿?0ï¿?1ï¿?0ï¿?6ï¿?0ï¿?3ï¿?0ï¿?0ï¿?0ï¿?2ï¿?0ï¿?0ï¿?0ï¿?0ï¿?0ï¿?9ï¿?0ï¿?5ï¿½ï¿½ï¿?0ï¿?1ï¿?0ï¿?9ï¿?0ï¿?7ï¿?0ï¿?9ï¿?0ï¿?4ï¿?0ï¿?7ï¿?0ï¿?14ï¿?0ï¿?0ï¿?0ï¿?2ï¿½ï¿½ï¿?0ï¿?0ï¿?0ï¿?5ï¿?0ï¿?3 */
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
     
     xTaskNotify(xHandleTaskDecoderPro, /*  */
	 POWER_OFF_BIT_4 ,            /* bit0  */
	 eSetBits);             /* BIT_*/
     

}

void xTask_PowerOn_Handler(void)
{
     
     xTaskNotify(xHandleTaskDecoderPro,  
	 POWER_KEY_BIT_0 ,            /*  */
	 eSetBits);             /* */
     

}

void xtask_decoder_task_isr_handler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xTaskNotifyFromISR(xHandleTaskDecoderPro,  /*  */
						DECODER_BIT_9,     /*   */
						eSetBits,  /*  */
						&xHigherPriorityTaskWoken);
     portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

















