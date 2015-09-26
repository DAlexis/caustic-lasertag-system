/*
 * uart.hpp
 *
 *  Created on: 9 сент. 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_HAL_UART_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_HAL_UART_HPP_

#include "core/os-wrappers.hpp"
#include <functional>

class IUARTManager
{
public:
	virtual ~IUARTManager() {}
	using TXDoneCallback = std::function<void(void)>;
	using RXDoneCallback = std::function<void(uint8_t*, uint16_t)>;

	virtual void init(uint8_t portNumber, uint32_t baudrate) = 0;
	virtual void setTXDoneCallback(TXDoneCallback callback) = 0;
	virtual void setRXDoneCallback(RXDoneCallback callback) = 0;
	virtual void setStopChar(uint8_t stopChar) = 0;
	virtual void enableStopChar(bool enabled) = 0;
	virtual void setBlockSize(uint16_t blockSize) = 0;
	virtual void setRXWorker(Worker* worker) = 0;


	virtual void transmit(uint8_t* buffer, uint16_t size = 0) = 0;
	virtual bool txBusy() = 0;
	virtual bool rxBusy() = 0;
};


class UARTManagerBase : public IUARTManager
{
public:
	void setTXDoneCallback(TXDoneCallback callback) { m_txCallback = callback; }
	void setRXDoneCallback(RXDoneCallback callback) {m_rxCallback = callback; }
	void setStopChar(uint8_t stopChar) { m_stopChar = stopChar; }
	void enableStopChar(bool enabled) { m_stopCharEnabled = enabled; }
	void setRXWorker(Worker* worker) { m_worker = worker; }
	void setBlockSize(uint16_t blockSize) { m_blockSize = blockSize; }

protected:
	TXDoneCallback m_txCallback = nullptr;
	RXDoneCallback m_rxCallback = nullptr;
	uint8_t m_stopChar = '\n';
	Worker* m_worker = nullptr;
	uint16_t m_blockSize = 1;
	bool m_stopCharEnabled = false;
};

class IUARTsFactory
{
public:
	virtual ~IUARTsFactory() {}
	virtual IUARTManager* create() = 0;
};

extern IUARTsFactory* UARTSFactory;

#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_HAL_UART_HPP_ */
