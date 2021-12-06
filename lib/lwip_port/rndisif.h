/*
 * rndisif.h
 *
 *  Created on: 6 Dec 2021
 *      Author: v.simonenko
 */

#ifndef LWIP_PORT_RNDISIF_H_
#define LWIP_PORT_RNDISIF_H_

#include "lwip/err.h"
#include "lwip/netif.h"

err_t rndisif_init(struct netif *netif);
err_t rndisif_input(struct netif *netif);

#endif /* LWIP_PORT_RNDISIF_H_ */
