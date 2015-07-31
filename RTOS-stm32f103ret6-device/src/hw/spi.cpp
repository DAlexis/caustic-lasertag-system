/*
 * spi.cpp
 *
 *  Created on: 24 июня 2015 г.
 *      Author: alexey
 */

#include "hw/spi-hw.hpp"

#include "utils/memory.hpp"
#include "core/string-utils.hpp"

#ifndef USE_STDPERIPH_SPI
	#include "core/logging.hpp"
#endif

static SPIsPool pool;

ISPIsPool* SPIs = &pool;

#ifdef USE_STDPERIPH_SPI

SPIManager::SPIManager(uint8_t SPIindex) :
	m_portNumber(SPIindex)
{
}

void SPIManager::init(uint32_t prescaler, IIOPin* NSSPin)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	SPI_InitTypeDef SPI_InitStruct;
	SPI_StructInit(&SPI_InitStruct);
	m_NSSPin = NSSPin;
	m_NSSPin->switchToOutput();
	m_NSSPin->set();
	uint32_t realPrescaler = SPI_BaudRatePrescaler_256;
	switch(prescaler)
	{
	case BaudRatePrescaler4:   realPrescaler = SPI_BaudRatePrescaler_4; break;
	case BaudRatePrescaler8:   realPrescaler = SPI_BaudRatePrescaler_8; break;
	case BaudRatePrescaler16:  realPrescaler = SPI_BaudRatePrescaler_16; break;
	case BaudRatePrescaler32:  realPrescaler = SPI_BaudRatePrescaler_32; break;
	case BaudRatePrescaler64:  realPrescaler = SPI_BaudRatePrescaler_64; break;
	case BaudRatePrescaler128: realPrescaler = SPI_BaudRatePrescaler_128; break;
	default:
	case BaudRatePrescaler256: realPrescaler = SPI_BaudRatePrescaler_256; break;
	}
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;     // transmit in master mode, NSS pin has to be always high
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft; // set the NSS management to internal and pull internal NSS high
	SPI_InitStruct.SPI_BaudRatePrescaler = realPrescaler; // SPI frequency is APB2 frequency / 4
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first

	switch(m_portNumber)
	{
		case 1:
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
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStruct);
		case 2:
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
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOB, &GPIO_InitStruct);
	}

	SPI_Init(m_SPI, &SPI_InitStruct);
	SPI_Cmd(m_SPI, ENABLE); // enable SPI
}

uint8_t SPIManager::sendByte(uint8_t data)
{
	//info << "TX:";
	//printf("TX: %x\n", data);
    uint8_t tmp;
	m_SPI->DR = data; // write data to be transmitted to the SPI data register
	while( !(m_SPI->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( !(m_SPI->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
	while( m_SPI->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
	tmp = m_SPI->DR; // return received data from SPI data register
	//info << "RXb:";
	//printf("RX: %x\n", tmp);
	return tmp;
}

uint8_t SPIManager::receiveByte()
{
	m_SPI->DR = 0xFF; // write data to be transmitted to the SPI data register
	while( !(m_SPI->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( !(m_SPI->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
	while( m_SPI->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
	//info << "RX:";
	//printf("RX %x\n", m_SPI->DR);
	return m_SPI->DR; // return received data from SPI data register
}

bool SPIManager::Transmit(uint8_t* data, uint16_t length, uint32_t Timeout)
{
	while (length--)
	{
		sendByte(*data);
		data++;
	}
	return true;
}

bool SPIManager::Receive(uint8_t* data, uint16_t length, uint32_t Timeout)
{

	while (length--)
	{
		*data = receiveByte();
		data++;
	}
	return true;
}

bool SPIManager::TransmitReceive(uint8_t* txbuf, uint8_t* rxbuf, uint16_t len, uint32_t Timeout)
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
	return true;
}



#else
SPIManager::SPIManager(uint8_t SPIindex)
{
	zerify(m_hspi);
	switch(SPIindex)
	{
	case 1: m_hspi.Instance = SPI1; break;
	case 2: m_hspi.Instance = SPI2; break;
	case 3: m_hspi.Instance = SPI3; break;
	default:
		error << "Invalid SPI number";
		return;
	}

}

void SPIManager::init(uint32_t prescaler, IIOPin* NSSPin)
{
	uint32_t baudratePrescaler;

	switch(prescaler)
	{
	case BaudRatePrescaler4: baudratePrescaler = SPI_BAUDRATEPRESCALER_4; break;
	case BaudRatePrescaler8: baudratePrescaler = SPI_BAUDRATEPRESCALER_8; break;
	case BaudRatePrescaler16: baudratePrescaler = SPI_BAUDRATEPRESCALER_16; break;
	case BaudRatePrescaler32: baudratePrescaler = SPI_BAUDRATEPRESCALER_32; break;
	case BaudRatePrescaler64: baudratePrescaler = SPI_BAUDRATEPRESCALER_64; break;
	case BaudRatePrescaler128: baudratePrescaler = SPI_BAUDRATEPRESCALER_128; break;
	case BaudRatePrescaler256: baudratePrescaler = SPI_BAUDRATEPRESCALER_256; break;
	default:
		error << "Invalid SPI prescaler";
		return;
	}

	m_hspi.Init.Mode = SPI_MODE_MASTER;
	m_hspi.Init.Direction = SPI_DIRECTION_2LINES;
	m_hspi.Init.DataSize = SPI_DATASIZE_8BIT;
	m_hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
	m_hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
	m_hspi.Init.NSS = SPI_NSS_SOFT;
	m_hspi.Init.BaudRatePrescaler = baudratePrescaler;
	m_hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	m_hspi.Init.TIMode = SPI_TIMODE_DISABLED;
	m_hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	m_hspi.Init.CRCPolynomial = 7;
	HAL_SPI_Init(&m_hspi);

	m_NSSPin = NSSPin;
	m_NSSPin->switchToOutput();
	m_NSSPin->reset();
}

bool SPIManager::Transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	m_NSSPin->reset();
	HAL_StatusTypeDef res = HAL_SPI_Transmit(&m_hspi, pData, Size, Timeout);
	m_NSSPin->set();
	if (res == HAL_OK)
		return true;
	else {
		error << "SPI failed:" << res;
		return false;
	}
}

bool SPIManager::Receive(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
//	uint8_t buff[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	HAL_StatusTypeDef res = HAL_SPI_Receive(&m_hspi, pData, Size, Timeout);
	//HAL_StatusTypeDef res = HAL_SPI_TransmitReceive(&m_hspi, buff, pData, Size, Timeout);
	if (res == HAL_OK)
		return true;
	else {
		error << "SPI failed:" << res;
		return false;
	}
	return true;
}

bool SPIManager::TransmitReceive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
	HAL_StatusTypeDef res = HAL_SPI_TransmitReceive(&m_hspi, pTxData, pRxData, Size, Timeout);
	if (res == HAL_OK)
		return true;
	else {
		error << "SPI failed:" << res;
		return false;
	}
}

#endif

ISPIManager* SPIsPool::getSPI(uint8_t portNumber)
{
	return new SPIManager(portNumber);
}

