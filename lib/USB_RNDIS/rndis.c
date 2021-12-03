/*
 * rndis.c
 *
 *  Created on: 13 oct 2021
 *      Author: v.simonenko
 */

#include "rndis.h"
#include "usb_conf.h"
#include "usb_sil.h"
#include "usb_desc.h"
#include "usb_lib.h"
#include <string.h>

static const uint8_t station_hwaddr[6] = { RNDIS_HWADDR };
static const uint8_t permanent_hwaddr[6] = { RNDIS_HWADDR };
usb_eth_stat_t usb_eth_stat = { 0, 0, 0, 0 };
uint32_t oid_packet_filter = 0x0000000;

const uint32_t OIDSupportedList[22] =
{
    OID_GEN_SUPPORTED_LIST,
    OID_GEN_HARDWARE_STATUS,
    OID_GEN_MEDIA_SUPPORTED,
    OID_GEN_MEDIA_IN_USE,
    OID_GEN_MAXIMUM_FRAME_SIZE,
    OID_GEN_LINK_SPEED,
    OID_GEN_TRANSMIT_BLOCK_SIZE,
    OID_GEN_RECEIVE_BLOCK_SIZE,
    OID_GEN_VENDOR_ID,
    OID_GEN_VENDOR_DESCRIPTION,
    OID_GEN_VENDOR_DRIVER_VERSION,
    OID_GEN_CURRENT_PACKET_FILTER,
    OID_GEN_MAXIMUM_TOTAL_SIZE,
    OID_GEN_PROTOCOL_OPTIONS,
    OID_GEN_MAC_OPTIONS,
    OID_GEN_MEDIA_CONNECT_STATUS,
    OID_GEN_MAXIMUM_SEND_PACKETS,
    OID_802_3_PERMANENT_ADDRESS,
    OID_802_3_CURRENT_ADDRESS,
    OID_802_3_MULTICAST_LIST,
    OID_802_3_MAXIMUM_LIST_SIZE,
    OID_802_3_MAC_OPTIONS
};

rndis_state_t rndis_state;
static const char *rndis_vendor = RNDIS_VENDOR;

uint8_t encapsulated_buffer[ENC_BUF_SIZE];
char rndis_rx_buffer[RNDIS_RX_BUFFER_SIZE];
uint8_t usb_rx_buffer[CDC_DATA_SIZE];

//todo clean
uint8_t received[RNDIS_MTU + 14];
int recvSize = 0;
void on_packet(const char *data, int size)
{
    memcpy(received, data, size);
    recvSize = size;
}
//

rndis_rxproc_t rndis_rxproc = on_packet;
uint8_t *rndis_tx_ptr = NULL;
uint8_t rndis_first_tx = 1;
int rndis_tx_size = 0;
int rndis_sended = 0;
int sended = 0;

void rndis_query_cmplt(int status, const void *data, int size)
{
	rndis_query_cmplt_t *c;
	c = (rndis_query_cmplt_t *)encapsulated_buffer;
	c->MessageType = REMOTE_NDIS_QUERY_CMPLT;
	c->MessageLength = sizeof(rndis_query_cmplt_t) + size;
	c->InformationBufferLength = size;
	c->InformationBufferOffset = 16;
	c->Status = status;
	memcpy(c + 1, data, size);
	USB_SIL_Write(CDC_CMD_EP, (uint8_t *)"\x01\x00\x00\x00\x00\x00\x00\x00", 8);
	SetEPTxCount(CDC_CMD_EP_IDX, 8);
	SetEPTxValid(CDC_CMD_EP_IDX);
}

void rndis_query_cmplt32(int status, uint32_t data)
{
	rndis_query_cmplt_t *c;
	c = (rndis_query_cmplt_t *)encapsulated_buffer;
	c->MessageType = REMOTE_NDIS_QUERY_CMPLT;
	c->MessageLength = sizeof(rndis_query_cmplt_t) + 4;
	c->InformationBufferLength = 4;
	c->InformationBufferOffset = 16;
	c->Status = status;
	*(uint32_t *)(c + 1) = data;
	USB_SIL_Write(CDC_CMD_EP, (uint8_t *)"\x01\x00\x00\x00\x00\x00\x00\x00", 8);
	SetEPTxCount(CDC_CMD_EP_IDX, 8);
	SetEPTxValid(CDC_CMD_EP_IDX);
}


