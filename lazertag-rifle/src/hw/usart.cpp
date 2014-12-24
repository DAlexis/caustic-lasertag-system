/*
 * usart.cpp
 *
 *  Created on: 24 дек. 2014 г.
 *      Author: alexey
 */

#include "hw/usart.hpp"

USARTSPool USARTSPoolInstance;
IUSARTSPool* USARTS = &USARTSPoolInstance;

using USART_IRQHandler = std::function<void()>;

USART_IRQHandler usartRX_IRQHandlers[3];

namespace
{
	constexpr uint8_t USART_IO_STATUS_RECEIVING = 1;
	constexpr uint8_t USART_IO_STATUS_TRANSMITTIMG = 2;

	struct USARTHWOptions {
		uint8_t APBx;
		uint32_t RCC_APBnPeriph_USARTn;
		uint32_t RCC_APB2Periph_GPIOx;
		GPIO_TypeDef* GPIOx;
		uint16_t GPIO_Pin_TX;
		uint16_t GPIO_Pin_RX;
		USART_TypeDef* USARTn;
		IRQn_Type USARTn_IRQn;
	};

	constexpr USARTHWOptions usartOptions[3] = {
		// USART #0 (hw USART1)
		{
			.APBx = 2,
			.RCC_APBnPeriph_USARTn = RCC_APB2Periph_USART1,
			.RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA,
			.GPIOx = GPIOA,
			.GPIO_Pin_TX = GPIO_Pin_9,
			.GPIO_Pin_RX = GPIO_Pin_10,
			.USARTn = USART1,
			.USARTn_IRQn = USART1_IRQn,
		},
		// USART #1 (hw USART2)
		{
			.APBx = 1,
			.RCC_APBnPeriph_USARTn = RCC_APB1Periph_USART2,
			.RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA,
			.GPIOx = GPIOA,
			.GPIO_Pin_TX = GPIO_Pin_2,
			.GPIO_Pin_RX = GPIO_Pin_3,
			.USARTn = USART2,
			.USARTn_IRQn = USART2_IRQn,
		},
		// USART #1 (hw USART3)
		{
			.APBx = 1,
			.RCC_APBnPeriph_USARTn = RCC_APB1Periph_USART3,
			.RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB,
			.GPIOx = GPIOB,
			.GPIO_Pin_TX = GPIO_Pin_10,
			.GPIO_Pin_RX = GPIO_Pin_11,
			.USARTn = USART3,
			.USARTn_IRQn = USART3_IRQn,
		}
	};
}

USARTManager::USARTManager(uint8_t portNumber) :
		USARTManagerBase(portNumber),
	input_buffer(nullptr),
	output_buffer(nullptr),
	input_stop_char('\r'),
	output_stop_char('\0'),
	input_cursor(0),
	output_cursor(0),
	input_buffer_size(0),
	io_status(0),
	need_echo(1)
{
}

USARTManager::~USARTManager()
{
}

