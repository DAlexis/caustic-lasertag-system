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


#include "bluetooth-bridge/bluetooth-bridge.hpp"
#include "cmsis_os.h"
#include "core/device-initializer.hpp"
#include "core/entry-point.h"
#include "core/logging.hpp"
#include "core/string-utils.hpp"
#include "fatfs.h"
#include "hal/hardware-initializer.hpp"
#include "hal/io-pins.hpp"
#include "hal/rtc.hpp"
#include "head-sensor/head-sensor.hpp"
#include "rifle/rifle.hpp"
#include "smart-point/smart-point.hpp"
#include <stdio.h>

SINGLETON_IN_CPP(DeviceInitializer)

DeviceInitializer::DeviceInitializer()
{
}

void DeviceInitializer::initEnvironment()
{
	Loggers::initLoggers();
#ifdef DEBUG
    debug.enable();
#ifndef DBG_RADIO_DISABLE
    radio.enable();
#endif
    trace.enable();
#endif
	info << "Logging initialized";
	systemClock->init();
	initFatFS();
	info << "Base environment initialization done";
}

IAnyDevice* DeviceInitializer::initDevice(const char* filename)
{
	if (isSdcardOk())
	{
		info << "Detecting device type using " << filename;
		// We can read device type from config file
		IniParser* parser = new IniParser;
		parser->setCallback(
			[this] (const char* key, const char* value)
			{
				if (m_device != nullptr)
				{
					warning << "Config file should contain only ONE device_type record!";
					return;
				}
				if (0 == strcmp(key, "device_type"))
				{
					if (0 == strcmp(value, "head_sensor"))
					{
						info << "Creating HEAD SENSOR device";
						m_device = new HeadSensor;
					}
					else if (0 == strcmp(value, "rifle"))
					{
						info << "Creating RIFLE device";
						m_device = new Rifle(RCSPAggregator::instance());
					}
					else if (0 == strcmp(value, "smart_point"))
					{
						info << "Creating SMART POINT device";
						m_device = new SmartPoint;
					}
					else if (0 == strcmp(value, "bluetooth_bridge"))
					{
						info << "Creating BLUETOOTH BRIDGE device";
						m_device = new BluetoothBridge;
					}
					else
					    m_device = nullptr;
				}
			}
		);
		Result res = parser->parseFile(filename);
		if (!res)
		{
			error << "Error while reading " << filename << ": " << res.errorText;
		}
		delete parser;
	} else {
		info << "SD-card unavailable";
		info << "Creating BLUETOOTH BRIDGE device";
		m_device = new BluetoothBridge;
	}

	if (m_device == nullptr)
	{
		error << "Fatal error: unknown device type, aborting...\n";
		for (;;);
	}


	info << "Reading device pinout...";
	Pinout* pinout = new Pinout;

	bool pinoutReadingSucceeded = false;
	if (isSdcardOk())
	{
		Result res = pinout->readIni("pinout.ini");
		if (res)
		{
			pinoutReadingSucceeded = true;
		} else {
			error << "Error while reading pinout.ini: " << res.errorText;
		}
	}

	if (!pinoutReadingSucceeded)
	{
		error << "Cannot read device pinout, setting to default";
		m_device->setDefaultPinout(*pinout);
	}

	if (!m_device->checkPinout(*pinout))
	{
		error << "!! Fatal error: invalid pinout for selected device, cannot initialize device !!";
		return nullptr;
	}

	info << "Initializing device...";
	m_device->init(*pinout, isSdcardOk());
	delete pinout;
	return m_device;
}

void DeviceInitializer::initFatFS()
{
	info << "Mounting file system...";
	FRESULT res = f_mount(&m_fatfs, "", 1);
	if (res == FR_OK)
	{
		info << "File system succesfuly mounted";
		m_fatfsSuccess = true;
	}
	else
	{
		error << "Error while mounting file system: " << res;
		m_fatfsSuccess = false;
	}
}

bool DeviceInitializer::isSdcardOk() const
{
	return m_fatfsSuccess;
}

void highLevelEntryPoint()
{
    DeviceInitializer::instance().initEnvironment();
    DeviceInitializer::instance().initDevice("device.ini");
}
