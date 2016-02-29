#include "stm32f4xx.h"
extern GPIO_InitTypeDef GPIO_StructForSPI1;
extern SPI_InitTypeDef SPI1_Struct;
void SPI1_InitFunction(void);
void SPI1_Write_Data(uint8_t address,uint8_t data);
uint8_t SPI1_Read_Data(uint8_t address);