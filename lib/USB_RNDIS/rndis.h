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

#define RNDIS_VENDOR                "BluePill"                     /* NIC vendor name */
#define RNDIS_LINK_SPEED            12000000                       /* Link baudrate (12Mbit/s for USB-FS) */
#define RNDIS_MTU                   1500                           /* MTU value */
#define ETH_HEADER_SIZE             14
#define ETH_MAX_PACKET_SIZE         RNDIS_MTU + ETH_HEADER_SIZE
#define RNDIS_RX_BUFFER_SIZE        (ETH_MAX_PACKET_SIZE + sizeof(rndis_data_packet_t))

#define OID_LIST_LENGTH             21
extern const uint32_t OIDSupportedList[OID_LIST_LENGTH];

#define ENC_BUF_SIZE                (OID_LIST_LENGTH * 4 + 32)
extern uint8_t encapsulated_buffer[];
extern uint8_t usb_rx_buffer[];

extern uint8_t *rndis_rx_ptr;
extern int rndis_rx_size;
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
uint8_t rndis_can_send(void);
uint8_t rndis_send(const void *data, int size);

#endif /* USB_RNDIS_RNDIS_H_ */
