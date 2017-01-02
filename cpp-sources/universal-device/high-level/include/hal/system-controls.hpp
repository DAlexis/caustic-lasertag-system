/*
 * system.hpp
 *
 *  Created on: 18 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HAL_SYSTEM_CONTROLS_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HAL_SYSTEM_CONTROLS_HPP_

class ISystemControls
{
public:
    virtual ~ISystemControls() {}
    virtual void rebootMCU() = 0;
};


extern ISystemControls* systemControls;

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HAL_SYSTEM_CONTROLS_HPP_ */
