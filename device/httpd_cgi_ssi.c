/*
 * httpd_cgi_ssi.c
 *
 *  Created on: 6 Dec 2021
 *      Author: v.simonenko
 */

#include "lwip/apps/httpd.h"
#include "device.h"

/******************* SSI Section *******************/

const char IsChecked[] = "checked";

const char *SSI_TAGS[] =
{
		"IsLED",	// 0
};

uint16_t SSI_Handler(int iIndex, char *pcInsert, int iInsertLen)
{
	pcInsert[0] = '\0';
	switch (iIndex)
	//iIndex of element in SSI_TAGS array
	{
	case 0: // is LED on
		if(!GPIO_ReadInputDataBit(GPIOC, PIN_LED))
			strcpy(pcInsert, IsChecked);
		break;
	default:
		return 0;

	}
	return strlen(pcInsert);
}

/******************* CGI Section *******************/

/**
 * @brief  CGI handler for LED control
 */
const char* CGI_LEDS_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	uint32_t i = 0;

	/* We have only one SSI handler iIndex = 0 */
	if (iIndex == 0)
	{
		/* All leds off */
		GPIO_SetBits(GPIOC, PIN_LED);

		/* Check cgi parameter : example GET /leds.cgi?led=1&led=2 */
		for (i = 0; i < iNumParams; i++)
		{
			/* check parameter "led" */
			if (strcmp(pcParam[i], "led") == 0)
			{
				/* switch led1 ON if 1 */
				if (strcmp(pcValue[i], "1") == 0)
					GPIO_ResetBits(GPIOC, PIN_LED);
			}
		}
	}
	/* uri to send after cgi call*/
	return "/index.shtml";
}

/* CGI call table */
const tCGI CGI_Handlers[] =
{
		{ "/leds.cgi", CGI_LEDS_Handler },
};

void LwIP_HTTPD_Init()
{
	http_set_ssi_handler(SSI_Handler, SSI_TAGS, sizeof(SSI_TAGS) / sizeof(SSI_TAGS[0]));
	http_set_cgi_handlers(CGI_Handlers, sizeof(CGI_Handlers) / sizeof(tCGI));
    httpd_init();
}
