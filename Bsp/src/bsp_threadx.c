#include "bsp.h"

#define STACK_SIZE_ONE    1024 
#define STACK_SIZE_TWO    512
#define STACK_SIZE_THREE  512

static TX_THREAD  thread_msg;
static TX_THREAD  thread_key;
static TX_THREAD  thread_decoder;

static UCHAR stack_msg_pro[STACK_SIZE_ONE];
static UCHAR stack_key_pro[STACK_SIZE_TWO];
static UCHAR stack_decoder_pro[STACK_SIZE_THREE];

static void vTaskMsgPro(ULONG thread_input);
static void vTaskKeyPro(ULONG thread_input);
static void vTaskDecoderPro(ULONG thread_input);

TX_EVENT_FLAGS_GROUP   commEventFlags;


static void key_handler(void);

static void power_run_handler(void);

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
uint8_t error_counter,counter;


/**
*@brief 
*@param
*@notice
*@retval
**/
static void vTaskDecoderPro(ULONG thread_input)
{
   (void)thread_input;
   ULONG actual_flags;
   while(1){

    tx_event_flags_get(&commEventFlags,
						(1<<9),
						TX_OR_CLEAR,   /*获取后清除标志*/
						&actual_flags,
						TX_WAIT_FOREVER);

   if(actual_flags & (1<< 9)){
      counter ++ ;
      decoder_handler();
   }


   }

}




