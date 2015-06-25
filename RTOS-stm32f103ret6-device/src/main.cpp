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
#include "core/logging.hpp"
#include "core/os-wrappers.hpp"
#include "hal/io-pins.hpp"
#include "dev/nrf24l01.hpp"
#include "device-initializer.hpp"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include <string.h>
#include <functional>
#include <stdio.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

osThreadId defaultTaskHandle;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

DeviceInitializer deviceInitializer;

FIL fl;
FATFS m_fatfs;

TaskOnce defaultTask(
		[]()
		{
			StartDefaultTask(nullptr);
		}
		);

TaskOnce t(
		[]()
			{


				info << "I'm task!! I'm task!! I'm task!! I'm task!! I'm task!! ";
				info << "I'm task!! I'm task!! I'm task!! I'm task!! I'm task!! ";
				info << "I'm task!! I'm task!! I'm task!! I'm task!! I'm task!! ";
			}
  	  	);

int q=0;

TaskCycled tc(
		[]()
			{
				info << "Cycled" << q++;
			}
  	  	);



IIOPin* pin;

void pinTest()
{
	info << "Getting pin";
	pin = IOPins->getIOPin(0, 0);

	pin->setExtiCallback(
		[] (bool state)
		{
			info << "EXTI callback: state is " << state;
		}
	);
	pin->enableExti(true);

	for (;;)
	{
		info << "pin status: " << pin->state();
		osDelay(100);
	}
}

TaskOnce pinTestTask(pinTest);

NRF24L01Manager nrf;

void NRFTest()
{

	nrf.enableDebug();
	nrf.init(
		IOPins->getIOPin(1, 7),
		IOPins->getIOPin(1, 12),
		IOPins->getIOPin(1, 8),
		2,
		false
	);
	nrf.printStatus();
}

TaskOnce interrogateRadio([]()
{
	//nrf.printStatus();

	for (;;)
	{

		osDelay(1000);
		nrf.printStatus();
	}
}
);

int main(void)
{
	deviceInitializer.initDevice();
	info << "=============== Device initialized ===============";
	//HAL_Delay(1000);

	//Loggers::initLoggers(1);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */

  //defaultTask.setStackSize(300);
  //defaultTask.run();
  //pinTestTask.run();
  //t.run(2000);
  //tc.run(0, 519, 0, 10);
	//NRFTest();
	NRFTest();
	interrogateRadio.setStackSize(500);
	interrogateRadio.run(100);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 


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


void StartDefaultTask(void const * argument)
{
	(void)argument;
	info << "Mounting FatFS";
	FRESULT res = f_mount(&m_fatfs, "", 1);
	fl.fs = &m_fatfs;
	info << "Done, res = " << (int) res;
	Mutex mutex;
	for(;;)
	{
		ScopedLock lck(mutex);
		//mutex.lock();
		static int counter = 0;
		info << "Os works: " << counter << "\n";
		counter++;

		FRESULT res = f_open(&fl, "config.ini", FA_OPEN_EXISTING | FA_READ);

		if (res != 0)
			error << "Error :(\n" << res;
		else
		{
			UINT br=0;
			res = f_read(&fl, buf, 9, &br);
			buf[9] = '\0';
			if (res == 0)
				info << "Readed " << br << " byes: " << buf;
			else
				error << "file reading error: " << res;
		}
		f_close(&fl);
		osDelay(1000);
		//mutex.unlock();
	}
  /* USER CODE END 5 */ 
}




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
