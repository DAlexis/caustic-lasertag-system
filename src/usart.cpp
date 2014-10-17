/*
 * usart-manager.cpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#include "usart.hpp"
#include "write-adaptor.h"
#include <string.h>
#include <new>
#include <stdio.h>
#include <stdlib.h>

#define USART_IO_STATUS_RECEIVING       (uint8_t) 1
#define USART_IO_STATUS_TRANSMITTIMG    (uint8_t) 2

USARTManager USART1Manager(USART1);

USARTManager::USARTManager(USART_TypeDef* usart) :

    input_buffer(nullptr),
    output_buffer(nullptr),
    input_stop_char('\r'),
    output_stop_char('\0'),
    input_cursor(0),
    output_cursor(0),
    input_buffer_size(0),
    reading_done_callback(nullptr),
    writing_done_callback(nullptr),
    io_status(0),
    need_echo(1),
    m_pUsart(usart)
{
}

void USARTManager::init(uint32_t baudrate)
{
    m_baudrate = baudrate;

    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_ClockInitTypeDef USART_ClockInitStructure;

    USART_StructInit(&USART_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
    // USART1 TX on pin GPIO_Pin_9 of port GPIOA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART1 RX on pin GPIO_Pin_10 of port GPIOA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Hm... examples does not contain GPIO_PinAFConfig(...) calls. May be next two lines somehow do it?

    // USART initialize
    USART_ClockStructInit(&USART_ClockInitStructure);
    USART_ClockInit(USART1, &USART_ClockInitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // Set priority to lowest
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; // Set subpriority to lowest
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //disable Transmit Data Register empty interrupt
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    //enable Receive Data register not empty interrupt
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
}

void USARTManager::waitForTransmitionEnd()
{
    while(USART_GetFlagStatus(m_pUsart, USART_FLAG_TC) == RESET) {}
}

void USARTManager::putChar(uint8_t ch)
{
    USART_SendData(USART1, (uint8_t) ch);
    waitForTransmitionEnd();
}

int USARTManager::write(const char* ptr, int len)
{
    waitForTransmitionEnd();
    for (int i = 0; i!=len; i++) {
        putChar( (uint8_t) ptr[i]);
    }
    return len;
}

void USARTManager::async_read(char* buffer, char stop_char, int maxlen, UsartReadingDoneCallback callback, uint8_t _need_echo)
{
    if (io_status & USART_IO_STATUS_RECEIVING)
        return;
    input_buffer = buffer;
    input_cursor = 0;
    input_buffer_size = maxlen;
    reading_done_callback = callback;
    io_status |= USART_IO_STATUS_RECEIVING;
    need_echo = _need_echo;
    input_stop_char = stop_char;
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void USARTManager::rxIRQHandler()
{
    char data_byte = (char)USART_ReceiveData(USART1);
    input_buffer[input_cursor] = data_byte;

    if (need_echo)
    {
        if (data_byte == '\r')
            USART_SendData(USART1, '\n');
        else
            USART_SendData(USART1, (uint16_t)data_byte);
        waitForTransmitionEnd();
    }

    if (data_byte == input_stop_char
        || data_byte == '\0'
        || input_cursor == input_buffer_size-2)
    {
        USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
        if (input_cursor == input_buffer_size-2)
            input_cursor++;
        input_buffer[input_cursor] = '\0';
        io_status &= ~USART_IO_STATUS_RECEIVING;
        if (reading_done_callback) {
            (*reading_done_callback)(input_buffer, input_cursor);

        }
    } else
        input_cursor++;
}

extern "C" {
    void USART1_IRQHandler(void)
    {
        // If we received a byte
        if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
        {
            USART_ClearITPendingBit(USART1, USART_IT_RXNE);
            USART1Manager.rxIRQHandler();
        }
    }
}

/////////////////////////////////////////////////////////////////////
// write adaptor
void usart1Write(const char* ptr, int len)
{
    USART1Manager.write(ptr, len);
}
