/*
 * spi.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_SPI_HPP_
#define LAZERTAG_RIFLE_INCLUDE_SPI_HPP_

class SPIManagersPool;

class SPIManager
{
    friend class SPIManagersPool;
public:

private:

};
/*
class USARTManagersPool
{
public:
    static USARTManagersPool& getInstance();
    void createUsart(USART_TypeDef* usart, uint32_t baudrate);
    USARTManager& getUsart(USART_TypeDef* usart);
    USARTManagersPool();
private:

    int getUsartIndex(USART_TypeDef* usart);
    USARTManager* m_pUsarts[USARTS_COUNT];
    static USARTManagersPool* m_self;
};
*/




#endif /* LAZERTAG_RIFLE_INCLUDE_SPI_HPP_ */
