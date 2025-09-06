#include "bsp.h"

uint8_t (*wifi_link_net_state)(void);


static uint8_t wifi_link_net_state_default(void);

void bsp_wifi_init(void)
{
    wifi_link_net_state_hanlder(wifi_link_net_state_default);



}

void wifi_link_net_state_hanlder(uint8_t (*wifi_link_net_handler)(void))
{


     wifi_link_net_state = wifi_link_net_handler;

}

static uint8_t wifi_link_net_state_default(void)
{
    if(run_t.wifi_link_net_success == 1) return 1;
    else return 0;

}

