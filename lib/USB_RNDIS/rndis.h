/*
 * rndis.h
 *
 *  Created on: 13 oct 2021
 *      Author: v.simonenko
 */

#ifndef USB_RNDIS_RNDIS_H_
#define USB_RNDIS_RNDIS_H_

#include "ndis.h"
#include "rndis_protocol.h"

#define RNDIS_MTU        1500                           /* MTU value */
#define RNDIS_LINK_SPEED 12000000                       /* Link baudrate (12Mbit/s for USB-FS) */
#define RNDIS_VENDOR     "BluePill"                      /* NIC vendor name */
#define RNDIS_HWADDR     0x20,0x89,0x84,0x6A,0x96,0xAB  /* MAC-address to set to host interface */

#define ETH_HEADER_SIZE             14
#define ETH_MAX_PACKET_SIZE         ETH_HEADER_SIZE + RNDIS_MTU
#define ETH_MIN_PACKET_SIZE         60
#define RNDIS_RX_BUFFER_SIZE        (ETH_MAX_PACKET_SIZE + sizeof(rndis_data_packet_t))

extern const uint32_t OIDSupportedList[22];
#define OID_LIST_LENGTH (sizeof(OIDSupportedList) / sizeof(*OIDSupportedList))
#define ENC_BUF_SIZE    (OID_LIST_LENGTH * 4 + 32)

extern uint8_t encapsulated_buffer[ENC_BUF_SIZE];
extern char rndis_rx_buffer[RNDIS_RX_BUFFER_SIZE];
extern uint8_t usb_rx_buffer[];

extern rndis_state_t rndis_state;
extern usb_eth_stat_t usb_eth_stat;

extern uint8_t *rndis_tx_ptr;
extern uint8_t rndis_first_tx;
extern int rndis_tx_size;
extern int rndis_sended;
extern int sended;

void rndis_query_cmplt(int status, const void *data, int size);
void rndis_query_cmplt32(int status, uint32_t data);
void rndis_query();
void rndis_handle_set_msg();
void usbd_cdc_transfer();

#endif /* USB_RNDIS_RNDIS_H_ */
