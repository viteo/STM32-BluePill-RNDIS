/*
 * rndisif.c
 *
 *  Created on: 6 Dec 2021
 *      Author: v.simonenko
 *
 *      RNDIS to LwIP Network Interface
 */

#include "rndisif.h"
#include "rndis.h"
#include "lwip/etharp.h"
#include "generic.h"

/* Network interface name */
#define IFNAME0 'S'
#define IFNAME1 'T'

err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    static char data[ETH_MAX_PACKET_SIZE];
    int size = 0;
    for (int i = 0; i < 200; i++)
    {
        if (rndis_can_send()) break;
        DWT_Delay_ms(1);
    }
    for(q = p; q != NULL; q = q->next)
    {
        if (size + q->len > ETH_MAX_PACKET_SIZE)
            return ERR_ARG;
        memcpy(data + size, (char *)q->payload, q->len);
        size += q->len;
    }
    if (!rndis_can_send())
        return ERR_USE;
    rndis_send(data, size);
    return ERR_OK;
}

/*
 * Should be called at the beginning of the program to set up the
 * network interface.
 *
 * This function should be passed as a parameter to netif_add().
 */
err_t rndisif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    netif->mtu = RNDIS_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
    netif->state = NULL;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    netif->output = etharp_output;
    netif->linkoutput = linkoutput_fn;
    return ERR_OK;
}

err_t rndisif_input(struct netif *netif)
{
    struct pbuf *frame;
    __disable_irq();
    frame = pbuf_alloc(PBUF_RAW, rndis_rx_size, PBUF_POOL);
    if (frame == NULL)
    {
        __enable_irq();
        return ERR_MEM;
    }
    memcpy(frame->payload, rndis_rx_ptr, rndis_rx_size);
    frame->len = rndis_rx_size;
    rndis_rx_size = 0;
    __enable_irq();
    err_t err = netif->input(frame, netif);
    if(err != ERR_OK)
    {
    	pbuf_free(frame);
    }
    return err;
}
