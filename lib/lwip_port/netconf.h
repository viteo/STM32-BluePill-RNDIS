/*
 * netconf.h
 *
 *  Created on: 6 Dec 2021
 *      Author: v.simonenko
 */

#ifndef LWIP_PORT_NETCONF_H_
#define LWIP_PORT_NETCONF_H_

void LwIP_Init(void);
void LwIP_Pkt_Handle(void);
//void LwIP_Periodic_Handle(volatile uint32_t localtime);

#endif /* LWIP_PORT_NETCONF_H_ */
