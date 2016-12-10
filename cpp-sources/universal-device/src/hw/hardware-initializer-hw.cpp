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


#include "fatfs.h"
#include "hal/io-pins.hpp"
#include "hw/hardware-initializer-hw.hpp"
#include "stm32f1xx_hal.h"
#include "utils/macro.hpp"
#include "utils/memory.hpp"
#include "usb_device.h"
#include <string.h>

HardwareInitializer instance;

IHardwareInitializer* hardwareInitializer = &instance;

extern "C" void Error_Handler()
{
    for(;;) {}
}

void HardwareInitializer::init()
{
	// Reset of all peripherals, Initializes the Flash interface and the Systick.
	HAL_Init();
	// Configure the system clock
	SystemClock_Config();
	// Initialize all configured peripherals
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_SDIO_SD_Init();
    //MX_USB_DEVICE_Init();
	//MX_FATFS_Init();
}

void HardwareInitializer::SystemClock_Config()
{
  DECL_CLEAN(RCC_OscInitTypeDef, RCC_OscInitStruct);
  DECL_CLEAN(RCC_ClkInitTypeDef, RCC_ClkInitStruct);
  DECL_CLEAN(RCC_PeriphCLKInitTypeDef, PeriphClkInit);
  HAL_StatusTypeDef result = HAL_OK;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  char isLSEOk = 1;
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  result = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if (result != HAL_OK) {
	isLSEOk = 0;
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
	  Error_Handler();
	}
  }
  

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (isLSEOk)
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  else
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void HardwareInitializer::MX_GPIO_Init(void)
{
  __GPIOC_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();

  IOPins->resetAllPins();
}


void HardwareInitializer::MX_SDIO_SD_Init(void)
{
#ifndef USE_STDPERIPH_SDCARD
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_4B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 3;
#endif // USE_STDPERIPH_SDCARD
}

void HardwareInitializer::MX_DMA_Init(void)
{
  // DMA controller clock enable
  __HAL_RCC_DMA2_CLK_ENABLE();

  // DMA interrupt init
  // Priority set to 4 for enabling DAC. Does not work with 5, I dont know why
  HAL_NVIC_SetPriority(DMA2_Channel3_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel3_IRQn);

#ifndef USE_STDPERIPH_SDCARD
  HAL_NVIC_SetPriority(DMA2_Channel4_5_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);
#endif // USE_STDPERIPH_SDCARD
}
