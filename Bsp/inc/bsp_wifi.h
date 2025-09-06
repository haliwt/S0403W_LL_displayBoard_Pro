#ifndef __BSP_WIFI_H
#define __BSP_WIFI_H
#include "main.h"


extern uint8_t (*wifi_link_net_state)(void);

void bsp_wifi_init(void);

void wifi_link_net_state_hanlder(uint8_t (*wifi_link_net_handler)(void));


#endif 

