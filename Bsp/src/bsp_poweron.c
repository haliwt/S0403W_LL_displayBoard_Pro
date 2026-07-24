#include "bsp.h"



// --- 1. 定义任务的时间周期（单位：毫秒，假设基础Tick为1ms） ---
#define PERIOD_DISP_TEMP_HUM       3    // 10ms*2 = 
#define PERIOD_DISP_BEIJING        10    // 10ms*200 = 2000ms = 2s

#define PERIOD_SET_TEMPERATURE     300    //  10ms*300 = 

#define PERIOD_TX_VERSION          80    //  10ms*80 = 
#define PERIOD_WORKS_HOURS         320    //   10ms * 400 = 
#define PERIOD_DISP_LEAF           2    //   10ms * 3 = 1
#define PERIOD_WIFI_ICON           100    //   10ms * 100 = 1300ms = 1.s


// --- 2. 定义分时任务控制结构体 ---
typedef struct {
    uint32_t last_tick;        // 记录上一次真正运行时的系统绝对时间戳
    //uint32_t counter;       // 时间计数器
    uint32_t period;        // 任务运行周期
    void (*task_handler)(void); // 任务函数指针
} TimeSharingTask_t;

static void handler_disp_temp_humidity_value(void);
static void handler_disp_beijing_time(void);
static void handler_set_temperature(void);

static void handler_works_hours(void);

static void handler_disp_fan_leaf(void);
static void handler_disp_wifi_icon(void);

static void handler_tx_version(void);

volatile uint8_t time_slot ;


// --- 4. 初始化分时任务表 ---
TimeSharingTask_t g_tasks[] = {
    {0, 33,          handler_disp_temp_humidity_value},//10ms*33= 330ms
    {0, 16,          handler_disp_beijing_time},
    {0, 340,         handler_set_temperature},
    {0, 320,         handler_works_hours},
    {0, 10,          handler_disp_fan_leaf},//10ms*10 
    {0, 98,          handler_disp_wifi_icon}
  
};

#define TASK_NUM (sizeof(g_tasks) / sizeof(TimeSharingTask_t))


static void power_off_breath_Led(void);


static void power_on_ref_init(void);


static void display_lcd_Icon_init(void);
static void power_on_initial(void);
static void power_on_cycle(void);


static uint8_t send_two_disp =0;



/*************************************************************************
*
*	Funtion Name: static void power_run_handler(void)
*	Function: 
*	Input Ref: 
*	Return Ref:
*
**************************************************************************/
void power_run_handler(void)
{
     static uint8_t counter;
     switch(run_t.power_on){

	 case power_on:
	 	  
          
           power_on_handler();
		   disp_time_colon_fun();
	       wifi_icon_blink_faster_handler();


       
	 break;
	 
	 case power_off:
          

         
		   gpro_t.gTimer_two_hours_conter=0; //WT.EDIT 2025.10.30
		   gpro_t.stopTwoHours_flag=0;
		   gpro_t.first_ptc_on=0;
           power_off_handler();
	       gpro_t.fan_run_one_minute=0;

		   if(gpro_t.gTimer_send_data_counter > 1){ //new version 
			 	 gpro_t.gTimer_send_data_counter =0;
				 SendData_Set_Command(0xF0,0x02);//software version is "2"
				 //tx_thread_sleep(2);

             }

			
	         if(lcd_t.gTimer_colon_counter > 1 ){
					lcd_t.gTimer_colon_counter  =0;
					SendData_Set_Command(0x11,1); //mainboard.WT.EDIT 2026.04.23
					//tx_thread_sleep(2); //WT.EDIT 2026.01.04
				   
	 
			  }

       
	 break;

     }
}


/*
	*@brief :
	*@param:
	*@retval:
*/
void power_on_handler(void)
{


	if(gpro_t.power_on_step < 10){
	   power_on_initial();
    }
	else 
		power_on_cycle();


}

