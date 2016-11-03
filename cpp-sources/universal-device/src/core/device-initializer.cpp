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


#include "core/device-initializer.hpp"
#include "core/logging.hpp"
#include "core/string-utils.hpp"
#include "hal/io-pins.hpp"
#include "hal/rtc.hpp"
#include "hal/hardware-initializer.hpp"
#include "head-sensor/head-sensor.hpp"
#include "smart-point/smart-point.hpp"
#include "rifle/rifle.hpp"
#include "bluetooth-bridge/bluetooth-bridge.hpp"
#include "cmsis_os.h"
#include "fatfs.h"
#include <stdio.h>

#ifndef USE_STDPERIPH_SDCARD
	SD_HandleTypeDef hsd;
	HAL_SD_CardInfoTypedef SDCardInfo;
	DMA_HandleTypeDef hdma_sdio;
	DMA_HandleTypeDef hdmatx;
	DMA_HandleTypeDef hdmarx;
#endif // USE_STDPERIPH_SDCARD


void DeviceInitializer::initEnvironment()
{
	// Initializing base hardware
	hardwareInitializer->init();
	printf("\nPre-init log\n");
	Loggers::initLoggers(1);
	info << "Logging initialized";
	systemClock->init();
	RTCManager->init();
	initFatFS();
	info << "Base environment initialization done";
	printf("\nprintf works()\n");
}

IAnyDevice* DeviceInitializer::initDevice(const char* filename)
{
	IAnyDevice* resultDevice = nullptr;
	if (isSdcardOk())
	{
		info << "Detecting device type using device.ini";
		// We can read device type from config file
		IniParcer* parcer = new IniParcer;
		parcer->setCallback(
			[&resultDevice] (const char* key, const char* value)
			{
				if (resultDevice != nullptr)
				{
					warning << "Config file should contain only ONE device_type record!";
					return;
				}
				if (0 == strcmp(key, "device_type"))
				{
					if (0 == strcmp(value, "head_sensor"))
					{
						info << "Creating HEAD SENSOR device";
						resultDevice = new HeadSensor;
					}
					else if (0 == strcmp(value, "rifle"))
					{
						info << "Creating RIFLE device";
						resultDevice = new Rifle(RCSPAggregator::instance());
					}
					else if (0 == strcmp(value, "smart_point"))
					{
						info << "Creating SMART POINT device";
						resultDevice = new SmartPoint;
					}
					else if (0 == strcmp(value, "bluetooth_bridge"))
					{
						info << "Creating BLUETOOTH BRIDGE device";
						resultDevice = new BluetoothBridge;
					}
					else
						resultDevice = nullptr;
				}
			}
		);
		Result res = parcer->parseFile(filename);
		if (!res)
		{
			error << "Error while reading " << filename << ": " << res.errorText;
		}
		delete parcer;
	} else {
		info << "SD-card unavaliable";
		info << "Creating BLUETOOTH BRIDGE device";
		resultDevice = new BluetoothBridge;
	}

	if (resultDevice == nullptr)
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
		resultDevice->setDafaultPinout(*pinout);
	}

	if (!resultDevice->checkPinout(*pinout))
	{
		error << "!! Fatal error: invalid pinout for selected device, cannot initialize device !!";
		return nullptr;
	}

	info << "Initializing device...";
	resultDevice->init(*pinout);
	delete pinout;
	return resultDevice;
}

void DeviceInitializer::startOS()
{
	info << "Starting FreeRTOS kernel...";
	osKernelStart();
}

void DeviceInitializer::initFatFS()
{
	MX_FATFS_Init();
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
