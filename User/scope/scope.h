#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>


void USART1_Config(void);
int fputc(int , FILE *f);
void UART_Send_Array(USART_TypeDef* USARTx,u8 * send_array,unsigned char array_size);  
#endif 