/*
	*@brief :
	*@param:
	*@retval:
*/
static void power_on_initial(void)
{
   static uint8_t dc_disp=0,i;
   uint32_t boot_tick;
   switch(gpro_t.power_on_step){

      case 0:
	  	
          power_on_ref_init();
	      gpro_t.key_set_temperature =0;
          gpro_t.power_on_step =1;
          gpro_t.gTimer_two_hours_second_counter=0;
		  gpro_t.gTimer_two_hours_conter=0;
		  gpro_t.stopTwoHours_flag=0;

		   // gpro_t.long_key_power_counter =0; 
 
	   //copy
	      gpro_t.fan_run_one_minute=0;
  

        
		//end
        run_t.wifi_set_temperature=40; //WT.EDIT 2025.01.15
        run_t.time_setting_mode = works_time;//WT.EDIT 2025.01.15
        run_t.smart_phone_set_temp_value_flag =0;//WT.EDIT 2025.01.15
        gpro_t.set_temp_value_success=0;//WT.EDIT 2025.01.15
        
		gpro_t.first_set_ptc_on =0;
		gpro_t.first_rcoder_ptc_on_flag=0;
        
       
     
	     gpro_t.power_on_step =1;

   

      break;

	  case 1:

	     gpro_t.gTimer_disp_temp_humi_value=20;
	     run_t.wifi_set_temperature=40;

		 if(dc_disp==0){
			dc_disp++;

            display_lcd_Icon_init();
		    //tx_thread_sleep(100);//stop 1s.
		 }
         else 
		 	dsiplay_numbers_one_to_four_fun();

		 disp_time_four_numbers_init();

#if 1	
        boot_tick = tx_time_get();
		for(i=0;i < TASK_NUM;i ++){

		     g_tasks[i].last_tick = boot_tick;
		}
#endif 
		 
		 gpro_t.power_on_step =0xfe;

		 break;
  }
 }

/*
	*@brief :
	*@param:
	*@retval:
*/
static void power_on_cycle(void)
{

    // 获取当前系统的绝对时间戳
      uint32_t current_tick = tx_time_get();
	
        // 通过时间片轮询核心算法，分时调用各个功能模块
    for (uint8_t i = 0; i < TASK_NUM; i++) 
    {
        if ((current_tick - g_tasks[i].last_tick) >= g_tasks[i].period) 
        {

//            // 【关键对齐】：将配置表的 ms 转换为当前硬件环境的 Tick 数
//            // 既然 1 Tick = 10ms，那么 Tick数 = ms / 10
//            uint32_t period_tick = g_tasks[i].period_ms / 10;
        
//	        // 防止配置错误：如果误填了小于 10ms 的周期，强制算作 1 个 Tick
//	        if (period_tick == 0) {
//	            period_tick = 1; 
//	        }

		   // 【工业级进化：防轰炸饱和截断】
            // 如果卡顿/被高优先级抢占的时间超过了 2 个周期，直接对齐当前时间，放弃追赶
            if ((current_tick - g_tasks[i].last_tick) > (g_tasks[i].period * 2)) 
            {
                g_tasks[i].last_tick = current_tick;
            }
            else 
            {
                // 如果只是正常范围内的轻微抖动，滚动累加周期，死锁锁相，消除长期长跑漂移
                g_tasks[i].last_tick += g_tasks[i].period;
            }
            
            // 触发对应周期的执行函数（确保不为 NULL，防止空指针崩溃）
            if (g_tasks[i].task_handler != NULL)
            {
                g_tasks[i].task_handler(); 
            }
        }
    }
	

}


#if 0
static void power_on_cycle(void)
{
    static uint8_t version;
    static uint8_t time_slot=0;
	
	switch(time_slot){
		
	  case 0:
  
	    disp_temp_humidity_wifi_icon_handler();
	   

	  break;

	  case 1:
	  	display_timer_and_beijing_time_handler();
       
	  break;

	
     case 2:
	 	
     if(gpro_t.key_set_temperature==0 && gpro_t.gTimer_temp_compare_value > 2 && gpro_t.stopTwoHours_flag==0 && gpro_t.smart_phone_app_timer_power_on_flag ==0){
	 	gpro_t.gTimer_temp_compare_value =0;
		
          set_temperature_compare_value_fun();

     	}
	  

	 break;

	 case 3:
	 	
		 if(send_two_disp < 5 ){
			 send_two_disp++;

		    version = version ^ 0x01;
		     if(version ==1){
			  SendData_Set_Command(0xF0,0x02);//software version is "2"
			  //tx_thread_sleep(2);

			 }
			 else{
			 	SendData_Set_Command(0x11,0x01);
			 	//tx_thread_sleep(2);
			 }

			 
		 }
		

	break;
		 
	case 4:

	    two_hours_recoder_fun();

	break;

	 case 5:
	 if(gpro_t.smart_phone_app_timer_power_on_flag ==1 && run_t.gTimer_ptc_fan_warning >6){
	      gpro_t.smart_phone_app_timer_power_on_flag=0;


	  }
	 
      disp_fan_leaf_run_icon();
	 break;

	 case 6:
          wifi_icon_blink_reg0xc5_handler();

	 break;
	 
	 default:
	 	break;
	 }
	
   time_slot ++;
   if(time_slot > 6) time_slot = 0;// 10ms * 7 = 90ms
 }

