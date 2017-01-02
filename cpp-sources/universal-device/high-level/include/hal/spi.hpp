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

#ifndef LAZERTAG_RIFLE_INCLUDE_HAL_SPI_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HAL_SPI_HPP_

#include "hal/io-pins.hpp"
#include <stdint.h>

class ISPIManager
{
public:
	constexpr static uint32_t BaudRatePrescaler4   = 1;
	constexpr static uint32_t BaudRatePrescaler8   = 2;
	constexpr static uint32_t BaudRatePrescaler16  = 3;
	constexpr static uint32_t BaudRatePrescaler32  = 4;
	constexpr static uint32_t BaudRatePrescaler64  = 5;
	constexpr static uint32_t BaudRatePrescaler128 = 6;
	constexpr static uint32_t BaudRatePrescaler256 = 7;

	constexpr static uint8_t SPIPhase1edge = 1;
	constexpr static uint8_t SPIPhase2edge = 2;

	constexpr static uint32_t DefaultTimeout = 100000;
	virtual ~ISPIManager() {}
	virtual void init(uint32_t prescaler, IIOPin* NSSPin, uint8_t SPIPhase = SPIPhase1edge) = 0;

	/// @Todo add overloadings for one byte
	virtual bool Transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout = DefaultTimeout) = 0;
	virtual bool Receive(uint8_t *pData, uint16_t Size, uint32_t Timeout = DefaultTimeout) = 0;
	virtual bool TransmitReceive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout = DefaultTimeout) = 0;


	bool Transmit(uint8_t byte, uint32_t Timeout = DefaultTimeout)
	{
		return Transmit(&byte, 1, Timeout);
	}

	bool Receive(uint8_t byte, uint32_t Timeout = DefaultTimeout)
	{
		return Receive(&byte, 1, Timeout);
	}

	uint8_t TransmitReceive(uint8_t byte, uint32_t Timeout = DefaultTimeout)
	{
		uint8_t result = 0;
		TransmitReceive(&byte, &result, 1, Timeout);
		return result;
	}
};

class ISPIUser
{
public:
	virtual ~ISPIUser() {}
	virtual void configureSPI() = 0;
};

class ISPIsPool
{
public:
	virtual ~ISPIsPool() {}
	/// Get SPI manager. SPI enumerated from 0, so MCU port called "SPI_1" has portNumber == 0
	virtual ISPIManager* getSPI(uint8_t portNumber) = 0;
};

extern ISPIsPool* SPIs;


#endif /* LAZERTAG_RIFLE_INCLUDE_HAL_SPI_HPP_ */
