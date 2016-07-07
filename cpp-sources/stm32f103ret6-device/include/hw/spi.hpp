/*
 * spi.hpp
 *
 *  Created on: 16 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HW_SPI_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HW_SPI_HPP_

#include "hal/spi.hpp"
#include "stm32f10x.h"

#define SPIS_COUNT         3

class SPIManager : public ISPIManager
{
public:
	SPIManager(uint8_t portNumber);
	void init(uint32_t prescaler);
	uint8_t sendByte(unsigned char data);
	uint8_t receiveByte();
	void send(uint8_t* data, unsigned int length);
	void receive(uint8_t* data, unsigned int length);
	void transmit(uint8_t* txbuf, uint8_t* rxbuf, unsigned int len);

private:
	uint8_t m_portNumber;
	SPI_TypeDef* m_SPI = nullptr;
};

class SPIsPool : public ISPIsPool
{
public:
	SPIsPool();
	ISPIManager* getSPI(uint8_t portNumber);

private:
	ISPIManager* m_SPIs[SPIS_COUNT];
};

#endif /* LAZERTAG_RIFLE_INCLUDE_HW_SPI_HPP_ */
