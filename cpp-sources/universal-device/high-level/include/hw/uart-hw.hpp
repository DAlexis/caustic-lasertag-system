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


#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_UART_HW_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_UART_HW_HPP_

#include "hal/uart.hpp"
#include "stm32f1xx_hal.h"

class UARTManager : public UARTManagerBase
{
public:
	constexpr static uint16_t rxBufferMaxSize = 200;

	UARTManager(uint8_t portNumber);
	void init(uint32_t baudrate) override;
	void transmit(uint8_t* buffer, uint16_t size) override;
	void transmitSync(uint8_t* buffer, uint16_t size, uint32_t timeout = 10000) override;
	bool txBusy() override;
	bool rxBusy() override;

	void txDoneISR();
	void rxDoneISR(uint8_t* buffer, uint16_t size);
private:
	USART_TypeDef* m_instance = nullptr;
	UART_HandleTypeDef* m_huart = nullptr;
	bool m_rxBusy = false;
	bool m_txBusy = false;
	uint8_t rxBuffer[200];
	uint16_t rxCount = 0;
};

class UARTSPool : public IUARTSPool
{
public:
	IUARTManager* get(uint8_t portNumber);
	UARTManager* getByHandle(UART_HandleTypeDef* handle);

private:
	UARTManager* m_uarts[3] = {nullptr, nullptr, nullptr};
};


#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_UART_HW_HPP_ */
