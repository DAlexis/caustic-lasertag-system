/*
 * uart.cpp
 *
 *  Created on: 9 сент. 2015 г.
 *      Author: alexey
 */

#include "hw/uart-hw.hpp"
#include "core/logging.hpp"
#include "utils/macro.hpp"

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

UARTManager* usart[3];

UARTsFactory factory;

IUARTsFactory* UARTSFactory = &factory;

UARTManager* handleToManager(UART_HandleTypeDef* handle)
{
	if (handle == &huart2)
		return usart[1];
	if (handle == &huart3)
		return usart[2];
	return nullptr;
}

void UARTManager::init(uint8_t portNumber, uint32_t baudrate)
{
	USART_TypeDef* instance;
	switch (portNumber)
	{
	case 2:
		m_huart = &huart2;
		usart[1] = this;
		instance = USART2;
		break;
	case 3:
		m_huart = &huart3;
		usart[2] = this;
		instance = USART3;
		break;
	default:
		error << "Cannot control usart " << portNumber;
		return;
	}

	m_huart->Instance = instance;
	m_huart->Init.BaudRate = baudrate;
	m_huart->Init.WordLength = UART_WORDLENGTH_8B;
	m_huart->Init.StopBits = UART_STOPBITS_1;
	m_huart->Init.Parity = UART_PARITY_NONE;
	m_huart->Init.Mode = UART_MODE_TX_RX;
	m_huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	m_huart->Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(m_huart);
	HAL_UART_Receive_IT(m_huart, &(rxBuffer[rxCount]), 1);
}

void UARTManager::transmit(uint8_t* buffer, uint16_t size)
{
	m_txBusy = true;
	HAL_UART_Transmit_IT(m_huart, buffer, size);
}

bool UARTManager::txBusy()
{
	return m_txBusy;
}

bool UARTManager::rxBusy()
{
	return m_rxBusy;
}

void UARTManager::txDoneISR()
{
	m_txBusy = false;
	if (m_txCallback)
		m_txCallback();
}

void UARTManager::rxDoneISR(uint8_t* buffer, uint16_t size)
{
	UNUSED_ARG(buffer);
	UNUSED_ARG(size);
	rxCount++;
	if (m_stopCharEnabled && rxBuffer[rxCount-1] == m_stopChar)
	{
		m_rxCallback(rxBuffer, rxCount);
		rxCount = 0;
	} else if (rxCount == rxBufferMaxSize || rxCount == m_blockSize)
	{
		m_rxCallback(rxBuffer, rxCount);
		rxCount = 0;
	}

	HAL_UART_Receive_IT(m_huart, &(rxBuffer[rxCount]), 1);
}

///////////////////
// UARTsFactory

IUARTManager* UARTsFactory::create()
{
	return new UARTManager;
}

extern "C"
{
	void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	{
		handleToManager(huart)->rxDoneISR(huart->pRxBuffPtr, huart->RxXferSize);

	}

	void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
	{
		handleToManager(huart)->txDoneISR();

	}
}
