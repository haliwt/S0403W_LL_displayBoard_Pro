#include "bsp.h"

/******************************************************************************
*
*Function Name:void receive_data_fromm_mainboard(uint8_t *pdata,uint8_t len)
*Funcion: handle of tall process 
*Input Ref:
*Return Ref:
*
******************************************************************************/
void receive_data_fromm_mainboard(uint8_t *pdata)
{
     uint8_t hum1,hum2,temp1,temp2,decade_temp,unit_temp; 

    switch(pdata[2]){

     case 0:


     break;

     case 0x31: //smart phone app timer that power on command,is normal power on and off

        if(pdata[3] == 0x01){ //open
           run_t.wifi_link_net_success=1;
            SendWifiData_Answer_Cmd(0x31,0x01);
            xTask_PowerOn_Handler() ; 

        }
        else if(pdata[3] == 0x0){ //close 
            run_t.wifi_link_net_success=1;
            SendWifiData_Answer_Cmd(0x031,0x0);
            xTask_PowerOff_Handler() ; 


        }

     break;

    case 0x21: //smart phone power on or off that App timer .
        if(pdata[3]==0x01){ //power on by smart phone APP
            gpro_t.smart_phone_app_timer_power_on_flag =1;
            run_t.wifi_link_net_success=1;
             
             xTask_PowerOn_Handler() ; 
        
         }
         else{  //power off by smart phone APP
             run_t.wifi_link_net_success=1;
             xTask_PowerOff_Handler() ;     
         }
       
     break; 

     case 0x02: //PTC打开关闭指令

     if(pdata[3] == 0x01){

            run_t.dry =1 ;//&& run_t.plasma ==1  && run_t.ultrasonic==1
            run_t.wifi_link_net_success=1;
           gpro_t.smart_phone_turn_off_ptc_flag = 0;
        }
        else if(pdata[3] == 0x0){

            run_t.wifi_link_net_success=1;
            run_t.dry =0;
            gpro_t.smart_phone_turn_off_ptc_flag = 1;

        }

     break;

     case 0x03: //PLASMA 打开关闭指令

        if(pdata[3] == 0x01){
            run_t.wifi_link_net_success=1;
        run_t.plasma =1;


        }
        else if(pdata[3] == 0x0){
           run_t.wifi_link_net_success=1;
         run_t.plasma =0;

        }


     break;


      case 0x04: //ultrasonic  打开关闭指令

        if(pdata[3] == 0x01){  //open 
            run_t.wifi_link_net_success=1;
            run_t.ultrasonic=1;

        }
        else if(pdata[3] == 0x0){ //close 
          run_t.wifi_link_net_success=1;
          run_t.ultrasonic=0;

        }


     break;


     

      case 0x08: //temperature of high warning.

        if(pdata[3] == 0x01){  //warning 

            run_t.ptc_warning = 1;
            //run_t.setup_timer_timing_item =  PTC_WARNING; //ptc warning 
            run_t.display_set_timer_or_works_time_mode = PTC_WARNING;
        
            run_t.dry =0;
            SendData_Set_Command(0x22,0x0); //close ptc ,but don't buzzer sound .

        }
        else if(pdata[3] == 0x0){ //close 

           run_t.ptc_warning = 0;
         

        }
            


      break;

      case 0x09: //fan of default of warning.

         if(pdata[3] == 0x01){  //warning 

            run_t.fan_warning = 1;
          
           run_t.display_set_timer_or_works_time_mode =FAN_WARNING;  //run_t.display_set_timer_or_works_time_mode
       
           run_t.dry =0;
            SendData_Set_Command(0x22,0x0); //close ptc ,but don't buzzer sound .

        }
        else if(pdata[3] == 0x0){ //close 

           run_t.fan_warning = 0;
         

        }


      break;


     //接收的是数据

      case 0x1A: //温度数据

        if(pdata[4] == 0x02){ //数据
        
           
             
             gpro_t.humidity_real_value = pdata[5];
             gpro_t.temp_real_value = pdata[6];

             //humidity_value 

            hum1 =  gpro_t.humidity_real_value/ 10;
            hum2 =  gpro_t.humidity_real_value % 10;

           lcd_t.number3_low= hum1;
		   lcd_t.number3_high =hum1;

		   lcd_t.number4_low = hum2;
	       lcd_t.number4_high = hum2;

            //temperature_value 
        //  temp = pdata[6];
          temp1 =   gpro_t.temp_real_value/ 10;
          temp2   = gpro_t.temp_real_value% 10;

           lcd_t.number1_low= temp1;
		   lcd_t.number1_high =temp1;

		   lcd_t.number2_low = temp2;
	       lcd_t.number2_high = temp2;

		   power_on_display_temp_handler(); //WT.EDIT 2025.03.28

        }
        else if(pdata[4] == 0x01){ //数据)

             
            
        }
      break;

      case 0x1B: //湿度数据

        if(pdata[2] == 0x0F){ //数据
           

        }
      break;

      case 0x1C: //表示时间：小时，分，秒

        if(pdata[4] == 0x03){ //数据

            if(pdata[5] < 24){ //WT.EDIT 2024.11.23

            lcd_t.display_beijing_time_flag= 1;

            run_t.dispTime_hours  =  pdata[5];
            run_t.dispTime_minutes = pdata[6];
            run_t.gTimer_disp_time_seconds =  pdata[7];
           }


        }
      break;

        case 0x1D: //表示日期： 年，月，日

        if(pdata[2] == 0x0F){ //数据

             
            
        }
      break;


      case 0x1E: //fan of speed is value 

          if(pdata[5] < 34){
            run_t.wifi_link_net_success=1;
            run_t.disp_wind_speed_grade = 10;


          }
          else if(pdata[5] < 67 && pdata[5] > 33){
            run_t.wifi_link_net_success=1;
            run_t.disp_wind_speed_grade = 60;

          }
          else if(pdata[5] > 66){
            run_t.wifi_link_net_success=1;
             run_t.disp_wind_speed_grade =100;
          }



      break;

      case 0x1F: //link wifi if success data 
        
      if(pdata[5] == 0x01){  // link wifi 
      
           run_t.wifi_link_net_success =1 ;      
      
        }
        else if(pdata[5] == 0x0){ //don't link wifi 
      
           run_t.wifi_link_net_success =0 ;     
      
         }
        


      break;

    case 0x27 : //AI mode by smart phone of APP be control.

        if(pdata[3]==2){
         //timer time + don't has ai item
              run_t.display_set_timer_or_works_time_mode = timer_time;
              run_t.gTimer_again_switch_works = 0; //WT.EDIT ,if don't define timer_time,wait 3s switch to works_time.
    	      run_t.gModel=0;
         }
         else{
                  //beijing time + ai item
              run_t.display_set_timer_or_works_time_mode = works_time;
             
	          run_t.gModel=1;

         }


     break;

     case 0x3A: // smart phone APP set temperature value 
        
        run_t.wifi_link_net_success=1;
        gpro_t.smart_phone_turn_off_ptc_flag=0; //smart phone app from setup temperature value .
        run_t.wifi_set_temperature = pdata[5];

        decade_temp =  run_t.wifi_set_temperature / 10 ;
		unit_temp =  run_t.wifi_set_temperature % 10; //
        
		lcd_t.number1_low=decade_temp;
		lcd_t.number1_high =decade_temp;

		lcd_t.number2_low = unit_temp;
		lcd_t.number2_high = unit_temp;

        run_t.smart_phone_set_temp_value_flag =1;

        
      break;

     case 0xFF: // copy send cmd acknowlege
          //power on or power off 
          if(pdata[3]==0x01){ //power on or power off cmd.
              if(pdata[4]==1){ //power on

                 gpro_t.receive_copy_cmd = ack_power_on;
                 do{
                     
                     power_on_key_short_fun();
                     run_t.power_on= power_on;
                 }while(0);
                
                 

              }
              else if(pdata[4]==2){ //power offf
                 gpro_t.receive_copy_cmd = ack_power_off;
                 run_t.power_on= power_off;

              }

          }
          else if(pdata[3] == 0x05){ //link wifi command .

              if(pdata[4]==1){

                 gpro_t.receive_copy_cmd = ack_wifi_on;
                  power_key_long_fun();

              }


          }
          else if(pdata[3] == 0x16){  //buzzer answer command 

                gpro_t.receive_copy_cmd = ack_buzzer_sound;

           }

         
     break;
     
     }

 }



