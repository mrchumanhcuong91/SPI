#include "spi_accelerometer.h"
GPIO_InitTypeDef GPIO_StructForSPI1;
SPI_InitTypeDef SPI1_Struct;
/////////////////////////////////////////////////////////////////
void SPI1_InitFunction(void){
	/*config for GPIO SPI
	Base on schematic of F4 KIT:
	- PA5 : SPI_Clock
	- PA7 :SPI_MOSI
	- PA6: SPI_MISO
	- PE3 : CS
	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	//for SPI 
	GPIO_StructForSPI1.GPIO_Mode = GPIO_Mode_AF;
	GPIO_StructForSPI1.GPIO_OType = GPIO_OType_PP;
	GPIO_StructForSPI1.GPIO_Pin =GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_StructForSPI1.GPIO_PuPd =GPIO_PuPd_NOPULL;
	GPIO_StructForSPI1.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_StructForSPI1);
	//pin source
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	//for chip select
	GPIO_StructForSPI1.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_StructForSPI1.GPIO_OType = GPIO_OType_PP;
	GPIO_StructForSPI1.GPIO_Pin =GPIO_Pin_3;
	GPIO_StructForSPI1.GPIO_PuPd =GPIO_PuPd_UP;
	GPIO_StructForSPI1.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOE, &GPIO_StructForSPI1);

	//
	/*config for SPI
	SPI_Direction: Unidirectional or bidirectional. We need bidirectional here for read and write.
	SPI_Mode: Master or Slave - our stm32f4 shoulb be the master here!
	SPI_DataSize: You can send 8 Bits and 16 Bits - we use 8 Bits.
	SPI_CPOL: Clock polarity - I set this to High, as it is in the read&write protocol in LIS302DL datasheet.
	SPI_CPHA: Defines the edge for bit capture - I use 2nd edge.
	SPI_NSS: Chip select hardware/sofware - I set this so software.
	SPI_BaudRatePrescaler:  Defines the clock speed of our SPI - set it to maximum here (Prescaler 2)
	SPI_FirstBit: Starting with MSB or LSB ? - LIS302DL datasheet -> its MSB !
	SPI_CRCPolynomial: Polynomial for CRC calculation - we don't use it in this example.
	*/
	SPI1_Struct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI1_Struct.SPI_CPHA =SPI_CPHA_2Edge;//=1
	SPI1_Struct.SPI_CPOL=SPI_CPOL_High;
	SPI1_Struct.SPI_DataSize = SPI_DataSize_8b;
	SPI1_Struct.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI1_Struct.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI1_Struct.SPI_Mode=SPI_Mode_Master;
	SPI1_Struct.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1, &SPI1_Struct);
	//
	SPI_Cmd(SPI1, ENABLE);
}
///////////////////////////////////////////////////////////////
void SPI1_Write_Data(uint8_t address, uint8_t data){
	GPIO_ResetBits(GPIOE, GPIO_Pin_3);
 
while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); 
SPI_I2S_SendData(SPI1, address);
while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
SPI_I2S_ReceiveData(SPI1);
 
while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); 
SPI_I2S_SendData(SPI1, data);
while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
SPI_I2S_ReceiveData(SPI1);
 
GPIO_SetBits(GPIOE, GPIO_Pin_3);
}
////////////////////////////////////////////////////////////////////
uint8_t SPI1_Read_Data(uint8_t address){
	GPIO_ResetBits(GPIOE, GPIO_Pin_3); 
	address = 0x80 | address;
 
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); 
	SPI_I2S_SendData(SPI1, address);
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	SPI_I2S_ReceiveData(SPI1); //Clear RXNE bit
 
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); 
	SPI_I2S_SendData(SPI1, 0x00); //Dummy byte to generate clock
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
 
GPIO_SetBits(GPIOE, GPIO_Pin_3);
 
return  SPI_I2S_ReceiveData(SPI1);
}