#endif
/*
	*@brief :
	*@param:
	*@retval:
*/
static void handler_disp_temp_humidity_value(void)
{

	disp_temp_humidity_wifi_icon_handler();

}
/*
	*@brief :
	*@param:
	*@retval:
*/
static void handler_disp_beijing_time(void)
{
   display_timer_and_beijing_time_handler();
}
/*
	*@brief :
	*@param:
	*@retval:
*/
static void handler_set_temperature(void)
{
   if(gpro_t.key_set_temperature==0 && gpro_t.gTimer_temp_compare_value > 2 && gpro_t.stopTwoHours_flag==0 && gpro_t.smart_phone_app_timer_power_on_flag ==0){
	 	gpro_t.gTimer_temp_compare_value =0;
		
          set_temperature_compare_value_fun();

     	}


}
/*
	*@brief :
	*@param:
	*@retval:
*/

static void handler_works_hours(void)
{

    two_hours_recoder_fun();
}
/*
	*@brief :
	*@param:
	*@retval:
*/
static void handler_disp_fan_leaf(void)
{
	disp_fan_leaf_run_icon();
  
}

static void handler_disp_wifi_icon(void)
{
	wifi_icon_blink_reg0xc5_handler();
	handler_tx_version();

}
/*
	*@brief :
	*@param:
	*@retval:
*/



/*
	*@brief :
	*@param:
	*@retval:
*/
static void handler_tx_version(void)
{
    static uint8_t send_two_disp =0,version=0;
    if(send_two_disp < 5 ){
			 send_two_disp++;

		    version = version ^ 0x01;
		     if(version ==1){
			  SendData_Set_Command(0xF0,0x02);//software version is "2"
			  //tx_thread_sleep(2);

			 }
			 else{
			 	SendData_Set_Command(0x11,0x01);
			 	//tx_thread_sleep(2);
			 }

			 
		 }

}

/*
	*@brief :
	*@param:
	*@retval:
*/




/*
	*@brief :
	*@param:
	*@retval:
*/
static void power_on_ref_init(void)
{

  
     if(gpro_t.smart_phone_app_timer_power_on_flag == 0){

    	
    	run_t.plasma=1;
    	run_t.dry =1;
    	run_t.ultrasonic =1;
    
    }
	 run_t.gAI =1; //WT.EDIT 2022.09.01
     run_t.gTimer_seconds_counter=0;

	 run_t.timer_time_hours =0;
	 run_t.timer_time_minutes =0;
	 run_t.power_off_id_flag=1;
      gpro_t.key_set_temperature =0;
	  gpro_t.power_on_every_times=1 ;
      run_t.disp_wind_speed_grade =100;//WT.EDIT 2025.04.16
      run_t.wifi_set_temperature=40; //WT.EDIT 2025.04.16
      if(run_t.wifi_link_net_success ==0){

		 run_t.gTimer_disp_time_seconds=0;
		 run_t.dispTime_hours=0;
	     run_t.dispTime_minutes=0;

      }
	  else if(run_t.wifi_link_net_success ==1 && lcd_t.display_beijing_time_flag==0){
	          run_t.gTimer_disp_time_seconds=0;
			  run_t.dispTime_hours=0;
			  run_t.dispTime_minutes=0;

      }

   
	//  display_lcd_Icon_init();
      LCD_BACK_LIGHT_ON();
	  POWER_ON_LED() ;
	  LED_MODEL_ON() ;
     


    
}



/**************************************************************************
 * 
 *Function Name:void power_key_long_fun(void)
 *Function:
 *Input Ref:
 *Return Ref:
 * 
*****************************************************************************/
void power_key_long_fun(void)
{
    
    run_t.wifi_link_net_success =0;
    run_t.gTimer_wifi_connect_counter=0;
  
    run_t.wifi_receive_led_fast_led_flag=0; //adjust if mainboard receive of connect wifi of signal
    run_t.wifi_led_fast_blink_flag=1;
  
}

