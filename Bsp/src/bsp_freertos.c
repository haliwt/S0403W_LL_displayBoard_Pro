#include "bsp.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"


#define POWER_KEY_BIT_0	        (1 << 0)
#define MODE_KEY_1	        (1 << 1)
#define DEC_KEY_2           (1 << 2)
#define ADD_KEY_3           (1 << 3)

#define POWER_OFF_BIT_4        (1 << 4)
#define POWER_ON_BIT_5         (1<< 5)



#define MODE_LONG_KEY_8         (1 << 8)
#define DECODER_BIT_9          (1<< 9)


/***********************************************************************************************************
											暮聡藵膰聲掳暮艁掳膰聵聨
***********************************************************************************************************/
static void vTaskRunPro(void *pvParameters);
static void vTaskDecoderPro(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void AppTaskCreate(void);



/* creat task communication  */
//static void AppObjCreate(void);


/***********************************************************************************************************
											暮聫聵茅聡聫暮艁掳膰聵聨
***********************************************************************************************************/
static TaskHandle_t xHandleTaskRunPro = NULL;
static TaskHandle_t xHandleTaskDecoderPro= NULL;
static TaskHandle_t xHandleTaskStart = NULL;


//TimerHandle_t           Timer1Timer_Handler;/* 暮沤職膰聴艣暮聶聞1陇71暮聫慕膰聼聞 */
//TimerHandle_t           Timer2Timer_Handler;/* 暮沤職膰聴艣暮聶聞1陇72暮聫慕膰聼聞 */

//void Timer1Callback(TimerHandle_t xTimer);  /* 暮沤職膰聴艣暮聶聞1陇71膷艣聟膰聴艣暮聸聻膷掳聝暮聡藵膰聲掳 */
//void Timer2Callback(TimerHandle_t xTimer);  /* 暮沤職膰聴艣暮聶聞1陇72膷艣聟膰聴艣暮聸聻膷掳聝暮聡藵膰聲掳 */
//static  QueueHandle_t xUartRxQueue = NULL;


uint8_t mode_sound,key_long_mode_flag;

uint8_t long_key_mode_counter,long_key_mode_counter;

uint8_t dc_power_on_first;

uint8_t ucQueueMsgValue[20];

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
     /* 暮聢聸暮钮艧盲钮钮暮聤膭 */
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
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(300); /* ÉèÖÃ×î´óµÈ´ýÊ±¼äÎª300ms */
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
    	
     
        if((ulValue & DECODER_BIT_9) != 0){
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
    BaseType_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(30); //40//30/* 膷沤啪莽藵沤膰聹藰膹偶藵暮陇搂莽颅聣暮啪聟膰聴艣茅聴麓盲赂艧30ms */
	uint32_t ulValue;
    
    static volatile uint8_t power_on_off_flag,fan_on_off_flag,dc_power_on ;
    static uint8_t smart_phone_app_timer_power_on_flag,app_power_off_flag;
  //  static uint8_t mode_sound;
    while(1)
    {
		
	xResult = xTaskNotifyWait(0x00000000,      
						          0xFFFFFFFF,      
						          &ulValue,        /*  */
						          xMaxBlockTime);  /* block times,releas cpu power right */
		
	if( xResult == pdPASS )
	{
    	
        if((ulValue & POWER_KEY_BIT_0) != 0)
        {

            gpro_t.smart_phone_power_on = 1;
            gpro_t.key_long_power_flag=0;
            gpro_t.long_key_power_counter=0;

        }
        else if((ulValue & POWER_ON_BIT_5) != 0){

            smart_phone_app_timer_power_on_flag=1;

        }
        else if((ulValue & POWER_OFF_BIT_4) != 0){

            app_power_off_flag =1;


        }
//        else if((ulValue & DECODER_BIT_9) != 0){
//             parse_recieve_data_handler();
//        }


    }
    else{ 

        if( gpro_t.key_power_flag == 1){ //key power key

            if(KEY_POWER_GetValue()  ==KEY_UP){
                gpro_t.key_power_flag++;
                

             if(gpro_t.key_long_power_flag ==1){ //WIFI KEY FUNCTION
                //  power_key_long_fun();
                 gpro_t.long_key_power_counter=0; //WT.EDIT 2025.05.10
                 gpro_t.send_ack_cmd = ack_wifi_on;
                 gpro_t.gTimer_again_send_power_on_off =0;
                 SendData_Set_Command(0x05,0x01); // link wifi of command .
                 osDelay(3);
                 gpro_t.gTimer_mode_key_long=0;


             }
             else{
			 	gpro_t.long_key_power_counter=0; //WT.EDIT 2025.05.10
                power_on_off_handler();
             }
         }
        }
        else if(gpro_t.key_mode_flag == 1){
                
                  if(KEY_MODE_GetValue() == KEY_UP){
                      gpro_t.key_mode_flag++;

                    if(key_long_mode_flag ==1){

                        gpro_t.gTimer_mode_key_long=0;
                      

                        mode_key_long_fun();


                    }
               

                 }
        }
        else if(gpro_t.smart_phone_power_on == 1){
                gpro_t.smart_phone_power_on ++;
                run_t.power_on= power_on;
                power_on_key_short_fun();

        }
        else if(smart_phone_app_timer_power_on_flag ==1){
                smart_phone_app_timer_power_on_flag++;
                 run_t.power_on= power_on;
                gpro_t.long_key_power_counter =0;
                run_t.power_on_disp_smg_number = 0;
                gpro_t.gTimer_again_send_power_on_off =0;
                power_on_key_short_fun();
                 

        }
        else if(app_power_off_flag ==1){
                app_power_off_flag++;
               
                run_t.power_on= power_off;
        }
        else if((gpro_t.key_add_flag ==1 || gpro_t.key_dec_flag ==1)&&run_t.power_on== power_on){
                

              if(gpro_t.key_add_flag == 1){

                 
               if(KEY_ADD_GetValue() == KEY_UP){
                  gpro_t.key_add_flag ++;
               //   gl_tMsg.long_key_mode_counter =0;
                  
                   SendData_Buzzer_Has_Ack();//SendData_Buzzer();
                   osDelay(3);//HAL_Delay(5);
                   add_key_fun();
                }
              

              }
              else if(gpro_t.key_dec_flag == 1){
               
                if(KEY_DEC_GetValue()==KEY_UP){
                    gpro_t.key_dec_flag ++;
                 //   gl_tMsg.long_key_mode_counter =0;
                   SendData_Buzzer_Has_Ack();//SendData_Buzzer();
                   osDelay(3);
   
                
                   dec_key_fun();
                }
            } 
        }


        if(run_t.power_on== power_on){

         //  test_n++;

           if(mode_sound == 1){
               mode_sound++;

              SendData_Buzzer_Has_Ack();//SendData_Buzzer(); 
              gpro_t.send_ack_cmd = ack_buzzer_sound;
              gpro_t.gTimer_again_send_power_on_off=0;
              
              osDelay(10);//20
            
              mode_key_short_fun();//SendData_Buzzer();

           }
           else if(mode_sound==2){
                 mode_sound ++ ;
                  
                 mode_key_ai_mode_handler();

           }
         

           if( gpro_t.gTimer_mode_key_long > 1 && (key_long_mode_flag  ==1 ||gpro_t.key_long_power_flag ==1)){
                 long_key_mode_counter =0;
                 gpro_t.long_key_power_counter =0;
         
                if(gpro_t.key_long_power_flag ==1){

                     gpro_t.key_long_power_flag=0;
                     
                }
                if(key_long_mode_flag==1){
                    key_long_mode_flag=0;

                 }

            }

          
       power_on_ref_init();
       disp_temp_humidity_wifi_icon_handler();

       display_timer_and_beijing_time_handler();
       disp_fan_leaf_run_icon(); //Display time and fan of leaf integration

       set_temperature_compare_value_fun();

       }
       else if(run_t.power_on== power_off){
          gpro_t.long_key_power_counter =0;
           gpro_t.key_long_power_flag =0;
           run_t.power_on_disp_smg_number = 0;
           power_off_handler();

       }
       send_cmd_ack_hanlder() ; 
     

    }

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
           long_key_mode_counter =0;
           gpro_t.long_key_power_counter++;

         if(gpro_t.long_key_power_counter > 60 && run_t.power_on== power_on ){
            gpro_t.long_key_power_counter =0;
            gpro_t.key_long_power_flag =1;
            gpro_t.gTimer_mode_key_long = 0;
            
             SendData_Buzzer();
         }

        if(dc_power_on_first==0){

          dc_power_on_first++;

        }
        else{
            gpro_t.key_power_flag = 1;
			printf("key_power_on !!! \r\n");

        }
        
     }
    }
    else if(KEY_MODE_GetValue() ==KEY_DOWN){

           gpro_t.long_key_power_counter=0;
         
          long_key_mode_counter ++ ;

          if(long_key_mode_counter > 60  && run_t.power_on== power_on &&  run_t.ptc_warning ==0 && run_t.fan_warning ==0){
             long_key_mode_counter=0;   
         
             key_long_mode_flag =1;
               gpro_t.gTimer_mode_key_long = 0;
            
                SendData_Buzzer();
           }

         if(run_t.power_on== power_on){
            gpro_t.key_mode_flag  =  1;

          }


     }
     else if(KEY_DEC_GetValue() == KEY_DOWN){
          gpro_t.long_key_power_counter=0;
          long_key_mode_counter =0 ;
           if(run_t.power_on== power_on){ //WT.EDIT .2025.01.15
               gpro_t.key_dec_flag = 1;
            }
     }
     else if(KEY_ADD_GetValue() ==KEY_DOWN){

          gpro_t.long_key_power_counter=0;
         
          long_key_mode_counter =0 ;
         if(run_t.power_on== power_on){ //WT.EDIT .2025.01.15
             gpro_t.key_add_flag = 1;
         }

    }
    vTaskDelay(10);
     
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
//	/* ´´½¨10¸öuint8_tÐÍÏûÏ¢¶ÓÁÐ */
//	xUartRxQueue = xQueueCreate(20, sizeof(uint8_t));
//    if(xUartRxQueue == 0 )
//    {
//        /* creat quenu is fail !!! */
//    }
//	
//	/* ´´½¨10¸ö´æ´¢Ö¸Õë±äÁ¿µÄÏûÏ¢¶ÓÁÐ£¬ÓÉÓÚCM3/CM4ÄÚºËÊÇ32Î»»ú£¬Ò»¸öÖ¸Õë±äÁ¿Õ¼ÓÃ4¸ö×Ö½Ú */
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
void App_PowerOff_Handler(void)
{
     
     xTaskNotify(xHandleTaskRunPro, /*  */
	 POWER_OFF_BIT_4 ,            /* bit0  */
	 eSetBits);             /* BIT_*/
     

}

void App_PowerOn_Handler(void)
{
     
     xTaskNotify(xHandleTaskRunPro, /*  */
	 POWER_KEY_BIT_0 ,            /*  */
	 eSetBits);             /* */
     

}

void app_decoder_task_isr_handler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xTaskNotifyFromISR(xHandleTaskDecoderPro,  /*  */
						DECODER_BIT_9,     /*   */
						eSetBits,  /*  */
						&xHigherPriorityTaskWoken);
     portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


//void app_xusart1_queue_isr_handler(uint8_t data)
//{
//	 BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//    
//    // 生产者：将接收到的字节放入队列
//    xQueueSendFromISR(xUartRxQueue, &data, &xHigherPriorityTaskWoken);
//    
//    // 如果有更高优先级的任务被唤醒，则进行上下文切换
//    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//
//}
//















