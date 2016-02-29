#include "stm32f4xx.h"
#include "my_usart.h"
#include "stdio.h"

uint8_t i;
uint8_t MSB, LSB;
int16_t Xg, Zg;                                 // 16-bit values from accelerometer
int16_t x_array[100];                           // 100 samples for X-axis
int16_t z_array[100];                           // 100 samples for Z-axis
float x_average;                                // x average of samples
float z_average;                                // z average of samples
float zx_theta;                                 // degrees between Z and X planes
char print_buffer[20];                          // printing the values in Putty

  SPI_InitTypeDef SPI_InitTypeDefStruct;
  GPIO_InitTypeDef GPIO_InitTypeDefStruct;
void SPI_send(uint8_t address, uint8_t data);
uint8_t SPI_read(uint8_t address);
void Sort_Signed(int16_t A[], uint8_t L);       // Bubble sort min to max, input: Array/Length
float gToDegrees(float V, float H);             // output: degrees between two planes, input: Vertical/Horizontal force
//chu manh cuong fig bug SPI

int main(void)
{
	GPIO_Config();
  USART_Config();

 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE);
 
  SPI_InitTypeDefStruct.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
  SPI_InitTypeDefStruct.SPI_Mode              = SPI_Mode_Master;
  SPI_InitTypeDefStruct.SPI_DataSize          = SPI_DataSize_8b;
  SPI_InitTypeDefStruct.SPI_CPOL              = SPI_CPOL_High;
  SPI_InitTypeDefStruct.SPI_CPHA              = SPI_CPHA_2Edge;
  SPI_InitTypeDefStruct.SPI_NSS               = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
  SPI_InitTypeDefStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitTypeDefStruct.SPI_FirstBit          = SPI_FirstBit_MSB;
  SPI_Init(SPI1, &SPI_InitTypeDefStruct);

  GPIO_InitTypeDefStruct.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_6;
  GPIO_InitTypeDefStruct.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitTypeDefStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitTypeDefStruct);

  GPIO_InitTypeDefStruct.GPIO_Pin   = GPIO_Pin_3;
  GPIO_InitTypeDefStruct.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitTypeDefStruct.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOE, &GPIO_InitTypeDefStruct);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

  GPIO_SetBits(GPIOE, GPIO_Pin_3);
  SPI_Cmd(SPI1, ENABLE);
  printf("aaaaaaaaaaaaaaaaaa");
  SPI_send(0x23, 0xc9);                         // resetting the accelerometer internal circuit
  SPI_send(0x20, 0x67);                         // 100Hz data update rate, block data update disable, x/y/z enabled 
  SPI_send(0x24, 0x20);                         // Anti aliasing filter bandwidth 800Hz, 16G (very sensitive), no self-test, 4-wire interface
  SPI_send(0x10, 0x00);                         // Output(X) = Measurement(X) - OFFSET(X) * 32;
  SPI_send(0x11, 0x00);                         // Output(Y) = Measurement(Y) - OFFSET(Y) * 32;
  SPI_send(0x12, 0x00);                         // Output(Z) = Measurement(Z) - OFFSET(Z) * 32;

  while(1)                                      // X and Z axes example
  {
    for(i = 0; i < 100; i++)                    // getting 100 samples
    {
      MSB = SPI_read(0x29);                     // X-axis MSB
      LSB = SPI_read(0x28);                     // X-axis LSB
      Xg = (MSB << 8) | (LSB);                  // Merging
      x_array[i] = Xg;
   
      MSB = SPI_read(0x2d);                     // Z-axis MSB
      LSB = SPI_read(0x2c);                     // Z-axis LSB
      Zg = (MSB << 8) | (LSB);                  // Merging
      z_array[i] = Zg;
    }
 
    Sort_Signed(x_array, 100);                  // Sorting min to max
    Sort_Signed(z_array, 100);                  // Sorting min to max
 
    x_average = 0;
    z_average = 0;
    for(i = 10; i < 90; i++)                    // removing 10 samples from bottom and 10 from top
    {
      x_average += x_array[i];                  // summing up
      z_average += z_array[i];                  // summing up
    }
 
    x_average /= 80;                            // dividing by the number of samples used
    x_average /= -141;                          // converting to meters per second squared
 
    z_average /= 80;                            // dividing by the number of samples used
    z_average /= -141;                          // converting to meters per second squared
 
   // zx_theta = gToDegrees(z, x);                // getting the degrees between Z and X planes
 
   // sprintf(print_buffer, "x: %.0f\tz: %.0f\tZ-X: %.0f", x_average, -z_average, zx_theta);
 printf("x: %.0f\tz: %.0f\t \n",x_average,-z_average);
   // (void)SHELL->Print(print_buffer);           // printing in Putty
  }
}



void SPI_send(uint8_t address, uint8_t data)
{
  GPIO_ResetBits(GPIOE, GPIO_Pin_3);
 
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
  SPI_I2S_SendData(SPI1, address);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  SPI_I2S_ReceiveData(SPI1);
 
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
  SPI_I2S_SendData(SPI1, data);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  SPI_I2S_ReceiveData(SPI1);
 
  GPIO_SetBits(GPIOE, GPIO_Pin_3);
}



uint8_t SPI_read(uint8_t address)
{
  GPIO_ResetBits(GPIOE, GPIO_Pin_3); 
  address = 0x80 | address;                         // 0b10 - reading and clearing status
  
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
  SPI_I2S_SendData(SPI1, address);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  SPI_I2S_ReceiveData(SPI1);
 
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
  SPI_I2S_SendData(SPI1, 0x00);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
 
  GPIO_SetBits(GPIOE, GPIO_Pin_3);
 
  return SPI_I2S_ReceiveData(SPI1);
}



void Sort_Signed(int16_t A[], uint8_t L)
{
  uint8_t i = 0;
  uint8_t status = 1;
 
  while(status == 1)
  {
    status = 0;
    for(i = 0; i < L-1; i++)
    {
      if (A[i] > A[i+1])
      {
        A[i]^=A[i+1];
        A[i+1]^=A[i];
        A[i]^=A[i+1];
        status = 1;    
      }
    }
  }
}

/*

float gToDegrees(float V, float H)               // refer to the orientation pic above
{
  float retval;
  uint16_t orientation;
 
  if (H == 0) H = 0.001;                         // preventing division by zero
  if (V == 0) V = 0.001;                         // preventing division by zero
 
  if ((H > 0) && (V > 0)) orientation = 0;
  if ((H < 0) && (V > 0)) orientation = 90; 
  if ((H < 0) && (V < 0)) orientation = 180;
  if ((H > 0) && (V < 0)) orientation = 270;
 
  retval = ((atan(V/H)/3.14159)*180);
  if (retval < 0) retval += 90;
  retval = abs(retval) + orientation;
  return retval;
}
*/