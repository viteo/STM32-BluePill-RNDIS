/*
 * rndis.c
 *
 *  Created on: 13 oct 2021
 *      Author: v.simonenko
 */

#include "rndis.h"
#include "device.h"
#include "usb_conf.h"
#include "usb_sil.h"
#include "usb_desc.h"
#include "usb_lib.h"
#include <string.h>

const uint32_t OIDSupportedList[OID_LIST_LENGTH] =
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
    OID_802_3_PERMANENT_ADDRESS,
    OID_802_3_CURRENT_ADDRESS,
    OID_802_3_MULTICAST_LIST,
    OID_802_3_MAXIMUM_LIST_SIZE,
    OID_802_3_MAC_OPTIONS
};
uint32_t oid_packet_filter = 0x0000000;

uint8_t encapsulated_buffer[ENC_BUF_SIZE];
uint8_t usb_rx_buffer[RNDIS_RX_BUFFER_SIZE];

uint8_t *rndis_rx_ptr = NULL;
int rndis_rx_size = 0;

uint8_t *rndis_tx_ptr = NULL;
uint8_t rndis_first_tx = 1;
int rndis_tx_size = 0;
int usb_sended = 0;

/* We have data to send back */
void rndis_cmd_return_ok()
{
	USB_SIL_Write(CDC_CMD_EP, (uint8_t*) "\x01\x00\x00\x00\x00\x00\x00\x00", 8);
	SetEPTxCount(CDC_CMD_EP_IDX, 8);
	SetEPTxValid(CDC_CMD_EP_IDX);
}

void rndis_query_cmplt(int status, const void *data, int size)
{
	rndis_query_cmplt_t *c;
	c = (rndis_query_cmplt_t*) encapsulated_buffer;
	c->MessageType = REMOTE_NDIS_QUERY_CMPLT;
	c->MessageLength = sizeof(rndis_query_cmplt_t) + size;
	c->InformationBufferLength = size;
	c->InformationBufferOffset = 16;
	c->Status = status;
	memcpy(c + 1, data, size);
	rndis_cmd_return_ok();
}

void rndis_query_cmplt32(int status, uint32_t data)
{
	rndis_query_cmplt(status, &data, sizeof(uint32_t));
}

void rndis_cmd_query()
{
	switch (((rndis_query_msg_t *)encapsulated_buffer)->Oid)
	{
		case OID_GEN_SUPPORTED_LIST:         rndis_query_cmplt(RNDIS_STATUS_SUCCESS, OIDSupportedList, 4 * OID_LIST_LENGTH); return;
		case OID_GEN_VENDOR_DRIVER_VERSION:  rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0x00001000);  return;
		case OID_802_3_CURRENT_ADDRESS:
		case OID_802_3_PERMANENT_ADDRESS:
		{
			uint8_t hwaddr[6]  = {MAC_ADDR0,MAC_ADDR1,MAC_ADDR2,MAC_ADDR3,MAC_ADDR4,MAC_ADDR5};
			rndis_query_cmplt(RNDIS_STATUS_SUCCESS, hwaddr, 6);
		}
		return;
		case OID_GEN_MEDIA_SUPPORTED:
		case OID_GEN_MEDIA_IN_USE:           rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, NDIS_MEDIUM_802_3); return;
		case OID_GEN_HARDWARE_STATUS:        rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
		case OID_GEN_LINK_SPEED:             rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, RNDIS_LINK_SPEED / 100); return;
		case OID_GEN_VENDOR_ID:              rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0x00FFFFFF); return;
		case OID_GEN_VENDOR_DESCRIPTION:     rndis_query_cmplt(RNDIS_STATUS_SUCCESS, RNDIS_VENDOR, strlen(RNDIS_VENDOR) + 1); return;
		case OID_GEN_CURRENT_PACKET_FILTER:  rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, oid_packet_filter); return;
		case OID_GEN_MAXIMUM_FRAME_SIZE:     rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE - ETH_HEADER_SIZE); return;
		case OID_GEN_RECEIVE_BLOCK_SIZE:
		case OID_GEN_TRANSMIT_BLOCK_SIZE:
		case OID_GEN_MAXIMUM_TOTAL_SIZE:     rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE); return;
		case OID_GEN_MEDIA_CONNECT_STATUS:   rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, NDIS_MEDIA_STATE_CONNECTED); return;
		case OID_802_3_MAXIMUM_LIST_SIZE:    rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 1); return;
		case OID_802_3_MULTICAST_LIST:
		case OID_802_3_MAC_OPTIONS:          rndis_query_cmplt32(RNDIS_STATUS_NOT_SUPPORTED, 0); return;
		case OID_GEN_MAC_OPTIONS:
		case OID_802_3_RCV_ERROR_ALIGNMENT:  rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
		default:                             rndis_query_cmplt(RNDIS_STATUS_FAILURE, NULL, 0); return;
	}
}

