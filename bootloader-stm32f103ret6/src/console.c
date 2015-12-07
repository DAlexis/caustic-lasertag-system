/*
 * console.c
 *
 *  Created on: 8 дек. 2015 г.
 *      Author: alexey
 */

#include "console.h"

#include "stm32f10x.h"

void initUart()
{
	USART_InitTypeDef usart1_init_struct;
		/* Bit configuration structure for GPIOA PIN9 and PIN10 */
	GPIO_InitTypeDef gpioa_init_struct;
	USART_ClockInitTypeDef USART_ClockInitStructure;

	/* Enalbe clock for USART1, AFIO and GPIOA */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO |
						   RCC_APB2Periph_GPIOA, ENABLE);

	/* GPIOA PIN9 alternative function Tx */
	gpioa_init_struct.GPIO_Pin = GPIO_Pin_9;
	gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
	gpioa_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpioa_init_struct);
	/* GPIOA PIN9 alternative function Rx */
	gpioa_init_struct.GPIO_Pin = GPIO_Pin_10;
	gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
	gpioa_init_struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &gpioa_init_struct);

	USART_ClockStructInit(&USART_ClockInitStructure);
	USART_ClockInit(USART1, &USART_ClockInitStructure);


	/* Baud rate 9600, 8-bit data, One stop bit
	 * No parity, Do both Rx and Tx, No HW flow control
	 */
	usart1_init_struct.USART_BaudRate = 921600;
	usart1_init_struct.USART_WordLength = USART_WordLength_8b;
	usart1_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart1_init_struct.USART_Parity = USART_Parity_No;
	usart1_init_struct.USART_StopBits = USART_StopBits_1;
	usart1_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	/* Configure USART1 */
	USART_Init(USART1, &usart1_init_struct);
	USART_Cmd(USART1, ENABLE);

	for (int i=0 ;i< 1000; i++)
	{

		USART_SendData(USART1, '\n');
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
	}
}

void putChar(char c)
{
	USART_SendData(USART1, c);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
}

void print(const char* str)
{
	for (; *str != '\0'; str++)
		putChar(*str);
}
