#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "generic.h"
#include "device.h"
#include "hw_config.h"
#include "usb_init.h"
//#include "netconf.h"

#include "rndis.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip.h"
#include "lwip/err.h"
#include "lwip/etharp.h"


err_t output_fn(struct netif *netif, struct pbuf *p, ip4_addr_t *ipaddr)
{
    return etharp_output(netif, p, ipaddr);
}

err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
    int i;
    struct pbuf *q;
    static char data[RNDIS_MTU + 14 + 4];
    int size = 0;
    for (i = 0; i < 200; i++)
    {
        if (rndis_can_send()) break;
        DWT_Delay_ms(1);
    }
    for(q = p; q != NULL; q = q->next)
    {
        if (size + q->len > RNDIS_MTU + 14)
            return ERR_ARG;
        memcpy(data + size, (char *)q->payload, q->len);
        size += q->len;
    }
    if (!rndis_can_send())
        return ERR_USE;
    rndis_send(data, size);
    return ERR_OK;
}

err_t netif_init_cb(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    netif->mtu = RNDIS_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
    netif->state = NULL;
    netif->name[0] = 'E';
    netif->name[1] = 'X';
    netif->linkoutput = linkoutput_fn;
    netif->output = output_fn;
    return ERR_OK;
}

struct netif netif_data;
static uint8_t hwaddr[6]  = {0x20,0x89,0x84,0x6A,0x96,00};
static uint8_t ipaddr[4]  = {192, 168, 7, 1};
static uint8_t netmask[4] = {255, 255, 255, 0};
static uint8_t gateway[4] = {0, 0, 0, 0};
#define PADDR(ptr) ((ip_addr_t *)ptr)

void init_lwip()
{
    struct netif  *netif = &netif_data;

    lwip_init();
    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, hwaddr, 6);

    netif = netif_add(netif, PADDR(ipaddr), PADDR(netmask), PADDR(gateway), NULL, netif_init_cb, ip_input);
    netif_set_default(netif);
}

void usb_polling()
{
    struct pbuf *frame;
    __disable_irq();
    if (recvSize == 0)
    {
        __enable_irq();
        return;
    }
    frame = pbuf_alloc(PBUF_RAW, recvSize, PBUF_POOL);
    if (frame == NULL)
    {
        __enable_irq();
        return;
    }
    memcpy(frame->payload, received, recvSize);
    frame->len = recvSize;
    recvSize = 0;
    __enable_irq();
    ethernet_input(frame, &netif_data);
    pbuf_free(frame);
}

static 	uint32_t LocalTime = 0;
uint32_t sys_now()
{
    return LocalTime;
}


uint32_t TCPTimer = 0;
uint32_t ARPTimer = 0;

void LwIP_Periodic_Handle(volatile uint32_t localtime)
{
#if LWIP_TCP
  /* TCP periodic process every 250 ms */
  if (localtime - TCPTimer >= 250)
  {
    TCPTimer =  localtime;
    tcp_tmr();
  }
#endif

  /* ARP periodic process every 1s */
  if ((localtime - ARPTimer) >= ARP_TMR_INTERVAL)
  {
    ARPTimer =  localtime;
    etharp_tmr();
  }

#ifdef USE_DHCP
  /* Fine DHCP periodic process every 500ms */
  if (localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
  {
    DHCPfineTimer =  localtime;
    dhcp_fine_tmr();
    if ((DHCP_state != DHCP_ADDRESS_ASSIGNED) &&
        (DHCP_state != DHCP_TIMEOUT) &&
          (DHCP_state != DHCP_LINK_DOWN))
    {
      /* toggle LED1 to indicate DHCP on-going process */
      STM_EVAL_LEDToggle(LED1);

      /* process DHCP state machine */
      LwIP_DHCP_Process_Handle();
    }
  }

  /* DHCP Coarse periodic process every 60s */
  if (localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
  {
    DHCPcoarseTimer =  localtime;
    dhcp_coarse_tmr();
  }

#endif
}

#include "lwip/apps/httpd.h"
const char *state_cgi_handler(int index, int n_params, char *params[], char *values[])
{
    return "/state.shtml";
}

const char *ctl_cgi_handler(int index, int n_params, char *params[], char *values[])
{
    return "/state.shtml";
}

static const tCGI cgi_uri_table[] =
{
    { "/state.cgi", state_cgi_handler },
    { "/ctl.cgi",   ctl_cgi_handler },
};


static u16_t ssi_handler(int index, char *insert, int ins_len)
{
    int res;

    if (ins_len < 32) return 0;

    switch (index)
    {
    case 0: /* systick */
        res = snprintf(insert, ins_len, "%u", 5);
        break;
    }

    return res;
}

static const char *ssi_tags_table[] =
{
    "systick", /* 0 */
    "btn",     /* 1 */
    "acc",     /* 2 */
    "ledg",    /* 3 */
    "ledo",    /* 4 */
    "ledr"     /* 5 */
};

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
	init_lwip();

    while (!netif_is_up(&netif_data)) ;

//    while (dhserv_init(&dhcp_config) != ERR_OK) ;

//    while (dnserv_init(PADDR(ipaddr), 53, dns_query_proc) != ERR_OK) ;

    http_set_cgi_handlers(cgi_uri_table, sizeof(cgi_uri_table) / sizeof(tCGI));
    http_set_ssi_handler(ssi_handler, ssi_tags_table, sizeof(ssi_tags_table) / sizeof(char *));
    httpd_init();


	uint32_t LocalTime = 0;
	while (1)
	{
		usb_polling();
		LwIP_Periodic_Handle(LocalTime);
		LocalTime += 10;

		if(LocalTime % 500 == 0)
			GPIO_ToggleBits(GPIOC, PIN_LED);
		DWT_Delay_ms(10);
	}
}
