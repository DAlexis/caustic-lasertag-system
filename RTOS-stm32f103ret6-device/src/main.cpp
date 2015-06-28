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
#include "head-sensor/head-sensor.hpp"
#include "core/logging.hpp"
#include "core/os-wrappers.hpp"
#include "device-initializer.hpp"
#include <functional>
#include <stdio.h>


DeviceInitializer deviceInitializer;

TaskCycled test;

class Aaa
{
public:
	void printHi()
	{
		info << "Hi!";
	}

};

TaskCycled t2([](){
	info << "Hi!2";
});

TaskCycled t3([](){
	info << "Hi!3";
});

TaskCycled alive([](){
	info << "I'm alive now";
});

HeadSensor headSensor;
HeadSensorPinoutMapping pinout;

int main(void)
{
	deviceInitializer.initDevice();
	debug.enable();
	radio.enable();
	trace.enable();
	info << "=============== Device initialized ===============";
	// Wait for voltages stabilization
	HAL_Delay(1000);

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	info << "Head sensor has size " << sizeof(headSensor);
/*
	HeadSensor *headSensor = nullptr;
	HeadSensorPinoutMapping pinout;
	headSensor = new HeadSensor;
	if (!headSensor)
	{
		error << "Fatal error: cannot allocate HeadSensor object";
		for(;;){}
	}
	headSensor->configure(pinout);*/
	headSensor.configure(pinout);
/*
	Aaa a;
	test.setTask(std::bind(&Aaa::printHi, &a));
	test.setStackSize(1024);
	test.run();
	t2.setStackSize(128);
	t2.run();
	t3.setStackSize(128);
	t3.run();*/
	alive.setStackSize(128);
	alive.run(0, 500, 500, 0);
	HAL_Delay(1000);
	// Turning on console
	//Scheduler::instance().addTask(std::bind(&Console::interrogate, &Console::instance()), false, 500000);

	// Turning on memory checker
	//Scheduler::instance().addTask(checkMemory, false, 500000);

	// Printing console prompt
	//Console::instance().prompt();

	// Running main loop of co-op scheduler
	//Scheduler::instance().mainLoop();
	info << "Starting kernel";
	osKernelStart();

	// We should never get here as control is now taken by the scheduler

	while (1)
	{

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
