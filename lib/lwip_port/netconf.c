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

struct netif rndis_netif;

/*
 * @brief  Initializes the lwIP stack
 * @param  None
 * @retval None
 */
void LwIP_Init(void)
{
    struct netif  *netif = &rndis_netif;
    uint8_t ipaddr[4]  = {IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3};
    uint8_t netmask[4] = {NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3};
    uint8_t gateway[4] = {GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3};
    uint8_t hwaddr[6]  = {MAC_ADDR0,MAC_ADDR1,MAC_ADDR2,MAC_ADDR3,MAC_ADDR4,MAC_ADDR5};

    lwip_init();
    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, hwaddr, 6);

    netif = netif_add(netif, (ip_addr_t *)ipaddr, (ip_addr_t *)netmask, (ip_addr_t *)gateway, NULL, rndisif_init, ethernet_input);
    netif_set_default(netif);

	/* Wait for it */
    while (!netif_is_up(netif)) ;
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
