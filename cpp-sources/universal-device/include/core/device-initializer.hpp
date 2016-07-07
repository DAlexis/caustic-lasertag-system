/*
 * device-initializer.hpp
 *
 *  Created on: 16 июня 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_INITIALIZER_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_INITIALIZER_HPP_

#include "fatfs.h"
#include "core/result-code.hpp"
#include "core/pinout.hpp"

class IAnyDevice
{
public:
	virtual ~IAnyDevice() {}
	virtual void init(const Pinout& pinout) = 0;
	virtual void setDafaultPinout(Pinout& pinout) = 0;
	virtual bool checkPinout(const Pinout& pinout) = 0;
};

class DeviceInitializer
{
public:
	void startOS();

	void initEnvironment();
	IAnyDevice* initDevice(const char* filename);
	bool isSdcardOk() const;

private:

	void initGPIO();
	void initSDIO();
	void initFatFS();
	void initClock();
	FATFS m_fatfs;
	bool m_fatfsSuccess = false;
};



#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_INITIALIZER_HPP_ */