// BCC校验函数
uint8_t bcc_check(const unsigned char *data, int len) {
    unsigned char bcc = 0;
    for (int i = 0; i < len; i++) {
        bcc ^= data[i];
    }
    return bcc;
}

/******************************************************************************
*
*Function Name:void send_cmd_ack_hanlder(void)
*Funcion: handle of tall process 
*Input Ref:
*Return Ref:
*
******************************************************************************/
void send_cmd_ack_hanlder(void)
{
   

    switch(gpro_t.send_ack_cmd){

    case ack_null:


    break;

    case ack_power_on:
        
      if(gpro_t.receive_copy_cmd == ok){
         gpro_t.receive_copy_cmd =0;
         gpro_t.send_ack_cmd = 0;
        
      }
      else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
         gpro_t.gTimer_again_send_power_on_off =0;
         SendCmd_fun(0x10,0x01); //send notice don't buzzer sound .
      }
                
     
    break;

    case ack_power_off :

     if(gpro_t.receive_copy_cmd == ok){
        gpro_t.receive_copy_cmd =0;
         gpro_t.send_ack_cmd = 0;
         
      }
      else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
          gpro_t.gTimer_again_send_power_on_off =0;
          SendCmd_fun(0x10,0x0);//power off notice ,don't buzzer sound .SendData_PowerOnOff(0);
      }


    break;

    case ack_wifi_on:

         if(gpro_t.receive_copy_cmd == ack_wifi_on){
            gpro_t.receive_copy_cmd =0;
             gpro_t.send_ack_cmd = 0;
             
          }
          else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
              gpro_t.gTimer_again_send_power_on_off =0;
              SendData_Set_Command(0x05,0x01); // link wifi of command .
          }



    break;

    case ack_ptc_on:


    break;

    case ack_ptc_off:

    break;

    case ack_ai_mode:
//        if(gpro_t.receive_copy_cmd == ack_ai_mode){
//            gpro_t.receive_copy_cmd =0;
//            gpro_t.send_ack_cmd = 0;
//
//        }
//        else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
//            gpro_t.gTimer_again_send_power_on_off =0;
//            SendData_Set_Command(0x27,0x01); //MODE_AI,BUR NO_BUZZER);
//
//        }
    break;

    case ack_not_ai_mode:

//       if(gpro_t.receive_copy_cmd == ack_not_ai_mode){
//            gpro_t.receive_copy_cmd =0;
//            gpro_t.send_ack_cmd = 0;
//
//        }
//        else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
//            gpro_t.gTimer_again_send_power_on_off =0;
//            SendData_Set_Command(0x27,0x02); //NOT_MODE_AI
//
//        }


    break;

    case ack_buzzer_sound:

//        if(gpro_t.receive_copy_cmd == ack_buzzer_sound){
//            gpro_t.receive_copy_cmd =0;
//            gpro_t.send_ack_cmd = 0;
//
//        }
//        else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
//            gpro_t.gTimer_again_send_power_on_off =0;
//            SendData_Set_Command(0x06,0x01); //buzzer sound command .
//
//        }

    break;


    

    


    }



}

