/*
 * usart-manager.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_USART_MANAGER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_USART_MANAGER_HPP_

#include "stm32f10x.h"
#include "managers-pool.hpp"

#define USARTS_COUNT        4

typedef int (*UsartReadingDoneCallback)(char* buffer, int bytesCount);
typedef void (*UsartWritingDoneCallback)(void);

class USARTManagersPool;

class USARTManager
{
    friend class USARTManagersPool;
public:
    void putChar(uint8_t ch);
    int write(const char* ptr, int len);
    void async_read(char* buffer, char stop_char, int maxlen, UsartReadingDoneCallback callback, uint8_t need_echo);
    USARTManager(USART_TypeDef* usart, uint32_t baudrate);
    void rxIRQHandler();
private:

    class USARTioContext
    {
    public:
        USARTioContext();

        char* input_buffer;
        char* output_buffer;
        char input_stop_char;
        char output_stop_char;
        int16_t input_cursor;
        int16_t output_cursor;
        int16_t input_buffer_size;
        UsartReadingDoneCallback reading_done_callback;
        UsartWritingDoneCallback writing_done_callback;
        uint8_t io_status;
        uint8_t need_echo;
    };

    USARTManager(const USARTManager&) = delete;
    void waitForTransmitionEnd();

    USARTioContext m_ioContext;

    USART_TypeDef* m_pUsart;
    uint32_t m_baudrate;
};

typedef ManagersPoolBase<USARTManager, USARTManagersPool, USARTS_COUNT, USART_TypeDef*, uint32_t> USARTManagersPoolBase;

class USARTManagersPool : public USARTManagersPoolBase
{
private:
    int deviceToIndex(USART_TypeDef* device);
};

#endif /* LAZERTAG_RIFLE_INCLUDE_USART_MANAGER_HPP_ */