/**************************************************************************
 * 
 *Function Name:void power_on_off_handler(void)
 *Function:
 *Input Ref:
 *Return Ref:
 * 
*****************************************************************************/
void power_on_off_handler(void)
{
    if(run_t.power_on== power_off){
    
    	gpro_t.power_on_step =0;
        SendData_PowerOnOff(1);
		//tx_thread_sleep(2);

	


    }
    else{ //power off .
  

	
        SendData_PowerOnOff(0);
	    //tx_thread_sleep(2);
      }
    
}


/**************************************************************************
 * 
 *Function Name:void power_on_off_handler(void)
 *Function:
 *Input Ref:
 *Return Ref:
 * 
*****************************************************************************/
static void display_lcd_Icon_init(void)
{
    

    // TIM1723_Write_Cmd(0x00);
	 TIM1723_Write_Cmd(0x40);
	 TIM1723_Write_Cmd(0x44);

 
     TM1723_Write_Display_Data(0xC2,((0X01+DRY_Symbol+KILL_Symbol+BUG_Symbol)+lcdNumber1_High[0])&0xff);//display digital "temp
	 TM1723_Write_Display_Data(0xC3,((AI_Symbol+lcdNumber1_Low[0])+lcdNumber2_High[0]) & 0xff);
	 TM1723_Write_Display_Data(0xC4,(0x01+lcdNumber2_Low[0]+lcdNumber3_High[0])&0xff);//display "t,c"
	 TM1723_Write_Display_Data(0xC5,(WIFI_Symbol+lcdNumber3_Low[0] + lcdNumber4_High[0]) & 0xff); //Wifi
     TM1723_Write_Display_Data(0xC9,(T8_HUM + lcdNumber4_Low[0] + lcdNumber5_High[0]) & 0xFF);
     TIM1723_Write_Cmd(LUM_VALUE);
 }



/**************************************************************************
 * 
 *Function Name:void power_on_off_handler(void)
 *Function:
 *Input Ref:
 *Return Ref:
 * 
*****************************************************************************/
void power_off_handler(void)
{
    
    if(run_t.power_off_id_flag == 1 || run_t.power_off_id_flag==2){   
        if(run_t.power_off_id_flag==1) run_t.power_off_id_flag =3;
		if(run_t.power_off_id_flag==2) run_t.power_off_id_flag =10;
		gpro_t.power_on_step=0;
        //cp_end 
        run_t.wifi_set_temperature =40;//WT.EDIT 2025.01.15
        run_t.smart_phone_set_temp_value_flag =0;//WT.EDIT 2025.01.15
        gpro_t.set_temp_value_success=0;//WT.EDIT 2025.01.15
        Lcd_PowerOff_Fun();
        //lcd_donot_disp_screen();
        Power_Off_Fun();
    

        LED_MODEL_OFF();
        POWER_ON_LED();

        run_t.wifi_led_fast_blink_flag=0;
        run_t.smart_phone_set_temp_value_flag=0;
        run_t.timer_time_hours =0;
        run_t.timer_time_minutes =0;

        run_t.fan_warning=0;
        run_t.ptc_warning = 0;

        run_t.gAI =0; //WT.EDIT 2022.09.01
        run_t.plasma=0;
        run_t.dry =0;
        run_t.ultrasonic =0;


        run_t.timer_set_success_flag = timing_not_definition;

        run_t.disp_wind_speed_grade =100;	

        gpro_t.smart_phone_app_timer_power_on_flag =0;
		run_t.gFan_RunContinue =1;
	     gpro_t.gTimer_temp_compare_value=0;
		if(lcd_t.display_beijing_time_flag == 0 ){

		 run_t.gTimer_disp_time_seconds=0;
		 run_t.dispTime_hours=0;
	     run_t.dispTime_minutes=0;

        }

      
	}
    
	
   
    #if 1
	power_off_breath_Led();

	#else 

	 POWER_OFF_LED();

	#endif 

	
	if(run_t.gFan_RunContinue == 1 && gpro_t.gTimer_temp_compare_value < 61){
       
	      LCD_BACK_LIGHT_ON();
	      LCD_Display_Wind_Icon_Handler();
     }
	 else if(gpro_t.gTimer_temp_compare_value  > 59){
           gpro_t.gTimer_temp_compare_value=0;
           run_t.gFan_RunContinue =0;
		   Lcd_PowerOff_Fun();
		 
		   if(lcd_t.display_beijing_time_flag == 0 ){

			 run_t.gTimer_disp_time_seconds=0;
			 run_t.dispTime_hours=0;
		     run_t.dispTime_minutes=0;
            }

	}
    
}

