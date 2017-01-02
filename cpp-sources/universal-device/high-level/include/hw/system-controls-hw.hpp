/*
 * system-controls-hw.hpp
 *
 *  Created on: 18 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HW_SYSTEM_CONTROLS_HW_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HW_SYSTEM_CONTROLS_HW_HPP_

#include "hal/system-controls.hpp"

class SystemControls : public ISystemControls
{
public:
    void rebootMCU() override;
};

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HW_SYSTEM_CONTROLS_HW_HPP_ */
