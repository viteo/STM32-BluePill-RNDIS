/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Endpoint routines
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
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "rndis.h"
#include <string.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback (void)
{

}

/*******************************************************************************
* Function Name  : EP2_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP2_IN_Callback(void)
{
	rndis_first_tx = 0;
	rndis_sended += sended;
	rndis_tx_size -= sended;
	rndis_tx_ptr += sended;
	sended = 0;
	usbd_cdc_transfer();
}

/*******************************************************************************
* Function Name  : EP2_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
	static int rndis_received = 0;
	int32_t rxCount = GetEPRxCount(CDC_DAT_EP_OUT_IDX);
	USB_SIL_Read(CDC_DAT_EP_OUT_IDX, (uint8_t*) usb_rx_buffer);
	if (rndis_received + rxCount > RNDIS_RX_BUFFER_SIZE)
	{
		rndis_received = 0;
	}
	else
	{
		if (rndis_received + rxCount <= RNDIS_RX_BUFFER_SIZE)
		{
			memcpy(&rndis_rx_buffer[rndis_received], usb_rx_buffer, rxCount);
			rndis_received += rxCount;
			if (rxCount != CDC_DATA_SIZE)
			{
				handle_packet(rndis_rx_buffer, rndis_received);
				rndis_received = 0;
			}
		}
		else
		{
			rndis_received = 0;
		}
	}
//	DCD_EP_PrepareRx(pdev, RNDIS_DATA_OUT_EP, (uint8_t*) usb_rx_buffer, RNDIS_DATA_OUT_SZ);
	SetEPRxCount(CDC_DAT_EP_OUT_IDX, CDC_DATA_SIZE);
	SetEPRxStatus(CDC_DAT_EP_OUT_IDX, EP_RX_VALID);
}


/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SOF_Callback(void)
{
	usbd_cdc_transfer();
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

