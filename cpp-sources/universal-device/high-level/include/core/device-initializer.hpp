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

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_INITIALIZER_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_INITIALIZER_HPP_

#include "core/result-code.hpp"
#include "fatfs.h"
#include "core/pinout.hpp"
#include "core/service-locator.hpp"

class IAnyDevice
{
public:
	virtual ~IAnyDevice() {}
	virtual void init(const Pinout& pinout, bool isSdcardOk) = 0;
	virtual void setDefaultPinout(Pinout& pinout) = 0;
	virtual bool checkPinout(const Pinout& pinout) = 0;
};

class DeviceInitializer
{
public:
	void startOS();

	void initEnvironment();
	IAnyDevice* initDevice(const char* filename);
	bool isSdcardOk() const;

	SINGLETON_IN_CLASS(DeviceInitializer)
private:
	DeviceInitializer();

	void initGPIO();
	void initSDIO();
	void initFatFS();
	void initClock();

	FATFS m_fatfs;
	ServiceLocator m_serviceLocator;
	bool m_fatfsSuccess = false;
	IAnyDevice* m_device = nullptr;
};



#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_INITIALIZER_HPP_ */
