#include "stm32f4xx.h"
#include "stdio.h"
extern USART_InitTypeDef USART_InitStructure;
extern GPIO_InitTypeDef GPIO_Usart_struct,GPIO_LED;
extern  NVIC_InitTypeDef NVIC_Struct;
void USART_Config(void);
void GPIO_Config(void);
void USART_puts(volatile char *s,int t);
