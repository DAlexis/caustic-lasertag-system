/*
 * spi.cpp
 *
 *  Created on: 24 июня 2015 г.
 *      Author: alexey
 */

#include "hw/spi-hw.hpp"

#include "utils/memory.hpp"
#include "core/string-utils.hpp"

#include "core/logging.hpp"
#include "hal/system-clock.hpp"

static SPIsPool pool;

ISPIsPool* SPIs = &pool;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

SPIManager *managers[3] = {nullptr, nullptr, nullptr};

static SPIManager *getManager(SPI_HandleTypeDef* hspi)
{
	if (hspi == &hspi1)
		return managers[0];
	if (hspi == &hspi2)
		return managers[1];
	if (hspi == &hspi3)
		return managers[2];

	return nullptr;
}

static void operationDone_ISR(SPI_HandleTypeDef* hspi)
{
	SPIManager *mgr = getManager(hspi);
	if (mgr)
		mgr->operationDone_ISR();
}


SPIManager::SPIManager(uint8_t SPIindex)
{
	switch(SPIindex)
	{
	case 1:
		m_hspi = &hspi1;
		zerify(*m_hspi);
		m_hspi->Instance = SPI1;
		break;
	case 2:
		m_hspi = &hspi2;
		zerify(*m_hspi);
		m_hspi->Instance = SPI2;
		break;
	case 3:
		m_hspi = &hspi3;
		zerify(*m_hspi);
		m_hspi->Instance = SPI3;
		break;
	default:
		error << "Invalid SPI number";
		return;
	}
	managers[SPIindex-1] = this;
}

void SPIManager::init(uint32_t prescaler, IIOPin* NSSPin)
{
	m_stager.stage("init");
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

	m_hspi->Init.Mode = SPI_MODE_MASTER;
	m_hspi->Init.Direction = SPI_DIRECTION_2LINES;
	m_hspi->Init.DataSize = SPI_DATASIZE_8BIT;
	m_hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
	m_hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
	m_hspi->Init.NSS = SPI_NSS_SOFT;
	m_hspi->Init.BaudRatePrescaler = baudratePrescaler;
	m_hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
	m_hspi->Init.TIMode = SPI_TIMODE_DISABLED;
	m_hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	m_hspi->Init.CRCPolynomial = 7;
	HAL_SPI_Init(m_hspi);

	m_NSSPin = NSSPin;
	m_NSSPin->switchToOutput();
	m_NSSPin->set();
	m_stager.stage("init done");
}

bool SPIManager::Transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	m_stager.stage("Transmit");

	m_operationDone = false;
	taskENTER_CRITICAL();
		HAL_StatusTypeDef res = HAL_SPI_Transmit_IT(m_hspi, pData, Size);
	taskEXIT_CRITICAL();

	if (res != HAL_OK)
	{
		m_stager.stage("Transmit fail");
		error << "SPI Transmit failed:" << res;
		return false;
	}
	m_stager.stage("Transmit: waiting for it");
	if (!waitForISR(Timeout))
	{
		error << "SPI Transmit timeout!";
		return false;
	}

	m_stager.stage("Transmit ok");
	return true;
}

bool SPIManager::Receive(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	m_stager.stage("Receive");
	m_operationDone = false;
	taskENTER_CRITICAL();
		HAL_StatusTypeDef res = HAL_SPI_Receive_IT(m_hspi, pData, Size);
	taskEXIT_CRITICAL();

	if (res != HAL_OK)
	{
		m_stager.stage("Receive fail");
		error << "SPI Receive failed:" << res;
		return false;
	}
	m_stager.stage("Receive: waiting for it");
	if (!waitForISR(Timeout))
	{
		error << "SPI Receive timeout!";
		return false;
	}

	m_stager.stage("Receive ok");
	return true;
}

bool SPIManager::TransmitReceive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
	m_stager.stage("TransmitReceive");

	m_operationDone = false;
	taskENTER_CRITICAL();
		HAL_StatusTypeDef res = HAL_SPI_TransmitReceive_IT(m_hspi, pTxData, pRxData, Size);
	taskEXIT_CRITICAL();
	if (res != HAL_OK)
	{
		m_stager.stage("TransmitReceive fail");
		error << "SPI transmit&receive failed:" << res;
		return false;
	}
	m_stager.stage("TransmitReceive: waiting for it");
	if (!waitForISR(Timeout))
	{
		error << "SPI TransmitReceive timeout!";
		return false;
	}

	m_stager.stage("TransmitReceive ok");
	return true;
}

void SPIManager::operationDone_ISR()
{
	m_operationDone = true;
}

bool SPIManager::waitForISR(uint32_t timeout)
{
	Time begin = systemClock->getTime();
	while (!m_operationDone) {
		if (systemClock->getTime() - begin > timeout)
		{
			// Not false to prevent case when context switching cause 'timeout',
			// but not real timeout
			return m_operationDone;
		}
	}
	return true;
}

extern "C" {
	void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
	{
		operationDone_ISR(hspi);
	}

	void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
	{
		operationDone_ISR(hspi);
	}

	void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
	{
		operationDone_ISR(hspi);
	}
}

ISPIManager* SPIsPool::getSPI(uint8_t portNumber)
{
	return new SPIManager(portNumber);
}

