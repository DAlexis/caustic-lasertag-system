/*
 * spi-hw.hpp
 *
 *  Created on: 24 июня 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_SPI_HW_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_SPI_HW_HPP_

#include "hal/spi.hpp"
#include "hal/io-pins.hpp"

#include "stm32f1xx_hal.h"
#include "core/diagnostic.hpp"



class SPIManager : public ISPIManager
{
public:
	SPIManager(uint8_t SPIindex);
	~SPIManager() {}

	void init(uint32_t prescaler, IIOPin* NSSPin, uint8_t SPIPhase = SPIPhase1edge);
	bool Transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout = defaultTimeout);
	bool Receive(uint8_t *pData, uint16_t Size, uint32_t Timeout = defaultTimeout);
	bool TransmitReceive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout = defaultTimeout);

	void operationDone_ISR();

private:
	constexpr static uint32_t defaultTimeout = 100000;
	bool waitForISR(uint32_t timeout = 0);

	SPI_HandleTypeDef* m_hspi;
	StagerStub m_stager{"SPIManager"};
	bool m_operationDone = false;

	uint8_t m_portNumber = 1;
	IIOPin* m_NSSPin = nullptr;

	uint8_t sendByte(uint8_t byte);
	uint8_t receiveByte();
};

class SPIsPool : public ISPIsPool
{
public:
	~SPIsPool() {}
	/// Get SPI manager. SPI enumerated from 0, so MCU port called "SPI_1" has portNumber == 0
	ISPIManager* getSPI(uint8_t portNumber);
private:
	/// @Todo add spis pool here
};




#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_SPI_HW_HPP_ */
