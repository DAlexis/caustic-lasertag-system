/*
 * i2c.hpp
 *
 *  Created on: 21 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HAL_I2C_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HAL_I2C_HPP_

#include <stdint.h>

class II2CManager
{
public:
    virtual ~II2CManager() {}
    virtual void init() = 0;
    virtual bool transmit(uint16_t address, uint8_t* data, uint16_t size) = 0;
    virtual bool isDeviceReady(uint16_t address, uint32_t Trials, uint32_t Timeout) = 0;
};

class II2CsPool
{
public:
    virtual ~II2CsPool() {}
    virtual II2CManager* get(uint8_t number) = 0;
};

extern II2CsPool* I2Cs;


#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HAL_I2C_HPP_ */
