/*
 * device-initializer.cpp
 *
 *  Created on: 16 июня 2015 г.
 *      Author: alexey
 */

#include "core/device-initializer.hpp"
#include "core/logging.hpp"
#include "core/string-utils.hpp"
#include "hal/io-pins.hpp"
#include "hal/rtc.hpp"
#include "hal/hardware-initializer.hpp"
#include "head-sensor/head-sensor.hpp"
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
	Loggers::initLoggers(1);
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
						resultDevice = new Rifle;
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
		info << "Hey! Im here!11";
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
	//resultDevice->
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
