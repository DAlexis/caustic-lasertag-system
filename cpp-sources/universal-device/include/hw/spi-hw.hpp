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
	uint8_t sendByte(uint8_t byte);
	uint8_t receiveByte();

	SPI_HandleTypeDef* m_hspi;
	StagerStub m_stager{"SPIManager"};
	bool m_operationDone = false;

	uint8_t m_portNumber = 1;
	IIOPin* m_NSSPin = nullptr;

	uint32_t m_configHash = 0;
};

class SPIsPool : public ISPIsPool
{
public:
	~SPIsPool() {}
	/// Get SPI manager. SPI enumerated from 0, so MCU port called "SPI_1" has portNumber == 0
	ISPIManager* getSPI(uint8_t portNumber);
private:
	ISPIManager* m_spis[3] = {nullptr, nullptr, nullptr};
	/// @Todo add spis pool here
};




#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_SPI_HW_HPP_ */
