/*
 * i2c-hw.cpp
 *
 *  Created on: 21 дек. 2016 г.
 *      Author: dalexies
 */

#include "hw/i2c-hw.hpp"
#include "core/logging.hpp"

I2CsPool i2csPool;
II2CsPool* I2Cs = &i2csPool;

extern I2C_HandleTypeDef hi2c2;

I2CManager::I2CManager(uint8_t number)
{
    switch (number) {
        case 2:
            m_hi2c = &hi2c2;
            break;
        default:
            error << "I2C port " << number << " not supported";
            return;
    }
}

void I2CManager::init()
{
    m_hi2c->Instance = I2C2;
    m_hi2c->Init.ClockSpeed = 100000;
    m_hi2c->Init.DutyCycle = I2C_DUTYCYCLE_2;
    m_hi2c->Init.OwnAddress1 = 0;
    m_hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    m_hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    m_hi2c->Init.OwnAddress2 = 0;
    m_hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    m_hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_OK != HAL_I2C_Init(m_hi2c))
    {
        error << "I2C bus initialization failed!";
    }
}

bool I2CManager::transmit(uint16_t address, uint8_t* data, uint16_t size)
{
    HAL_StatusTypeDef res = HAL_I2C_Master_Transmit_DMA(m_hi2c, address, data, size);

    if (res != HAL_OK)
    {
        error << "Error while HAL_I2C_Master_Transmit_DMA(...): " << res;
        return false;
    }
    while(HAL_DMA_GetState(m_hi2c->hdmatx) != HAL_DMA_STATE_READY)
    {
    }
    return true;
}

bool I2CManager::isDeviceReady(uint16_t address, uint32_t Trials, uint32_t Timeout)
{
    HAL_StatusTypeDef res = HAL_I2C_IsDeviceReady(m_hi2c, address, Trials, Timeout);
    if (res != HAL_OK)
    {
        error << "HAL_I2C_IsDeviceReady failed: " << res;
        return false;
    }
    return true;
}

II2CManager* I2CsPool::get(uint8_t number)
{
    if (number != 2)
    {
        error << "I2CsPool::get(): only I2C #2 supported";
        return nullptr;
    }
    if (m_i2cs[number-1] == nullptr)
    {
        m_i2cs[number-1] = new I2CManager(number);
    }

    return m_i2cs[number-1];
}
