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

//#define USE_STDPERIPH_SPI

#ifdef USE_STDPERIPH_SPI
	#include "stm32f10x.h"
#else
	#include "stm32f1xx_hal.h"
#endif


class SPIManager : public ISPIManager
{
public:
	SPIManager(uint8_t SPIindex);
	~SPIManager() {}

	void init(uint32_t prescaler, IIOPin* NSSPin);
	bool Transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout = 0xFFFFFFFF);
	bool Receive(uint8_t *pData, uint16_t Size, uint32_t Timeout = 0xFFFFFFFF);
	bool TransmitReceive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout = 0xFFFFFFFF);
private:
#ifndef USE_STDPERIPH_SPI
	SPI_HandleTypeDef m_hspi;
#endif
	uint8_t m_portNumber = 1;
	IIOPin* m_NSSPin = nullptr;
	SPI_TypeDef* m_SPI = nullptr;
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
