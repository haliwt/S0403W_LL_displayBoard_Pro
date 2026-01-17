#ifndef __BSP_FAN_H
#define __BSP_FAN_H
#include "main.h"


typedef enum _fan_speed
{

  fan_speed_full,
  fan_speed_middle,
  fan_speed_low



}fan_speed;



void disp_fan_leaf_init(void);




void display_dry_temp_fun(void);





#endif 

