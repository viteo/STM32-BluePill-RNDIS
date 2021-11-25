#ifndef __RNDISIF_H__
#define __RNDISIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

err_t rndisif_init(struct netif *netif);
err_t rndisif_input(struct netif *netif);

#endif
