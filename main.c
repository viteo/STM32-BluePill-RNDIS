#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "generic.h"
#include "device.h"
#include "hw_config.h"
#include "usb_init.h"
#include "netconf.h"

int main()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = PIN_LED;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
/****/
	Set_System();
	Set_USBClock();
	DWT_Delay_Init();
	USB_Interrupts_Config(ENABLE);
	USB_Init();
/****/
//	LwIP_Init();

	uint32_t LocalTime = 0;
	while (1)
	{
//		LwIP_Pkt_Handle();
//		LwIP_Periodic_Handle(LocalTime);
		LocalTime += 10;

		if(LocalTime % 500 == 0)
			GPIO_ToggleBits(GPIOC, PIN_LED);
		DWT_Delay_ms(10);
	}
}
