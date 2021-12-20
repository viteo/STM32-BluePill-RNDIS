/*
 * netconf.c
 *
 *  Created on: 6 Dec 2021
 *      Author: v.simonenko
 */

#include "netconf.h"
#include "device.h"
#include "rndisif.h"
#include "lwip/init.h"
#include "netif/ethernet.h"

struct netif rndis_netif; //network interface
const ip_addr_t ipaddr  = IPADDR4_INIT_BYTES(IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
const ip_addr_t netmask = IPADDR4_INIT_BYTES(NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
const ip_addr_t gateway = IPADDR4_INIT_BYTES(GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

#include "dhserver.h"
static dhcp_entry_t entries[] =
{
    {
    		{0},
			IPADDR4_INIT_BYTES(IP_ADDR0, IP_ADDR1, IP_ADDR2, DHCP_ADDR_1),
			IPADDR4_INIT_BYTES(NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3),
			24 * 60 * 60
    },
    {
    		{0},
			IPADDR4_INIT_BYTES(IP_ADDR0, IP_ADDR1, IP_ADDR2, DHCP_ADDR_2),
			IPADDR4_INIT_BYTES(NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3),
			24 * 60 * 60
    },
};

static dhcp_config_t dhcp_config =
{
	&ipaddr,              /* server address */
	PORT_DHCP,            /* port */
    &ipaddr,              /* dns server */
    "stm",                /* dns suffix */
    sizeof(entries) / sizeof(entries[0]),       /* entry count */
    entries               /* entries */
};

#include "dnserver.h"

uint32_t dns_query_proc(const char *name, ip_addr_t *addr)
{
    if (strcmp(name, "run.stm") == 0 || strcmp(name, "www.run.stm") == 0)
    {
        addr->addr = ipaddr.addr;
        return 1;
    }
    return 0;
}

/*
 * @brief  Initializes the lwIP stack
 * @param  None
 * @retval None
 */
void LwIP_Init(void)
{
    struct netif  *netif = &rndis_netif;

    lwip_init();

    netif = netif_add(netif, &ipaddr, &netmask, &gateway, NULL, rndisif_init, ethernet_input);
    netif_set_default(netif);

    while (!netif_is_up(netif)) ;

    while (dhserv_init(&dhcp_config)) ;

    while (dnserv_init(&ipaddr, PORT_DNS, dns_query_proc)) ;
}

/**
* @brief  Called when a frame is received
* @param  None
* @retval None
*/
void LwIP_Pkt_Handle(void)
{
  /* Read a received packet from the buffers and send it to the lwIP for handling */
  rndisif_input(&rndis_netif);
}

/*
* @brief  LwIP periodic tasks
* @param  localtime the current LocalTime value
* @retval None
*/
//uint32_t TCPTimer = 0;
//uint32_t ARPTimer = 0;
//void LwIP_Periodic_Handle(volatile uint32_t localtime)
//{
//#if LWIP_TCP
//  /* TCP periodic process every 250 ms */
//  if (localtime - TCPTimer >= 250)
//  {
//    TCPTimer =  localtime;
//    tcp_tmr();
//  }
//#endif
//
//  /* ARP periodic process every 1s */
//  if ((localtime - ARPTimer) >= ARP_TMR_INTERVAL)
//  {
//    ARPTimer =  localtime;
//    etharp_tmr();
//  }
//}
