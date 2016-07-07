/*
 * uart-hw.hpp
 *
 *  Created on: 9 сент. 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_UART_HW_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_UART_HW_HPP_

#include "hal/uart.hpp"
#include "stm32f1xx_hal.h"

class UARTManager : public UARTManagerBase
{
public:
	constexpr static uint16_t rxBufferMaxSize = 200;
	void init(uint8_t portNumber, uint32_t baudrate);
	void transmit(uint8_t* buffer, uint16_t size);
	void transmitSync(uint8_t* buffer, uint16_t size, uint32_t timeout = 10000);
	bool txBusy();
	bool rxBusy();

	void txDoneISR();
	void rxDoneISR(uint8_t* buffer, uint16_t size);
private:
	UART_HandleTypeDef* m_huart = nullptr;
	bool m_rxBusy = false;
	bool m_txBusy = false;
	uint8_t rxBuffer[200];
	uint16_t rxCount = 0;
};

class UARTsFactory : public IUARTsFactory
{
public:
	IUARTManager* create();
};



#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_UART_HW_HPP_ */