void rndis_query()
{
	switch (((rndis_query_msg_t *)encapsulated_buffer)->Oid)
	{
		case OID_GEN_SUPPORTED_LIST:         rndis_query_cmplt(RNDIS_STATUS_SUCCESS, OIDSupportedList, 4 * OID_LIST_LENGTH); return;
		case OID_GEN_VENDOR_DRIVER_VERSION:  rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0x00001000);  return;
		case OID_802_3_CURRENT_ADDRESS:      rndis_query_cmplt(RNDIS_STATUS_SUCCESS, &station_hwaddr, 6); return;
		case OID_802_3_PERMANENT_ADDRESS:    rndis_query_cmplt(RNDIS_STATUS_SUCCESS, &permanent_hwaddr, 6); return;
		case OID_GEN_MEDIA_SUPPORTED:        rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, NDIS_MEDIUM_802_3); return;
		case OID_GEN_MEDIA_IN_USE:           rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, NDIS_MEDIUM_802_3); return;
		case OID_GEN_PHYSICAL_MEDIUM:        rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, NDIS_MEDIUM_802_3); return;
		case OID_GEN_HARDWARE_STATUS:        rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
		case OID_GEN_LINK_SPEED:             rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, RNDIS_LINK_SPEED / 100); return;
		case OID_GEN_VENDOR_ID:              rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0x00FFFFFF); return;
		case OID_GEN_VENDOR_DESCRIPTION:     rndis_query_cmplt(RNDIS_STATUS_SUCCESS, rndis_vendor, strlen(rndis_vendor) + 1); return;
		case OID_GEN_CURRENT_PACKET_FILTER:  rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, oid_packet_filter); return;
		case OID_GEN_MAXIMUM_FRAME_SIZE:     rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE - ETH_HEADER_SIZE); return;
		case OID_GEN_MAXIMUM_TOTAL_SIZE:     rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE); return;
		case OID_GEN_TRANSMIT_BLOCK_SIZE:    rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE); return;
		case OID_GEN_RECEIVE_BLOCK_SIZE:     rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE); return;
		case OID_GEN_MEDIA_CONNECT_STATUS:   rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, NDIS_MEDIA_STATE_CONNECTED); return;
		case OID_GEN_RNDIS_CONFIG_PARAMETER: rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
		case OID_802_3_MAXIMUM_LIST_SIZE:    rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 1); return;
		case OID_802_3_MULTICAST_LIST:       rndis_query_cmplt32(RNDIS_STATUS_NOT_SUPPORTED, 0); return;
		case OID_802_3_MAC_OPTIONS:          rndis_query_cmplt32(RNDIS_STATUS_NOT_SUPPORTED, 0); return;
		case OID_GEN_MAC_OPTIONS:            rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, /*MAC_OPT*/ 0); return;
		case OID_802_3_RCV_ERROR_ALIGNMENT:  rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
		case OID_802_3_XMIT_ONE_COLLISION:   rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
		case OID_802_3_XMIT_MORE_COLLISIONS: rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
		case OID_GEN_XMIT_OK:                rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, usb_eth_stat.txok); return;
		case OID_GEN_RCV_OK:                 rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, usb_eth_stat.rxok); return;
		case OID_GEN_RCV_ERROR:              rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, usb_eth_stat.rxbad); return;
		case OID_GEN_XMIT_ERROR:             rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, usb_eth_stat.txbad); return;
		case OID_GEN_RCV_NO_BUFFER:          rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
		default:                             rndis_query_cmplt(RNDIS_STATUS_FAILURE, NULL, 0); return;
	}
}

