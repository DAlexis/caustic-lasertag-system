/*
 * i2c-hw.hpp
 *
 *  Created on: 21 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HW_I2C_HW_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HW_I2C_HW_HPP_

#include "hal/i2c.hpp"
#include "stm32f1xx_hal.h"

class I2CManager : public II2CManager
{
public:
    I2CManager(uint8_t number);
    void init() override;
    bool transmit(uint16_t address, uint8_t* data, uint16_t size) override;
    bool isDeviceReady(uint16_t address, uint32_t Trials, uint32_t Timeout) override;

private:
    I2C_HandleTypeDef* m_hi2c;
};

class I2CsPool : public II2CsPool
{
public:
    II2CManager* get(uint8_t number) override;

private:
    I2CManager* m_i2cs[2] = {nullptr, nullptr};
};



#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HW_I2C_HW_HPP_ */
