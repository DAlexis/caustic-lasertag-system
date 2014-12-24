/*
 * usart.hpp
 *
 *  Created on: 24 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HAL_USART_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HAL_USART_HPP_

#include <functional>
//#include <stdint.h>

using USARTReadingDoneCallback = std::function<void(char*, int)>;
using USARTWritingDoneCallback = std::function<void()>;

class IUSARTManager
{
public:
	virtual ~IUSARTManager() {}

	virtual void init(uint32_t baudrate) = 0;
	virtual void putChar(uint8_t ch) = 0;
	virtual int syncWrite(const char* ptr, int len) = 0;
	virtual void asyncRead(char* buffer, char stop_char, int maxlen, USARTReadingDoneCallback callback, uint8_t need_echo) = 0;
};

class IUSARTSPool
{
public:
	virtual IUSARTManager* getUSART(uint8_t portNumber) = 0;
};

extern IUSARTSPool* USARTS;

class USARTManagerBase : public IUSARTManager
{
protected:
	USARTManagerBase(uint8_t portNumber) : m_portNumber(portNumber) {}

	USARTReadingDoneCallback m_readingDoneCB;
	USARTWritingDoneCallback m_writingDoneCB;

	uint8_t m_portNumber;
};



#endif /* LAZERTAG_RIFLE_INCLUDE_HAL_USART_HPP_ */
