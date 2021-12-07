/**
  ******************************************************************************
  * @file    usb_desc.h
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Descriptor Header for RNDIS Device
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05
#define USB_DEVICE_QUALIFIER_DESCRIPTOR         0x06

#define CDC_INTERFACE_SIZE         8
#define CDC_DATA_SIZE              64

#define RNDIS_SIZ_DEVICE_DESC        18
#define RNDIS_SIZ_CONFIG_DESC        75
#define RNDIS_SIZ_STRING_LANGID      4
#define RNDIS_SIZ_STRING_VENDOR      38
#define RNDIS_SIZ_STRING_PRODUCT     50
#define RNDIS_SIZ_STRING_SERIAL      26
#define RNDIS_SIZ_STRING_CONFIG      26
#define RNDIS_SIZ_STRING_INTERFACE   32

/* Exported functions ------------------------------------------------------- */
extern const uint8_t RNDIS_DeviceDescriptor[RNDIS_SIZ_DEVICE_DESC];
extern const uint8_t RNDIS_ConfigDescriptor[RNDIS_SIZ_CONFIG_DESC];

extern const uint8_t RNDIS_StringLangID[RNDIS_SIZ_STRING_LANGID];
extern const uint8_t RNDIS_StringVendor[RNDIS_SIZ_STRING_VENDOR];
extern const uint8_t RNDIS_StringProduct[RNDIS_SIZ_STRING_PRODUCT];
extern uint8_t RNDIS_StringSerial[RNDIS_SIZ_STRING_SERIAL];
extern const uint8_t RNDIS_StringConfig[RNDIS_SIZ_STRING_CONFIG];
extern const uint8_t RNDIS_StringInterface[RNDIS_SIZ_STRING_INTERFACE];

#endif /* __USB_DESC_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
