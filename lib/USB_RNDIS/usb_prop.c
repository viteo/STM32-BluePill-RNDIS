/**
  ******************************************************************************
  * @file    usb_prop.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   All processing related to CDC
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
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"
#include "rndis.h"
#include "ndis_protocol.h"
#include "rndis_protocol.h"
#include <string.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
{
		EP_NUM,
		1
};

DEVICE_PROP Device_Property =
{
		RNDIS_Init,
		RNDIS_Reset,
		RNDIS_Status_In,
		RNDIS_Status_Out,
		RNDIS_Data_Setup,
		RNDIS_NoData_Setup,
		RNDIS_Get_Interface_Setting,
		RNDIS_GetDeviceDescriptor,
		RNDIS_GetConfigDescriptor,
		RNDIS_GetStringDescriptor,
		0,
		0x40 /*Max Packet Size*/
};

USER_STANDARD_REQUESTS User_Standard_Requests =
{
		CDC_GetConfiguration,
		RNDIS_SetConfiguration,
		CDC_GetInterface,
		CDC_SetInterface,
		CDC_GetStatus,
		CDC_ClearFeature,
		CDC_SetEndPointFeature,
		CDC_SetDeviceFeature,
		RNDIS_SetDeviceAddress
};

ONE_DESCRIPTOR Device_Descriptor =
{
		(uint8_t*)RNDIS_DeviceDescriptor,
		RNDIS_SIZ_DEVICE_DESC
};

ONE_DESCRIPTOR Config_Descriptor =
{
		(uint8_t*)RNDIS_ConfigDescriptor,
		RNDIS_SIZ_CONFIG_DESC
};

ONE_DESCRIPTOR String_Descriptor[6] =
{
		{(uint8_t*)RNDIS_StringLangID, RNDIS_SIZ_STRING_LANGID},
		{(uint8_t*)RNDIS_StringVendor, RNDIS_SIZ_STRING_VENDOR},
		{(uint8_t*)RNDIS_StringProduct, RNDIS_SIZ_STRING_PRODUCT},
		{(uint8_t*)RNDIS_StringSerial, RNDIS_SIZ_STRING_SERIAL},
		{(uint8_t*)RNDIS_StringConfig, RNDIS_SIZ_STRING_CONFIG},
		{(uint8_t*)RNDIS_StringInterface, RNDIS_SIZ_STRING_INTERFACE}
};

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : RNDIS_Init.
* Description    : RNDIS init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void RNDIS_Init(void)
{
  /* Update the serial number string descriptor with the data from the unique ID*/
  Get_SerialNum();

  pInformation->Current_Configuration = 0;

  /* Connect the device */
  PowerOn();

  /* Perform basic device initialization operations */
  USB_SIL_Init();

  bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : RNDIS_Reset
* Description    : RNDIS reset routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void RNDIS_Reset(void)
{
	/* Set RNDIS DEVICE as not configured */
	pInformation->Current_Configuration = 0;

	/* Current Feature initialization */
	pInformation->Current_Feature = RNDIS_ConfigDescriptor[7];

	/* Set RNDIS DEVICE with the default Interface*/
	pInformation->Current_Interface = 0;

	SetBTABLE(BTABLE_ADDRESS);

	/* Initialize Endpoint 0 */
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_STALL);
	SetEPRxAddr(ENDP0, ENDP0_RXADDR);
	SetEPTxAddr(ENDP0, ENDP0_TXADDR);
	Clear_Status_Out(ENDP0);
	SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
	SetEPRxValid(ENDP0);

	/* Initialize CDC Endpoints */
	SetEPType(CDC_CMD_EP_IDX, EP_INTERRUPT);
	SetEPTxAddr(CDC_CMD_EP_IDX, ENDP1_CMDADDR);
	SetEPTxStatus(CDC_CMD_EP_IDX, EP_TX_NAK);
	SetEPRxStatus(CDC_CMD_EP_IDX, EP_RX_DIS);

	SetEPType(CDC_DAT_EP_IN_IDX, EP_BULK);
	SetEPTxAddr(CDC_DAT_EP_IN_IDX, ENDP2_TXADDR);
	SetEPTxStatus(CDC_DAT_EP_IN_IDX, EP_TX_NAK);
	SetEPRxStatus(CDC_DAT_EP_IN_IDX, EP_RX_DIS);

	SetEPType(CDC_DAT_EP_OUT_IDX, EP_BULK);
	SetEPRxAddr(CDC_DAT_EP_OUT_IDX, ENDP3_RXADDR);
	SetEPRxStatus(CDC_DAT_EP_OUT_IDX, EP_RX_VALID);
	SetEPTxStatus(CDC_DAT_EP_OUT_IDX, EP_TX_DIS);
	SetEPRxCount(CDC_DAT_EP_OUT_IDX, CDC_DATA_SIZE);
	SetEPRxValid(CDC_DAT_EP_OUT_IDX);

	/* Set this device to response on default address */
	SetDeviceAddress(0);

	bDeviceState = ATTACHED;
}

