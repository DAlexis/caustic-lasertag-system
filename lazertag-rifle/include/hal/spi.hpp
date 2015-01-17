/*
 * spi.hpp
 *
 *  Created on: 16 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HAL_SPI_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HAL_SPI_HPP_

#include <stdint.h>

class ISPIManager
{
public:
	virtual ~ISPIManager() {}
	virtual void init(uint32_t prescaler) = 0;
	virtual uint8_t sendByte(unsigned char data) = 0;
	virtual uint8_t receiveByte() = 0;
	virtual void send(uint8_t* data, unsigned int length) = 0;
	virtual void receive(uint8_t* data, unsigned int length) = 0;
	virtual void transmit(uint8_t* txbuf, uint8_t* rxbuf, unsigned int len) = 0;
};

class ISPIsPool
{
public:
	virtual ~ISPIsPool() {}
	virtual ISPIManager* getSPI(uint8_t portNumber) = 0;
};

extern ISPIsPool* SPIs;


#endif /* LAZERTAG_RIFLE_INCLUDE_HAL_SPI_HPP_ */
