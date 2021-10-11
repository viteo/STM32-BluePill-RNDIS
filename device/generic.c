#include "generic.h"
#include "stm32f10x_rcc.h"

static RCC_ClocksTypeDef RCC_Clocks;

void DWT_Delay_Init()
{
	RCC_GetClocksFreq(&RCC_Clocks);
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	DWT->CYCCNT = 0;
}

void DWT_Delay_us(uint32_t delay)
{
	uint32_t initial_ticks = DWT->CYCCNT;
	delay *= (RCC_Clocks.HCLK_Frequency / 1000000);
	while (DWT->CYCCNT - initial_ticks < delay);
}

void DWT_Delay_ms(uint32_t delay)
{
	uint32_t initial_ticks = DWT->CYCCNT;
	delay *= (RCC_Clocks.HCLK_Frequency / 1000);
	while (DWT->CYCCNT - initial_ticks < delay);
}

void array_copy_8(volatile uint8_t *dst, volatile uint8_t *src, volatile uint8_t size)
{
    while (size--)
        *dst++ = *src++;
}


void array_copy_32(volatile uint32_t *dst, volatile uint32_t *src, volatile uint8_t size)
{
	while (size--)
		*dst++ = *src++;
}


/**
 * @brief  Byte mirror view
 */
uint8_t reverse_byte(uint8_t BYTE)
{
	BYTE = (BYTE & 0xF0) >> 4 | (BYTE & 0x0F) << 4;
	BYTE = (BYTE & 0xCC) >> 2 | (BYTE & 0x33) << 2;
	BYTE = (BYTE & 0xAA) >> 1 | (BYTE & 0x55) << 1;
	return BYTE;
}

//SPL GPIO Extention for bit toggling
/**
  * @brief  Toggles selected data port bits.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bits to be toggled.
  *   This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  GPIOx->ODR ^= GPIO_Pin;
}
