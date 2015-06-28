/*
 * device-initializer.cpp
 *
 *  Created on: 16 июня 2015 г.
 *      Author: alexey
 */

#include "device-initializer.hpp"
#include "core/logging.hpp"
#include "cmsis_os.h"
#include "fatfs.h"
#include <stdio.h>

SD_HandleTypeDef hsd;
HAL_SD_CardInfoTypedef SDCardInfo;

void DeviceInitializer::initDevice()
{
	HAL_Init();
	initClock();
	initGPIO();
	initSDIO();
	Loggers::initLoggers(1);
	initFatFS();
	info << "Base HW initialization done";
	printf("\nprintf works()\n");
	//__HAL_SD_SDIO_DISABLE(&hsd);
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
	__GPIOD_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
}

void DeviceInitializer::initSDIO()
{
	hsd.Instance = SDIO;
	hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
	hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
	hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
	hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
	hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	hsd.Init.ClockDiv = 3;
}

void DeviceInitializer::initFatFS()
{
	MX_FATFS_Init();
	info << "Mounting file system...";
	FRESULT res = f_mount(&m_fatfs, "", 1);
	if (res == FR_OK)
		info << "File system succesfuly mounted";
	else
		error << "Error while mounting file system: " << res;
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