void rndis_handle_set_msg()
{
	rndis_set_cmplt_t *c;
	rndis_set_msg_t *m;
	rndis_Oid_t oid;

	c = (rndis_set_cmplt_t *)encapsulated_buffer;
	m = (rndis_set_msg_t *)encapsulated_buffer;

	oid = m->Oid;
	c->MessageType = REMOTE_NDIS_SET_CMPLT;
	c->MessageLength = sizeof(rndis_set_cmplt_t);
	c->Status = RNDIS_STATUS_SUCCESS;

	switch (oid)
	{
		/* Parameters set up in 'Advanced' tab */
		case OID_GEN_RNDIS_CONFIG_PARAMETER:
			{
                rndis_config_parameter_t *p;
				char *ptr = (char *)m;
				ptr += sizeof(rndis_generic_msg_t);
				ptr += m->InformationBufferOffset;
				p = (rndis_config_parameter_t *)ptr;
//todo				rndis_handle_config_parm(ptr, p->ParameterNameOffset, p->ParameterValueOffset, p->ParameterNameLength, p->ParameterValueLength);
			}
			break;

		/* Mandatory general OIDs */
		case OID_GEN_CURRENT_PACKET_FILTER:
			oid_packet_filter = *((uint32_t *)((uint8_t *)&(m->RequestId) + m->InformationBufferOffset));
			if (oid_packet_filter)
			{
//todo				rndis_packetFilter(oid_packet_filter);
				rndis_state = rndis_data_initialized;
			}
			else
			{
				rndis_state = rndis_initialized;
			}
			break;

		case OID_GEN_CURRENT_LOOKAHEAD:
			break;

		case OID_GEN_PROTOCOL_OPTIONS:
			break;

		/* Mandatory 802_3 OIDs */
		case OID_802_3_MULTICAST_LIST:
			break;

		/* Power Managment: fails for now */
		case OID_PNP_ADD_WAKE_UP_PATTERN:
		case OID_PNP_REMOVE_WAKE_UP_PATTERN:
		case OID_PNP_ENABLE_WAKE_UP:
		default:
			c->Status = RNDIS_STATUS_FAILURE;
			break;
	}

	/* c->MessageID is same as before */
	USB_SIL_Write(CDC_CMD_EP, (uint8_t *)"\x01\x00\x00\x00\x00\x00\x00\x00", 8);
	SetEPTxCount(CDC_CMD_EP_IDX, 8);
	SetEPTxValid(CDC_CMD_EP_IDX);
	return;
}



void usbd_cdc_transfer()
{
	if (sended != 0 || rndis_tx_ptr == NULL || rndis_tx_size <= 0)
		return ;
	if (rndis_first_tx)
	{
		static uint8_t first[CDC_DATA_SIZE];
		rndis_data_packet_t *hdr;

		hdr = (rndis_data_packet_t *)first;
		memset(hdr, 0, sizeof(rndis_data_packet_t));
		hdr->MessageType = REMOTE_NDIS_PACKET_MSG;
		hdr->MessageLength = sizeof(rndis_data_packet_t) + rndis_tx_size;
		hdr->DataOffset = sizeof(rndis_data_packet_t) - offsetof(rndis_data_packet_t, DataOffset);
		hdr->DataLength = rndis_tx_size;

		sended = CDC_DATA_SIZE - sizeof(rndis_data_packet_t);
		if (sended > rndis_tx_size) sended = rndis_tx_size;
		memcpy(first + sizeof(rndis_data_packet_t), rndis_tx_ptr, sended);

		USB_SIL_Write(CDC_DAT_EP_IN_IDX, (uint8_t *)&first, sizeof(rndis_data_packet_t) + sended);
		SetEPTxCount(CDC_DAT_EP_IN_IDX, sizeof(rndis_data_packet_t) + sended);
		SetEPTxValid(CDC_DAT_EP_IN_IDX);
	}
	else
	{
		int n = rndis_tx_size;
		if (n > CDC_DATA_SIZE) n = CDC_DATA_SIZE;
		USB_SIL_Write(CDC_DAT_EP_IN_IDX, rndis_tx_ptr, n);
		SetEPTxCount(CDC_DAT_EP_IN_IDX, n);
		SetEPTxValid(CDC_DAT_EP_IN_IDX);
		sended = n;
	}
	return;
}

void handle_packet(const char *data, int size)
{
	rndis_data_packet_t *p;
	p = (rndis_data_packet_t *)data;
	if (size < sizeof(rndis_data_packet_t))
		return;
	if (p->MessageType != REMOTE_NDIS_PACKET_MSG || p->MessageLength != size)
		return;
	if (p->DataOffset + /*offsetof(rndis_data_packet_t, DataOffset)*/ 8 + p->DataLength != size)
	{
		usb_eth_stat.rxbad++;
		return;
	}
	usb_eth_stat.rxok++;
	if (rndis_rxproc != NULL)
		rndis_rxproc(&rndis_rx_buffer[p->DataOffset + offsetof(rndis_data_packet_t, DataOffset)], p->DataLength);
}

uint8_t rndis_can_send(void)
{
	return rndis_tx_size <= 0;
}

uint8_t rndis_send(const void *data, int size)
{
	if (size <= 0 ||
		size > ETH_MAX_PACKET_SIZE ||
		rndis_tx_size > 0) return 0;

	__disable_irq();
	rndis_first_tx = 1;
	rndis_tx_ptr = (uint8_t *)data;
	rndis_tx_size = size;
	rndis_sended = 0;
	__enable_irq();

	return 1;
}
