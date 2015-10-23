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
#include "head-sensor/head-sensor.hpp"
#include "rifle/rifle.hpp"
#include "bluetooth-bridge/bluetooth-bridge.hpp"
#include "cmsis_os.h"
#include "fatfs.h"
#include <stdio.h>

#ifndef USE_STDPERIPH_SDCARD
	SD_HandleTypeDef hsd;
	HAL_SD_CardInfoTypedef SDCardInfo;
#endif // USE_STDPERIPH_SDCARD
DMA_HandleTypeDef hdma_sdio;
DMA_HandleTypeDef hdmatx;
DMA_HandleTypeDef hdmarx;

void DeviceInitializer::initHW()
{
	HAL_Init();
	initClock();
	initGPIO();
	initSDIO();
	Loggers::initLoggers(1);
	initFatFS();
	info << "Base HW initialization done";
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

	if (!m_fatfsSuccess || pinout->readIni("pinout.ini"))
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

void DeviceInitializer::initGPIO()
{
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();
	// This is to prevent random voltages at MCU pins if something go bad during device initialization
	IOPins->resetAllPins();
}

void DeviceInitializer::initSDIO()
{
#ifndef USE_STDPERIPH_SDCARD
	hsd.Instance = SDIO;
	hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
	hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
	hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
	hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
	hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	hsd.Init.ClockDiv = 3;
#endif // USE_STDPERIPH_SDCARD
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

void DeviceInitializer::initClock()
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

	// HAL_Init already done this thing
/*
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);*/
}
