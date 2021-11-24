/**
  ******************************************************************************
  * @file    usb_desc.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Descriptors for Virtual Com Port Demo
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"

/* USB Standard Device Descriptor */
const uint8_t RNDIS_DeviceDescriptor[] =
{
		0x12,   /* bLength */
		USB_DEVICE_DESCRIPTOR_TYPE,     /* bDescriptorType */
		0x00,
		0x02,   /* bcdUSB = 2.00 */
		0xE0,   /* bDeviceClass: Wireless Controller (dirty haxx with vid&pid) */
		0x00,   /* bDeviceSubClass: Ethernet networking Control Model */
		0x00,   /* bDeviceProtocol */
		0x40,   /* bMaxPacketSize EP0 */
		0x44,
		0x4E,   /* idVendor = 0x4E44 */
		0x53,
		0x49,   /* idProduct = 0x4953 */
		0xFF,
		0xFF,   /* bcdDevice = 2.00 */
		1,      /* Index of string descriptor describing manufacturer */
		2,      /* Index of string descriptor describing product */
		3,      /* Index of string descriptor describing the device's serial number */
		0x01    /* bNumConfigurations */
};

const uint8_t RNDIS_ConfigDescriptor[] =
{
		/*Configuration Descriptor*/
		0x09,   /* bLength: Configuration Descriptor size */
		USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */
		RNDIS_SIZ_CONFIG_DESC,       /* wTotalLength:no of returned bytes */
		0x00,
		0x02,   /* bNumInterfaces: 2 interface */
		0x01,   /* bConfigurationValue: Configuration value */
		0x00,   /* iConfiguration: Index of string descriptor describing the configuration - unused */
		0x40,   /* bmAttributes: self powered */
		0x01,   /* MaxPower x*2mA = 2 mA */
		/******************** Interface association descriptor (IAD) ******************/
		0x08,   /* bLength */
		0x0B,   /* bDescriptorType */
		CDC_INTERFACE_IDX, /* bFirstInterface */
		0x02,   /* bInterfaceCount */
		0xE0,   /* bFunctionClass (Wireless Controller) */
		0x01,   /* bFunctionSubClass */
		0x03,   /* bFunctionProtocol */
		0x00,   /* iFunction */
		/********************  CDC interfaces ********************/
		/*Interface Descriptor*/
		0x09,   /* bLength: Interface Descriptor size */
		USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
		/* Interface descriptor type */
		CDC_INTERFACE_IDX,   /* bInterfaceNumber: Number of Interface */
		0x00,   /* bAlternateSetting: Alternate setting */
		0x01,   /* bNumEndpoints: One endpoints used */
		0xE0,   /* bInterfaceClass: Wireless Controller */
		0x01,   /* bInterfaceSubClass */
		0x03,   /* bInterfaceProtocol: Remote NDIS */
		0x00,   /* iInterface: */
		/*Header Functional Descriptor*/
		0x05,   /* bLength: Endpoint Descriptor size */
		0x24,   /* bDescriptorType: CS_INTERFACE */
		0x00,   /* bDescriptorSubtype: Header Func Desc */
		0x10,
		0x01,   /* bcdCDC: spec release number: 1.10 */
		/*Call Management Functional Descriptor*/
		0x05,   /* bFunctionLength */
		0x24,   /* bDescriptorType: CS_INTERFACE */
		0x01,   /* bDescriptorSubtype: Call Management Func Desc */
		0x00,   /* bmCapabilities: D0+D1 */
		CDC_INTERFACE_IDX + 1,   /* bDataInterface: 2 */
		/*Abstract Control Management Functional Descriptor*/
		0x04,   /* bFunctionLength */
		0x24,   /* bDescriptorType: CS_INTERFACE */
		0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
		0x00,   /* bmCapabilities:  Device supports the notification Network_Connection */
		/*Union Functional Descriptor*/
		0x05,   /* bFunctionLength */
		0x24,   /* bDescriptorType: CS_INTERFACE */
		0x06,   /* bDescriptorSubtype: Union func desc */
		CDC_INTERFACE_IDX,   /* bControlInterface: RNDIS Communications Control */
		CDC_INTERFACE_IDX + 1,   /* bSubordinateInterface: RNDIS Ethernet Data */
		/*Endpoint 1 CDC Descriptor*/
		0x07,   /* bLength: Endpoint Descriptor size */
		USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
		CDC_CMD_EP,   /* bEndpointAddress: (IN1) */
		0x03,   /* bmAttributes: Interrupt */
		CDC_INTERFACE_SIZE,   /* wMaxPacketSize: */
		0x00,
		0x01,   /* bInterval: 1 ms */
		/* CDC Data interface descriptor*/
		0x09,   /* bLength: Endpoint Descriptor size */
		USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
		CDC_INTERFACE_IDX + 1,   /* bInterfaceNumber: Number of Interface */
		0x00,   /* bAlternateSetting: Alternate setting */
		0x02,   /* bNumEndpoints: Two endpoints used */
		0x0A,   /* bInterfaceClass: CDC */
		0x00,   /* bInterfaceSubClass: */
		0x00,   /* bInterfaceProtocol: */
		0x00,   /* iInterface: */
		/*Endpoint OUT Descriptor*/
		0x07,   /* bLength: Endpoint Descriptor size */
		USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
		CDC_DAT_OUT_EP,   /* bEndpointAddress (OUT3)*/
		0x02,   /* bmAttributes: Bulk */
		CDC_DATA_SIZE,   /* wMaxPacketSize: */
		0x00,
		0x00,   /* bInterval: ignore for Bulk transfer */
		/*Endpoint IN Descriptor*/
		0x07,   /* bLength: Endpoint Descriptor size */
		USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
		CDC_DAT_IN_EP,   /* bEndpointAddress: (IN3) */
		0x02,   /* bmAttributes: Bulk */
		CDC_DATA_SIZE,             /* wMaxPacketSize: */
		0x00,
		0x00    /* bLength: Endpoint Descriptor size */
};

