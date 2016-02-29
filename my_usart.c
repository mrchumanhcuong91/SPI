#include"my_usart.h"
USART_InitTypeDef USART_InitStructure;
 GPIO_InitTypeDef GPIO_Usart_struct;
void GPIO_Config(void){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	// cau hinh GPIO --->> TX ,RX
  GPIO_Usart_struct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Usart_struct.GPIO_OType = GPIO_OType_PP;
  GPIO_Usart_struct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  GPIO_Usart_struct.GPIO_PuPd =GPIO_PuPd_UP;
  GPIO_Usart_struct.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_Usart_struct);
  	//
 GPIO_PinAFConfig(GPIOB, GPIO_PinSource6,GPIO_AF_USART1);
 GPIO_PinAFConfig(GPIOB, GPIO_PinSource7,GPIO_AF_USART1);
	//Enable GPIO for LED
}
void USART_Config(void)
{
// cap xung clock cho GPIO va USART
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//
  /* USARTx configured as follows:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART1, &USART_InitStructure);
	
  USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);// enable interrupts usart1
  USART_Cmd(USART1, ENABLE);

}
void USART_puts(volatile char *s,int t){

	while(t>0){
		// wait until data register is empty
		while( !(USART1->SR & 0x00000040) ); 
		USART_SendData(USART1, *s);
		*s++;
		t--;
	}
}
int fputc(int ch, FILE *f)
{
	while( !(USART1->SR & 0x00000040) );
      /* Place your implementation of fputc here */
      /* e.g. write a character to the USART */
      USART_SendData(USART1,(uint8_t)ch);

    /* Loop until the end of transmission */
      while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

      return ch;
}