void rndis_cmd_set_msg()
{
	rndis_set_cmplt_t *c;
	rndis_set_msg_t *m;
	rndis_Oid_t oid;

	c = (rndis_set_cmplt_t*) encapsulated_buffer;
	m = (rndis_set_msg_t*) encapsulated_buffer;

	oid = m->Oid;
	c->MessageType = REMOTE_NDIS_SET_CMPLT;
	c->MessageLength = sizeof(rndis_set_cmplt_t);
	c->Status = RNDIS_STATUS_SUCCESS;

	switch (oid)
	{
	/* Parameters set up in 'Advanced' tab */
	case OID_GEN_RNDIS_CONFIG_PARAMETER:
	{
		char *ptr = (char*) m;
		ptr += sizeof(rndis_generic_msg_t);
		ptr += m->InformationBufferOffset;
	}
		break;

		/* Mandatory general OIDs */
	case OID_GEN_CURRENT_PACKET_FILTER:
		oid_packet_filter = *((uint32_t*) ((uint8_t*) &(m->RequestId) + m->InformationBufferOffset));
		break;

	case OID_GEN_CURRENT_LOOKAHEAD:
	case OID_GEN_PROTOCOL_OPTIONS:
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

	rndis_cmd_return_ok();
	return;
}

void rndis_cmd_message()
{
	switch (((rndis_generic_msg_t*) encapsulated_buffer)->MessageType)
	{
	case REMOTE_NDIS_INITIALIZE_MSG:
	{
		rndis_initialize_cmplt_t *m;
		m = ((rndis_initialize_cmplt_t*) encapsulated_buffer);
		/* m->MessageID is same as before */
		m->MessageType = REMOTE_NDIS_INITIALIZE_CMPLT;
		m->MessageLength = sizeof(rndis_initialize_cmplt_t);
		m->MajorVersion = RNDIS_MAJOR_VERSION;
		m->MinorVersion = RNDIS_MINOR_VERSION;
		m->Status = RNDIS_STATUS_SUCCESS;
		m->DeviceFlags = RNDIS_DF_CONNECTIONLESS;
		m->Medium = RNDIS_MEDIUM_802_3;
		m->MaxPacketsPerTransfer = 1;
		m->MaxTransferSize = RNDIS_RX_BUFFER_SIZE;
		m->PacketAlignmentFactor = 0;
		m->AfListOffset = 0;
		m->AfListSize = 0;
		rndis_cmd_return_ok();
	}
		break;

	case REMOTE_NDIS_QUERY_MSG:
		rndis_cmd_query();
		break;

	case REMOTE_NDIS_SET_MSG:
		rndis_cmd_set_msg();
		break;

	case REMOTE_NDIS_RESET_MSG:
	{
		rndis_reset_cmplt_t *m;
		m = ((rndis_reset_cmplt_t*) encapsulated_buffer);
		m->MessageType = REMOTE_NDIS_RESET_CMPLT;
		m->MessageLength = sizeof(rndis_reset_cmplt_t);
		m->Status = RNDIS_STATUS_SUCCESS;
		m->AddressingReset = 1; /* Make it look like we did something */
		/* m->AddressingReset = 0; - Windows halts if set to 1 for some reason */
		rndis_cmd_return_ok();
	}
		break;

	case REMOTE_NDIS_KEEPALIVE_MSG:
	{
		rndis_keepalive_cmplt_t *m;
		m = (rndis_keepalive_cmplt_t*) encapsulated_buffer;
		m->MessageType = REMOTE_NDIS_KEEPALIVE_CMPLT;
		m->MessageLength = sizeof(rndis_keepalive_cmplt_t);
		m->Status = RNDIS_STATUS_SUCCESS;
		rndis_cmd_return_ok();
	}
		break;
	default:
		break;
	}
}

void usbd_cdc_transfer()
{
	if (usb_sended != 0 || rndis_tx_ptr == NULL || rndis_tx_size <= 0)
		return;
	if (rndis_first_tx)
	{
		static uint8_t header_buf[CDC_DATA_SIZE];
		rndis_data_packet_t *header;

		header = (rndis_data_packet_t*) header_buf;
		memset(header, 0, sizeof(rndis_data_packet_t));
		header->MessageType = REMOTE_NDIS_PACKET_MSG;
		header->MessageLength = sizeof(rndis_data_packet_t) + rndis_tx_size;
		header->DataOffset = sizeof(rndis_data_packet_t) - offsetof(rndis_data_packet_t, DataOffset);
		header->DataLength = rndis_tx_size;

		usb_sended = CDC_DATA_SIZE - sizeof(rndis_data_packet_t);
		if (usb_sended > rndis_tx_size)
			usb_sended = rndis_tx_size;
		memcpy(header_buf + sizeof(rndis_data_packet_t), rndis_tx_ptr, usb_sended);

		USB_SIL_Write(CDC_DAT_EP_IN_IDX, (uint8_t*) &header_buf, sizeof(rndis_data_packet_t) + usb_sended);
		SetEPTxCount(CDC_DAT_EP_IN_IDX, sizeof(rndis_data_packet_t) + usb_sended);
		SetEPTxValid(CDC_DAT_EP_IN_IDX);
	}
	else
	{
		usb_sended = rndis_tx_size;
		if (usb_sended > CDC_DATA_SIZE)
			usb_sended = CDC_DATA_SIZE;
		USB_SIL_Write(CDC_DAT_EP_IN_IDX, rndis_tx_ptr, usb_sended);
		SetEPTxCount(CDC_DAT_EP_IN_IDX, usb_sended);
		SetEPTxValid(CDC_DAT_EP_IN_IDX);
	}
}

uint8_t rndis_data_pending()
{
	return rndis_rx_size != 0;
}

void rndis_handle_packet(const uint8_t *data, int size)
{
	rndis_data_packet_t *p;
	p = (rndis_data_packet_t*) data;
	if (size < sizeof(rndis_data_packet_t))
		return;
	if (p->MessageType != REMOTE_NDIS_PACKET_MSG || p->MessageLength != size)
		return;
	if (p->DataOffset + offsetof(rndis_data_packet_t, DataOffset) + p->DataLength != size)
		return;
	rndis_rx_ptr = &usb_rx_buffer[p->DataOffset + offsetof(rndis_data_packet_t, DataOffset)];
	rndis_rx_size = size;
}

uint8_t rndis_can_send(void)
{
	return rndis_tx_size <= 0;
}

void rndis_send(const void *data, int size)
{
	if (size <= 0 || size > ETH_MAX_PACKET_SIZE || rndis_tx_size > 0)
		return;

	rndis_first_tx = 1;
	rndis_tx_ptr = (uint8_t*) data;
	rndis_tx_size = size;
}