/* USB String Descriptors */
const uint8_t RNDIS_StringLangID[RNDIS_SIZ_STRING_LANGID] =
{
		RNDIS_SIZ_STRING_LANGID,
		USB_STRING_DESCRIPTOR_TYPE,
		0x09,
		0x04 /* LangID = 0x0409: U.S. English */
};

const uint8_t RNDIS_StringVendor[RNDIS_SIZ_STRING_VENDOR] =
{
		RNDIS_SIZ_STRING_VENDOR,     /* Size of Vendor string */
		USB_STRING_DESCRIPTOR_TYPE,             /* bDescriptorType*/
		/* Manufacturer: "STMicroelectronics" */
		'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
		'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
		'c', 0, 's', 0
};

const uint8_t RNDIS_StringProduct[RNDIS_SIZ_STRING_PRODUCT] =
{
		RNDIS_SIZ_STRING_PRODUCT,          /* bLength */
		USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
		/* Product name: "STM32 RemoteNDIS (RNDIS)" */
		'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, ' ', 0, 'R', 0, 'e', 0,
		'm', 0, 'o', 0, 't', 0, 'e', 0, 'N', 0, 'D', 0, 'I', 0, 'S', 0,
		' ', 0, '(', 0, 'R', 0, 'N', 0, 'D', 0, 'I', 0, 'S', 0, ')', 0
};

uint8_t RNDIS_StringSerial[RNDIS_SIZ_STRING_SERIAL] =
{
		RNDIS_SIZ_STRING_SERIAL,           /* bLength */
		USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
		0 //Fills in IntToUnicode()
};

const uint8_t RNDIS_StringConfig[RNDIS_SIZ_STRING_CONFIG] =
{
		RNDIS_SIZ_STRING_CONFIG,
		USB_STRING_DESCRIPTOR_TYPE,
		/* Interface: "RNDIS Config" */
		'R', 0, 'N', 0, 'D', 0, 'I', 0, 'S', 0, ' ', 0,
		'C', 0, 'o', 0, 'n', 0, 'f', 0, 'i', 0, 'g', 0
};

const uint8_t RNDIS_StringInterface[RNDIS_SIZ_STRING_INTERFACE] =
{
		RNDIS_SIZ_STRING_INTERFACE,
		USB_STRING_DESCRIPTOR_TYPE,
		/* Interface: "RNDIS Interface" */
		'R', 0, 'N', 0, 'D', 0, 'I', 0, 'S', 0, ' ', 0,
		'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0
};

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
