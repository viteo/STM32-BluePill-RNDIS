#ifndef GENERIC_H_
#define GENERIC_H_

#include "stm32f10x.h"

#define ROUND_DIVIDE(dividend, divisor) (((dividend) + (divisor) / 2) / (divisor))

void DWT_Delay_Init();
void DWT_Delay_us(uint32_t delay_us);
void DWT_Delay_ms(uint32_t delay_ms);
void array_copy_8(volatile uint8_t *dst, volatile uint8_t *src, volatile uint8_t size);
void array_copy_32(volatile uint32_t *dst, volatile uint32_t *src, volatile uint8_t size);
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8_t reverse_byte(uint8_t BYTE);

#endif /* GENERIC_H_ */
