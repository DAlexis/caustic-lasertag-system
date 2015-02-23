/*
 * usart.hpp
 *
 *  Created on: 24 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HW_USART_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HW_USART_HPP_

#include "hal/usart.hpp"
#include "stm32f10x.h"

class USARTManager : public USARTManagerBase
{
public:
	USARTManager(uint8_t portNumber);
	~USARTManager();
	void init(uint32_t baudrate);
	void putChar(uint8_t ch);
	int syncWrite(const char* ptr, int len);
	void asyncRead(char* buffer, char stop_char, int maxlen, USARTReadingDoneCallback callback, uint8_t need_echo);

private:
	void RX_IRQHandler();
	USARTManager(const USARTManager&) = delete;
	void waitForTransmitionEnd();

	char* input_buffer;
	char* output_buffer;
	char input_stop_char;
	char output_stop_char;
	int16_t input_cursor;
	int16_t output_cursor;
	int16_t input_buffer_size;
	uint8_t io_status;
	uint8_t need_echo;
	uint32_t m_baudrate;
};

#define MAX_USARTS_COUNT	3

class USARTSPool : public IUSARTSPool
{
public:
	USARTSPool();
	~USARTSPool();
	IUSARTManager* getUSART(uint8_t portNumber);

private:
	IUSARTManager* m_usart[MAX_USARTS_COUNT];
};

#endif /* LAZERTAG_RIFLE_INCLUDE_HW_USART_HPP_ */