/************************************************************************
	*
	*Function Name: static void Power_Off_Fun(void)
	*
	*
	*
	*
************************************************************************/
 void Power_Off_Fun(void)
{
	
        run_t.gAI =0; //WT.EDIT 2022.09.01
		run_t.plasma=0;
		run_t.dry =0;
		run_t.ultrasonic =0;

		//run_t.gPower_On=power_off;
		
		run_t.wifi_led_fast_blink_flag=0;
		run_t.timer_set_success_flag = timing_not_definition;
		
		run_t.disp_wind_speed_grade =100;	
		
		
} 

/************************************************************************
	*
	*Function Name: static void power_off_breath_Led(void)
	*
	*
	*
	*
************************************************************************/
static void power_off_breath_Led(void)
 {
    static uint8_t led_flag;
	#if 0
	if(gpro_t.gTimer_disp_temp_humi_value> 1){
	    gpro_t.gTimer_disp_temp_humi_value=0;

	    led_flag = led_flag ^ 0x01;
	    if(led_flag==1){
	   
		   POWER_ON_LED();

	    }
		else{
            POWER_OFF_LED();
		}
	  
	}
   #else 
	  POWER_OFF_LED();
	 
  #endif 
	      
}
/**
*@brief: works two hours after have a rest ten minutes
*@notice :
*@param:
**/
void two_hours_recoder_fun(void)
{
  static uint8_t switch_flag=0;
  #if 0
    if(gpro_t.gTimer_two_hours_conter > 5 && gpro_t.stopTwoHours_flag==0){
  #else
    if(gpro_t.gTimer_two_hours_conter > 119 && gpro_t.stopTwoHours_flag==0){
  #endif  
      gpro_t.gTimer_two_hours_conter=0;
	  gpro_t.gTimer_two_hours_second_counter=0;
      gpro_t.stopTwoHours_flag=1;
	  gpro_t.gTimer_counter_one_minute =0;
	  gpro_t.fan_run_one_minute=1;
	 

	
	   gpro_t.fan_interval_stop_f = 0xff;
       gpro_t.two_hours_interval_f =  0xff;  
	  SendData_Set_Command(0x19,0x01); //两个小时休息 10分钟，到了。
	  //tx_thread_sleep(2);
	  

  }
  #if 0
  	else if(gpro_t.stopTwoHours_flag==1 && gpro_t.gTimer_two_hours_conter > 2){//10
  #else 
	 else if(gpro_t.stopTwoHours_flag==1 && gpro_t.gTimer_two_hours_conter > 10){

  #endif 
      gpro_t.gTimer_two_hours_conter=0;
	  gpro_t.gTimer_two_hours_second_counter=0;
      gpro_t.stopTwoHours_flag=0;
	  gpro_t.fan_run_one_minute=3;
      SendData_Set_Command(0x19,0x0); //两个小时休息10分钟，结束通知。
	  //tx_thread_sleep(2);
	
	    

     }
  
  //others separately 

  
  if(gpro_t.fan_run_one_minute==1 && gpro_t.gTimer_counter_one_minute >59){
       gpro_t.fan_run_one_minute=2;
       SendData_Set_Command(0x0B,0);//fan stop run .1. OPEN ,0- CLOSE 
	   //tx_thread_sleep(2);


  }
  if(gpro_t.stopTwoHours_flag==1 && gpro_t.gTimer_counter > 2 &&  gpro_t.fan_run_one_minute ==2){
	  gpro_t.gTimer_counter=0;

      switch_flag = switch_flag ^ 0x01;

	  if(switch_flag ==1){

           SendData_Set_Command(0x0B,0);//fan stop run . C
		   tx_thread_sleep(1);
			

	  }
	  else{
       
        SendData_Set_Command(0x19,0x01);//两个小时，休息十分钟。
	    tx_thread_sleep(1);
	  }
	  

  }

  
  if(gpro_t.stopTwoHours_flag==0 &&  gpro_t.fan_run_one_minute==3 ){

	 gpro_t.fan_run_one_minute=0;
	 SendData_Set_Command(0x19,0x0);
	 tx_thread_sleep(1);
	

  }
     
}
  


