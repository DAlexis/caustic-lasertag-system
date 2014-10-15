/*
 * spi.cpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#include "spi.hpp"

template<>
SPIManagersPool* SPIManagersPoolBase::m_self = nullptr;

SPIManager::SPIManager(SPI_TypeDef* SPIx) :
    m_SPI(SPIx)
{
}

void SPIManager::init(uint32_t prescaler)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef SPI_InitStruct;

    // enable clock for used IO pins
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* configure pins used by SPI1
     * PA5 = SCK
     * PA6 = MISO
     * PA7 = MOSI
     */
    // So output
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // So input
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // enable peripheral clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    /* configure SPI1 in Mode 0
     * CPOL = 0 --> clock is low when idle
     * CPHA = 0 --> data is sampled at the first edge
     */

    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;     // transmit in master mode, NSS pin has to be always high
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
    // For some reason (I dont know!) SPI_NSSInternalSoft_Set cause MC freeze
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;// | SPI_NSSInternalSoft_Set; // set the NSS management to internal and pull internal NSS high
    SPI_InitStruct.SPI_BaudRatePrescaler = prescaler; // SPI frequency is APB2 frequency / 4
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
    SPI_Init(m_SPI, &SPI_InitStruct);

    SPI_Cmd(m_SPI, ENABLE); // enable SPI1
}

void SPIManager::send_single(unsigned char data)
{
    unsigned char tmp;
    m_SPI->DR = data; // write data to be transmitted to the SPI data register
    while( !(m_SPI->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
    while( !(m_SPI->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
    while( m_SPI->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
    tmp = m_SPI->DR; // return received data from SPI data register
}

unsigned char SPIManager::receive_single()
{
    m_SPI->DR = 0xFF; // write data to be transmitted to the SPI data register
    while( !(m_SPI->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
    while( !(m_SPI->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
    while( m_SPI->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
    return m_SPI->DR; // return received data from SPI data register
}

void SPIManager::send(unsigned char* data, unsigned int length)
{
    while (length--)
    {
        send_single(*data);
        data++;
    }
}

void SPIManager::receive(unsigned char* data, unsigned int length)
{
    while (length--)
    {
        *data = receive_single();
        data++;
    }
}

void SPIManager::transmit(unsigned char* txbuf, unsigned char* rxbuf, unsigned int len)
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

int SPIManagersPool::deviceToIndex(SPI_TypeDef* device)
{
    if (device == SPI1)
        return 0;
    if (device == SPI2)
        return 1;
    return 0;
}
