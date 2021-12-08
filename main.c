#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "generic.h"
#include "device.h"
#include "hw_config.h"
#include "usb_init.h"
#include "netconf.h"
#include "httpd_cgi_ssi.h"
#include "rndis.h"

#include "dhserver.h"
#define NUM_DHCP_ENTRY 3

static dhcp_entry_t entries[NUM_DHCP_ENTRY] =
{
    /* mac    ip address        subnet mask        lease time */
    { {0}, {192, 168, 7, 2}, {255, 255, 255, 0}, 24 * 60 * 60 },
    { {0}, {192, 168, 7, 3}, {255, 255, 255, 0}, 24 * 60 * 60 },
    { {0}, {192, 168, 7, 4}, {255, 255, 255, 0}, 24 * 60 * 60 }
};

static dhcp_config_t dhcp_config =
{
    {192, 168, 7, 1}, 67, /* server address, port */
    {192, 168, 7, 1},     /* dns server */
    "stm",                /* dns suffix */
    NUM_DHCP_ENTRY,       /* num entry */
    entries               /* entries */
};

#include "dnserver.h"
static uint8_t ipaddr[4]  = {192, 168, 7, 1};

uint32_t dns_query_proc(const char *name, ip_addr_t *addr)
{
    if (strcmp(name, "run.stm") == 0 || strcmp(name, "www.run.stm") == 0)
    {
        addr->addr = *(uint32_t *)ipaddr;
        return 1;
    }
    return 0;
}

int main()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = PIN_LED;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
/****/
	Set_System();
	Set_USBClock();
	DWT_Delay_Init();
	USB_Interrupts_Config(ENABLE);
	USB_Init();
/****/
	LwIP_Init();
	LwIP_httpd_Init();

    while (dhserv_init(&dhcp_config) != ERR_OK) ;

    while (dnserv_init((ip_addr_t *)ipaddr, 53, dns_query_proc) != ERR_OK) ;

	uint32_t LocalTime = 0;
	while (1)
	{
		if (rndis_data_pending())
			LwIP_Pkt_Handle();
//		LwIP_Periodic_Handle(LocalTime);
//		LocalTime += 10;
//
//		if(LocalTime % 500 == 0)
//		{
//			GPIO_ToggleBits(GPIOC, PIN_LED);
//		}
//		DWT_Delay_ms(10);
	}
}
