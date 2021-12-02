#include "rndisif.h"
#include "device.h"
#include "generic.h"
#include "rndis.h"
#include "netif/etharp.h"
#include <string.h>

/* Network interface name */
#define IFNAME0 's'
#define IFNAME1 't'


err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
    int i;
    struct pbuf *q;
    static char data[RNDIS_MTU + 14 + 4];
    int size = 0;
    for (i = 0; i < 200; i++)
    {
        if (rndis_can_send()) break;
        DWT_Delay_ms(1); //todo check lower timeout
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

/**
 * Should be called at the beginning of the program to set up the
 * network interface.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t rndisif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
  
#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  netif->mtu = RNDIS_MTU;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;

  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = linkoutput_fn;

  return ERR_OK;
}

err_t rndisif_input(struct netif *netif)
{
    struct pbuf *frame;
    __disable_irq();
    if (recvSize == 0) //todo move check outside or merge call with usb on_packet()
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
    ethernet_input(frame, netif);
    pbuf_free(frame);
}
