/*
 * hardware-initializer-hw.cpp
 *
 *  Created on: 27 дек. 2015 г.
 *      Author: alexey
 */

#include "hw/hardware-initializer-hw.hpp"
#include "hal/io-pins.hpp"
#include "fatfs.h"
#include "stm32f1xx_hal.h"

HardwareInitializer instance;

IHardwareInitializer* hardwareInitializer = &instance;

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
	//MX_FATFS_Init();
}

void HardwareInitializer::SystemClock_Config()
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
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

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  // SysTick_IRQn interrupt configuration
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
