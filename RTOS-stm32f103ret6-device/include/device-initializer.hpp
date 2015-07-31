/*
 * device-initializer.hpp
 *
 *  Created on: 16 июня 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_INITIALIZER_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_INITIALIZER_HPP_

#include "stm32f1xx_hal.h"
#include "fatfs.h"

class DeviceInitializer
{
public:
	void initDevice();
	void startOS();
private:
	void initGPIO();
	void initSDIO();
	void initFatFS();
	void initClock();
	FATFS m_fatfs;

};



#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_INITIALIZER_HPP_ */