/**
*@brief 
*@param
*@notice
*@retval
**/
static void vTaskMsgPro(ULONG thread_input)
{
  (void)thread_input;
  while(1){
   key_handler();
       
		if(gpro_t.power_on_off_rx_flag ==1 && gpro_t.gTimer_power_off_on_minute_fan >1){
			    gpro_t.gTimer_power_off_on_minute_fan =0;
		     	SendData_Set_Command(0x10,1); //mainboard.WT.EDIT 2026.01.04
                tx_thread_sleep(5); //WT.EDIT 2026.01.04

        }
		power_run_handler();
		
	
        tx_thread_sleep(20);//60
   
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
  while(1){
	 if(KEY_POWER_GetValue()  ==KEY_DOWN){

          if(power_on_key ==0){
              power_on_key ++;
             
          }
          else{
           gl_ref.long_key_mode_counter =0;
          

         if(gl_ref.long_key_power_counter < 150 && run_t.power_on== power_on ){//65
            gl_ref.long_key_power_counter++;

		    if(gl_ref.long_key_power_counter > 85){
	            gl_ref.long_key_power_counter =200;
	            gl_ref.key_long_power_flag =1;
	           // gpro_t.gTimer_mode_key_long = 0;

			    SendData_Set_Command(0x05,0x01); // link wifi of command .
	            tx_thread_sleep(10);
	           // gpro_t.gTimer_mode_key_long=0;
				gl_ref.key_power_flag = 0;
		   }
         }
	        if(gl_ref.long_key_power_counter ==200)gl_ref.key_power_flag = 3;
	        else gl_ref.key_power_flag = 1;
        }
    }
    else if(KEY_MODE_GetValue() ==KEY_DOWN && run_t.power_on== power_on){

           gl_ref.long_key_power_counter=0;
         
         if(run_t.ptc_warning ==0 && run_t.fan_warning ==0 && gl_ref.long_key_mode_counter < 150){
	        gl_ref.long_key_mode_counter ++ ;

          if(gl_ref.long_key_mode_counter > 65 ){
             gl_ref.long_key_mode_counter=200;   
         
                mode_key_long_fun();
                SendData_Buzzer();
				tx_thread_sleep(5);
           }
          }

         if(gl_ref.long_key_mode_counter==200) gl_ref.key_mode_flag  = 3;
		 else gl_ref.key_mode_flag  = 1;
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

	tx_thread_sleep(20);
     

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

  tx_event_flags_create(&commEventFlags,"commEventFlags");

  tx_thread_create(&thread_decoder,
  					"DecoderPro",
  					vTaskDecoderPro,
  					0,
  					stack_decoder_pro,
  					STACK_SIZE_THREE,
  					1,
  					1,
  					TX_NO_TIME_SLICE,
  					TX_AUTO_START);


   tx_thread_create(&thread_msg,
   					"MsgPro",
   					vTaskMsgPro,
   					0,
   					stack_msg_pro,
   					STACK_SIZE_ONE,
   					3,
   					3,
   					TX_NO_TIME_SLICE,
   					TX_AUTO_START);

	tx_thread_create(&thread_key,
					"KeyPro",
					vTaskKeyPro,
					0,
					stack_key_pro,
					STACK_SIZE_TWO,
					2,
					2,
					TX_NO_TIME_SLICE,
					TX_AUTO_START);
   
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
  if(gl_ref.key_power_flag == 3 && KEY_POWER_GetValue() ==KEY_UP){ //key power key
     gl_ref.key_power_flag++;
	 gl_ref.long_key_power_counter=0;
  }
  else if(gl_ref.key_power_flag == 1 && KEY_POWER_GetValue()  ==KEY_UP){ //key power key

               gl_ref.key_power_flag++;
			   gl_ref.long_key_power_counter=0;
               gl_ref.long_key_mode_counter=0;
               power_on_off_handler();
             
 }
 else if(gl_ref.key_mode_flag ==3 &&  KEY_MODE_GetValue() == KEY_UP){
			gl_ref.key_mode_flag ++;
			
			gl_ref.long_key_mode_counter=0;
            gl_ref.long_key_power_counter=0;
            

 }
 else if(gl_ref.key_mode_flag == 1 && KEY_MODE_GetValue() == KEY_UP){
                gl_ref.key_mode_flag++;

	    gl_ref.long_key_mode_counter=0;
        gl_ref.long_key_power_counter=0;
		gl_ref.key_mode_short_flag =1;
		SendData_Buzzer();
		tx_thread_sleep(5);
		
		
   }
   else if((gl_ref.key_add_flag ==1 || gl_ref.key_dec_flag ==1)&&run_t.power_on== power_on){
                
			  

              if(gl_ref.key_add_flag == 1){

                 
               if(KEY_ADD_GetValue() == KEY_UP){
                  gl_ref.key_add_flag ++;
              
                  
                   SendData_Buzzer_Has_Ack();//SendData_Buzzer();
                   tx_thread_sleep(5);
                   add_key_fun();
				   
                }
              

              }
              else if(gl_ref.key_dec_flag == 1){
               
                if(KEY_DEC_GetValue()==KEY_UP){
                    gl_ref.key_dec_flag ++;
               
                   SendData_Buzzer_Has_Ack();//SendData_Buzzer();
				   
                   tx_thread_sleep(5);
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
     static uint8_t counter;
     switch(run_t.power_on){

	 case power_on:
         

          if(gl_ref.key_mode_short_flag ==1){
            gl_ref.key_mode_short_flag ++ ;
            mode_key_short_fun();
            display_ai_icon(run_t.gModel) ;
		  }


           power_on_handler();
		   
	       disp_fan_leaf_run_icon(); //Display time and fan of leaf integration
	       disp_time_colon_fun();
	       while(run_t.power_off_id_flag == 1){
		   	 run_t.power_off_id_flag++;
			 Display_Kill_Dry_Ster_Icon();
	          power_on_first_again_fun();
           }
            //data:2026.01.19 wt.edit 
		   	if(gpro_t.gTimer_disp_dry_counter> 0 && gpro_t.temp_key_set_value==0 && gpro_t.set_up_temp_value_done != 1){
			   
		 	   gpro_t.gTimer_disp_dry_counter=0;

               if(gpro_t.power_on_counter_temp == 0){
                  gpro_t.power_on_counter_temp++;
				   gpro_t.temp_real_value= 25;
			   }
			     counter_time_numbers();
		        Display_Kill_Dry_Ster_Icon();

            }
	     
       
	 break;
	 
	 case power_off:
          
           gl_ref.long_key_power_counter =0;
           gl_ref.key_long_power_flag =0;
           run_t.power_on_disp_smg_number = 0;
		   gpro_t.gTimer_two_hours_conter=0; //WT.EDIT 2025.10.30
		   gpro_t.stopTwoHours_flag=0;
           power_off_handler();
	       gpro_t.fan_run_one_minute=0;

		   counter ++ ;
		   
		    if(gpro_t.again_confirm_power_off_flag == 1 && counter > 40 ){
				counter =0;
				SendData_Set_Command(0x10,0); //mainboard.WT.EDIT 2026.01.04
                tx_thread_sleep(10); //WT.EDIT 2026.01.04
			   // gpro_t.again_confirm_power_off_flag++;

		    }

			if(gpro_t.gTimer_power_off_on_minute_fan > 60){

			    gpro_t.again_confirm_power_off_flag++;
			    SendData_Set_Command(0x12,1); //turn off fun .mainboard.WT.EDIT 2026.01.04
			    tx_thread_sleep(10); //WT.EDIT 2026.01.04


			}

			if(gpro_t.gTimer_send_data_counter > 1){ //new version 
			 	 gpro_t.gTimer_send_data_counter =0;
				 SendData_Set_Command(0xF0,0x02);//software version is "2"
				 tx_thread_sleep(10);

             }

       
	 break;

     }
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
   tx_event_flags_set(&commEventFlags,(1<<9),TX_OR);
}


void tx_application_stack_error_handler(TX_THREAD *thread_ptr)
{
  printf("stack overflow in thread:%s \n", thread_ptr->tx_thread_name );
}



