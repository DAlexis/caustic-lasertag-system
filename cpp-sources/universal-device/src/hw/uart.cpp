/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/


#include "hw/uart-hw.hpp"
#include "core/logging.hpp"
#include "utils/macro.hpp"

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

UARTManager* usart[3] = {nullptr, nullptr, nullptr};

UARTsFactory factory;

IUARTsFactory* UARTSFactory = &factory;

UARTManager* handleToManager(UART_HandleTypeDef* handle)
{
	if (handle == &huart1)
		return usart[0];
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
	case 1:
		m_huart = &huart1;
		usart[0] = this;
		instance = USART1;
		break;
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

void UARTManager::transmitSync(uint8_t* buffer, uint16_t size, uint32_t timeout)
{
	m_txBusy = true;
	HAL_UART_Transmit(m_huart, buffer, size, timeout);
	m_txBusy = false;
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
		UARTManager *mgr = handleToManager(huart);
		if (mgr)
			mgr->rxDoneISR(huart->pRxBuffPtr, huart->RxXferSize);

	}

	void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
	{
		UARTManager *mgr = handleToManager(huart);
		if (mgr)
			mgr->txDoneISR();

	}
}
