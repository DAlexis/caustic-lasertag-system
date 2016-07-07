/*
 * spi.hpp
 *
 *  Created on: 16 янв. 2015 г.
 *      Author: alexey
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
