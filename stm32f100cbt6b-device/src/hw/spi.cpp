/*
 * spi.cpp
 *
 *  Created on: 16 янв. 2015 г.
 *      Author: alexey
 */

#include "hw/spi.hpp"
#include "stm32f10x.h"

SPIsPool SPIsPoolInstance;
ISPIsPool* SPIs = &SPIsPoolInstance;

SPIManager::SPIManager(uint8_t portNumber) :
	m_portNumber(portNumber)
{
}


void SPIManager::init(uint32_t prescaler)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	SPI_InitTypeDef SPI_InitStruct;
	SPI_StructInit(&SPI_InitStruct);
	uint32_t realPrescaler = SPI_BaudRatePrescaler_256;
	switch(prescaler)
	{
	case BaudRatePrescaler2:   realPrescaler = SPI_BaudRatePrescaler_2; break;
	case BaudRatePrescaler4:   realPrescaler = SPI_BaudRatePrescaler_4; break;
	case BaudRatePrescaler8:   realPrescaler = SPI_BaudRatePrescaler_8; break;
	case BaudRatePrescaler16:  realPrescaler = SPI_BaudRatePrescaler_16; break;
	case BaudRatePrescaler32:  realPrescaler = SPI_BaudRatePrescaler_32; break;
	case BaudRatePrescaler64:  realPrescaler = SPI_BaudRatePrescaler_64; break;
	case BaudRatePrescaler128: realPrescaler = SPI_BaudRatePrescaler_128; break;
	default:
	case BaudRatePrescaler256: realPrescaler = SPI_BaudRatePrescaler_256; break;
	}

	switch(m_portNumber)
	{
		case 0:
			m_SPI = SPI1;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
			/* configure pins used by SPI1
			 * PA5 = SCK
			 * PA6 = MISO
			 * PA7 = MOSI
			 */
			// Output
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_5;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStruct);

			// Input
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStruct);

			SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
			SPI_InitStruct.SPI_Mode = SPI_Mode_Master;     // transmit in master mode, NSS pin has to be always high
			SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
			SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
			SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
			// For some reason (I dont know!) SPI_NSSInternalSoft_Set cause MC freeze
			SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;// | SPI_NSSInternalSoft_Set; // set the NSS management to internal and pull internal NSS high
			SPI_InitStruct.SPI_BaudRatePrescaler = realPrescaler; // SPI frequency is APB2 frequency / 4
			SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
			SPI_Init(m_SPI, &SPI_InitStruct);

			SPI_Cmd(m_SPI, ENABLE); // enable SPI1
			return;
		case 1:
			m_SPI = SPI2;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
			/* configure pins used by SPI2
			 * PB13 = SCK
			 * PB14 = MISO
			 * PB15 = MOSI
			 */
			// So output
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOB, &GPIO_InitStruct);

			// So input
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOB, &GPIO_InitStruct);

			SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
			SPI_InitStruct.SPI_Mode = SPI_Mode_Master;     // transmit in master mode, NSS pin has to be always high
			SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
			SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
			SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at second edge
			// For some reason (I dont know!) SPI_NSSInternalSoft_Set cause MC freeze
			SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;// | SPI_NSSInternalSoft_Set; // set the NSS management to internal and pull internal NSS high
			SPI_InitStruct.SPI_BaudRatePrescaler = realPrescaler; // SPI frequency is APB1 frequency / prescaler
			SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
			SPI_Init(m_SPI, &SPI_InitStruct);
			SPI_Cmd(m_SPI, ENABLE); // enable SPI2
			return;
	}
}

uint8_t SPIManager::sendByte(unsigned char data)
{
    uint8_t tmp;
	m_SPI->DR = data; // write data to be transmitted to the SPI data register
	while( !(m_SPI->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( !(m_SPI->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
	while( m_SPI->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
	tmp = m_SPI->DR; // return received data from SPI data register
	return tmp;
}

uint8_t SPIManager::receiveByte()
{
	m_SPI->DR = 0xFF; // write data to be transmitted to the SPI data register
	while( !(m_SPI->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( !(m_SPI->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
	while( m_SPI->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
	return m_SPI->DR; // return received data from SPI data register
}

void SPIManager::send(uint8_t* data, unsigned int length)
{
	while (length--)
	{
		sendByte(*data);
		data++;
	}
}

void SPIManager::receive(uint8_t* data, unsigned int length)
{
	while (length--)
	{
		*data = receiveByte();
		data++;
	}
}

void SPIManager::transmit(uint8_t* txbuf, uint8_t* rxbuf, unsigned int len)
{
	while (len--)
	{
		m_SPI->DR = *txbuf; // write data to be transmitted to the SPI data register
		while( !(m_SPI->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
		while( !(m_SPI->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
		while( m_SPI->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
		*rxbuf = m_SPI->DR; // return received data from SPI data register
		txbuf++;
		rxbuf++;
	}
}

///////////////////////
// SPIsPool
SPIsPool::SPIsPool()
{
	for (int i=0; i<SPIS_COUNT; i++)
		m_SPIs[i] = nullptr;
}

ISPIManager* SPIsPool::getSPI(uint8_t portNumber)
{
	if (!m_SPIs[portNumber])
		m_SPIs[portNumber] = new SPIManager(portNumber);
	return m_SPIs[portNumber];
}
