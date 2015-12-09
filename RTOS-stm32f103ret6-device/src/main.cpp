/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bootloader/bootloader.h"
#include "head-sensor/head-sensor.hpp"
#include "core/logging.hpp"
#include "core/os-wrappers.hpp"
#include "core/device-initializer.hpp"
#include "dev/wav-player.hpp"
#include "hal/adc.hpp"
#include "hal/rtc.hpp"

#include <functional>
#include <stdio.h>

#include "loader.h"

DeviceInitializer deviceInitializer;

IADC* adc = nullptr;

void placeholder() {}

TaskCycled alive([](){

	RTCTime t;
	RTCManager->getTime(t);
	RTCDate d;
	RTCManager->getDate(d);
	uint16_t val=adc->get();
	info << "I'm alive now " << t.hours << ":" << t.mins << ":" << t.secs << "; "
			<< d.day << "." << d.month << "." << d.year << ", v=" << val;
	info << "FreeRTOS free heap: " << xPortGetFreeHeapSize() << ", min: " << xPortGetMinimumEverFreeHeapSize();

});

TaskOnce sound([](){
	info << "Sound here";


	//WavPlayer::instance().play("piknik.wav", 1);
	//WavPlayer::instance().play("sine.wav", 0);
});

extern "C" void testFunc1(int* arg);

unsigned char test2[] = {0xDE, 0xDE, 0xDE, 0xDE, 0xAD};

int main(void)
{
	__enable_irq();
	int q=43;
	testFunc1(&q);
	deviceInitializer.initHW();
	// Wait for voltages stabilization
	printf("Hal delay\n");
	HAL_Delay(100);
	printf("delay done\n");
#ifdef DEBUG
	debug.enable();
	radio.enable();
	trace.enable();
#endif
	info << "=============== Device initialization ===============";
	info << "System runs with bootloader ver. " << FUNC_PREFIX(getBootloaderVersion)();
	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */


	sound.setStackSize(128);
	//sound.run(2000);

	IAnyDevice* device = deviceInitializer.initDevice("device.ini");

	alive.setStackSize(200);
	alive.run(0, 500, 500, 0);
	adc = ADCs->create();
	adc->init(0,0);
	HAL_Delay(10);


	info << " !!! !!! !!! bootlader_testFuncLib: " << bootloader_testFuncLib();
	info << " !!! !!! !!! main: " << (uint32_t) main << " testFunc1: " << (uint32_t) testFunc1
			<< " bootlader_testFuncLib: " << (uint32_t) bootloader_testFuncLib;
	info << " !!! !!! !!! test2: " << (uint32_t) test2 << " getTestString: " << (uint32_t) getTestString()
			<< " FUNC_PREFIX(testFuncArray)():" << (uint32_t) FUNC_PREFIX(testFuncArray)();
	unsigned char testchar =  FUNC_PREFIX(testFuncArray)()[2];
	if (testchar == 0xAE)
		info << "+++ testchar is good";
	else
		info << "--- testchar is baad";



	Kernel::instance().run();


	// We should never get here as control is now taken by the scheduler
	while (1)
	{
		printf("That's fail\n");
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */
char buf[10];


#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
	//error << "Called assert_failed at " << (char*)file << ":" << (int)line;
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	printf("Wrong parameters value: file %s on line %lu\r\n", file, line);
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
