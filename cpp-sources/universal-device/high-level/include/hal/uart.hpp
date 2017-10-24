/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_HAL_UART_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_HAL_UART_HPP_

#include <functional>

class Worker;

class IUARTManager
{
public:
	virtual ~IUARTManager() {}
	using TXDoneCallback = std::function<void(void)>;
	using RXDoneCallback = std::function<void(uint8_t*, uint16_t)>;

	virtual void init(uint32_t baudrate, bool useHalfDuplex = false) = 0;
	virtual void setTXDoneCallback(TXDoneCallback callback) = 0;
	virtual void setRXDoneCallback(RXDoneCallback callback) = 0;
	virtual void setStopChar(uint8_t stopChar) = 0;
	virtual void enableStopChar(bool enabled) = 0;
	virtual void setBlockSize(uint16_t blockSize) = 0;
	virtual void setRXWorker(Worker* worker) = 0;


	virtual void transmit(uint8_t* buffer, uint16_t size) = 0;
	virtual void transmitSync(uint8_t* buffer, uint16_t size, uint32_t timeout = 100000) = 0;
	virtual bool txBusy() = 0;
	virtual bool rxBusy() = 0;
};


class UARTManagerBase : public IUARTManager
{
public:
	void setTXDoneCallback(TXDoneCallback callback) override { m_txCallback = callback; }
	void setRXDoneCallback(RXDoneCallback callback) override {m_rxCallback = callback; }
	void setStopChar(uint8_t stopChar) override { m_stopChar = stopChar; }
	void enableStopChar(bool enabled) override { m_stopCharEnabled = enabled; }
	void setBlockSize(uint16_t blockSize) override { m_blockSize = blockSize; }
	void setRXWorker(Worker* worker) override { m_worker = worker; }

protected:
	TXDoneCallback m_txCallback = nullptr;
	RXDoneCallback m_rxCallback = nullptr;
	uint8_t m_stopChar = '\n';
	Worker* m_worker = nullptr;
	uint16_t m_blockSize = 1;
	bool m_stopCharEnabled = false;
};

class IUARTSPool
{
public:
	constexpr static uint8_t UART1 = 0;
	constexpr static uint8_t UART2 = 1;
	constexpr static uint8_t UART3 = 2;

	virtual ~IUARTSPool() {}
	virtual IUARTManager* get(uint8_t portNumber) = 0;
};

extern IUARTSPool* UARTs;



#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_HAL_UART_HPP_ */
