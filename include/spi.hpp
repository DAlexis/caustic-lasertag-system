/*
 * spi.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_SPI_HPP_
#define LAZERTAG_RIFLE_INCLUDE_SPI_HPP_

#include "stm32f10x.h"

class SPIManager
{
public:
    SPIManager(SPI_TypeDef* SPIx);
    void init(uint32_t prescaler);
    void send_single(unsigned char data);
    unsigned char receive_single();
    void send(unsigned char* data, unsigned int length);
    void receive(unsigned char* data, unsigned int length);
    void transmit(unsigned char* txbuf, unsigned char* rxbuf, unsigned int len);
private:
    SPI_TypeDef* m_SPI;
};

extern SPIManager SPI1Manager;

#endif /* LAZERTAG_RIFLE_INCLUDE_SPI_HPP_ */