void USARTManager::init(uint32_t baudrate)
{
	m_baudrate = baudrate;

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;

	USART_StructInit(&USART_InitStructure);

	RCC_APB2PeriphClockCmd(usartOptions[m_portNumber].RCC_APB2Periph_GPIOx | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
	if (usartOptions[m_portNumber].APBx == 1)
		RCC_APB1PeriphClockCmd(usartOptions[m_portNumber].RCC_APBnPeriph_USARTn, ENABLE);
	else if (usartOptions[m_portNumber].APBx == 2)
		RCC_APB2PeriphClockCmd(usartOptions[m_portNumber].RCC_APBnPeriph_USARTn, ENABLE);
	else
		return; // :(

	// USART1 TX on pin GPIO_Pin_9 of port GPIOA
	GPIO_InitStructure.GPIO_Pin = usartOptions[m_portNumber].GPIO_Pin_TX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART1 RX on pin GPIO_Pin_10 of port GPIOA
	GPIO_InitStructure.GPIO_Pin = usartOptions[m_portNumber].GPIO_Pin_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Hm... examples does not contain GPIO_PinAFConfig(...) calls. May be next two lines somehow do it?

	// USART initialize
	USART_ClockStructInit(&USART_ClockInitStructure);
	USART_ClockInit(usartOptions[m_portNumber].USARTn, &USART_ClockInitStructure);

	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(usartOptions[m_portNumber].USARTn, &USART_InitStructure);
	USART_Cmd(usartOptions[m_portNumber].USARTn, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = usartOptions[m_portNumber].USARTn_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // Set priority to lowest
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; // Set subpriority to lowest
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//disable Transmit Data Register empty interrupt
	USART_ITConfig(usartOptions[m_portNumber].USARTn, USART_IT_TXE, DISABLE);
	//enable Receive Data register not empty interrupt
	USART_ITConfig(usartOptions[m_portNumber].USARTn, USART_IT_RXNE, DISABLE);

	usartRX_IRQHandlers[m_portNumber] = std::bind(&USARTManager::RX_IRQHandler, this);
}

void USARTManager::waitForTransmitionEnd()
{
	while(USART_GetFlagStatus(usartOptions[m_portNumber].USARTn, USART_FLAG_TC) == RESET) {}
}

void USARTManager::putChar(uint8_t ch)
{
	USART_SendData(usartOptions[m_portNumber].USARTn, ch);
	waitForTransmitionEnd();
}

int USARTManager::syncWrite(const char* ptr, int len)
{
	waitForTransmitionEnd();
	for (int i = 0; i!=len; i++) {
		putChar( (uint8_t) ptr[i]);
	}
	return len;
}

void USARTManager::asyncRead(char* buffer, char stop_char, int maxlen, USARTReadingDoneCallback callback, uint8_t _need_echo)
{
	if (io_status & USART_IO_STATUS_RECEIVING)
		return;
	input_buffer = buffer;
	input_cursor = 0;
	input_buffer_size = maxlen;
	m_readingDoneCB = callback;
	io_status |= USART_IO_STATUS_RECEIVING;
	need_echo = _need_echo;
	input_stop_char = stop_char;
	USART_ITConfig(usartOptions[m_portNumber].USARTn, USART_IT_RXNE, ENABLE);
}

void USARTManager::RX_IRQHandler()
{
	char data_byte = (char)USART_ReceiveData(USART1);
	input_buffer[input_cursor] = data_byte;

	if (need_echo)
	{
		if (data_byte == '\r')
			USART_SendData(USART1, '\n');
		else
			USART_SendData(USART1, (uint16_t)data_byte);
		waitForTransmitionEnd();
	}

	if (data_byte == input_stop_char
		|| data_byte == '\0'
		|| input_cursor == input_buffer_size-2)
	{
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
		if (input_cursor == input_buffer_size-2)
			input_cursor++;
		input_buffer[input_cursor] = '\0';
		io_status &= ~USART_IO_STATUS_RECEIVING;
		if (m_readingDoneCB) {
			m_readingDoneCB(input_buffer, input_cursor);
		}
	} else
		input_cursor++;
}


///////////////////////
// USARTSPool
USARTSPool::USARTSPool()
{
	for (int i=0; i<MAX_USARTS_COUNT; i++)
		m_usart[i] = nullptr;
}


IUSARTManager* USARTSPool::getUSART(uint8_t portNumber)
{
	if (!m_usart[portNumber])
		m_usart[portNumber] = new USARTManager(portNumber);

	return m_usart[portNumber];

}

USARTSPool::~USARTSPool()
{
	// For Chuck Norris MCU
	for (int i=0; i<MAX_USARTS_COUNT; i++)
		if (m_usart[i]) delete m_usart[i];
}

///////////////////////
// IRQs

extern "C" {
    void USART1_IRQHandler(void)
    {
        // If we received a byte
        if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
        {
            USART_ClearITPendingBit(USART1, USART_IT_RXNE);
            usartRX_IRQHandlers[0]();
        }
    }

    void USART2_IRQHandler(void)
	{
		// If we received a byte
		if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
		{
			USART_ClearITPendingBit(USART2, USART_IT_RXNE);
			usartRX_IRQHandlers[1]();
		}
	}

    void USART3_IRQHandler(void)
	{
		// If we received a byte
		if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
		{
			USART_ClearITPendingBit(USART3, USART_IT_RXNE);
			usartRX_IRQHandlers[2]();
		}
	}
}