/*******************************************************************************
* Function Name  : RNDIS_SetConfiguration.
* Description    : Update the device state to configured.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void RNDIS_SetConfiguration(void)
{
	if (pInformation->Current_Configuration != 0)
	{
		/* Device configured */
		bDeviceState = CONFIGURED;
	}
}

/*******************************************************************************
* Function Name  : RNDIS_SetDeviceAddress.
* Description    : Update the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void RNDIS_SetDeviceAddress(void)
{
	bDeviceState = ADDRESSED;
}

/*******************************************************************************
* Function Name  : RNDIS_Status_In.
* Description    : RNDIS Status In Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void RNDIS_Status_In(void)
{
	rndis_cmd_message();
}

/*******************************************************************************
* Function Name  : RNDIS_Status_Out
* Description    : RNDIS Status OUT Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void RNDIS_Status_Out(void)
{

}

uint8_t* RNDIS_CopyData(uint16_t Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = 8;
		return NULL;
	}
	else
	{
		return encapsulated_buffer + pInformation->Ctrl_Info.Usb_wOffset;
	}
}

/*******************************************************************************
* Function Name  : RNDIS_Data_Setup
* Description    : handle the data class specific requests
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT RNDIS_Data_Setup(uint8_t RequestNo)
{
	switch(pInformation->USBbmRequestType & REQUEST_TYPE)
	{
	case CLASS_REQUEST:
		if (pInformation->USBwLength != 0) /* is data setup packet? */
		{
			/* Check if the request is Device-to-Host */
			if(pInformation->USBbmRequestType & 0x80)
			{
				pInformation->Ctrl_Info.Usb_wLength = ((rndis_generic_msg_t *)encapsulated_buffer)->MessageLength;
				pInformation->Ctrl_Info.Usb_wOffset = 0;
				pInformation->Ctrl_Info.CopyData = RNDIS_CopyData;
				return USB_SUCCESS;
			}
			else /* Host-to-Device requeset */
			{
				pInformation->Ctrl_Info.Usb_rLength = pInformation->USBwLength;
				pInformation->Ctrl_Info.Usb_rOffset = 0;
				pInformation->Ctrl_Info.CopyData = RNDIS_CopyData;
				return USB_SUCCESS;
			}
		}
		break;
/*
	case STANDARD_REQUEST:
#define USB_DEVICE_QUALIFIER_DESCRIPTOR 6
		if(RequestNo == USB_DEVICE_QUALIFIER_DESCRIPTOR)
			return USB_SUCCESS;
*/
	default:
		return USB_UNSUPPORT;
	}
	return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : RNDIS_NoData_Setup.
* Description    : handle the no data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT RNDIS_NoData_Setup(uint8_t RequestNo)
{
	return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : RNDIS_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
uint8_t *RNDIS_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : RNDIS_GetConfigDescriptor.
* Description    : get the configuration descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *RNDIS_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : RNDIS_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *RNDIS_GetStringDescriptor(uint16_t Length)
{
  uint8_t wValue0 = pInformation->USBwValue0;
  if (wValue0 >= sizeof(String_Descriptor) / sizeof(String_Descriptor[0]))
  {
    return NULL;
  }
  else
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
}

/*******************************************************************************
* Function Name  : RNDIS_Get_Interface_Setting.
* Description    : test the interface and the alternate setting according to the
*                  supported one.
* Input1         : uint8_t: Interface : interface number.
* Input2         : uint8_t: AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
RESULT RNDIS_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 1)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
