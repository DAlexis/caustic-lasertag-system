/*
 * hardware-initializer.hpp
 *
 *  Created on: 27 дек. 2015 г.
 *      Author: alexey
 */

#ifndef INC_HAL_HARDWARE_INITIALIZER_HPP_
#define INC_HAL_HARDWARE_INITIALIZER_HPP_

class IHardwareInitializer
{
public:
	~IHardwareInitializer() {}
	virtual void init() = 0;
};

extern IHardwareInitializer* hardwareInitializer;

#endif /* INC_HAL_HARDWARE_INITIALIZER_HPP_ */
