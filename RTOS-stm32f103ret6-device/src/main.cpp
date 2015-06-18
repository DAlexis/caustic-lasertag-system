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

extern "C" void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
/*
class TestBase
{
public:
	virtual ~TestBase() {}
	virtual uint8_t* getText() = 0;
};

class TestChild : public TestBase
{
public:
	uint8_t* getText()
	{
		return (uint8_t*) "C++ test\n";
	}
};
*/
//TestBase* tb = new TestChild;

/* USER CODE END 0 */

DeviceInitializer deviceInitializer;

FIL fl;
FATFS m_fatfs;

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	deviceInitializer.initDevice();
	info << "Device initialized";
	//HAL_Delay(1000);

	//Loggers::initLoggers(1);

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

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
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 
  //char *q = malloc(100);
  /*
  char *q = new char[10000];
  memset(q, 0, 10000);

  delete q;*/
  /* Start scheduler */
  info << "Starting kernel";
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/* USER CODE BEGIN 4 */



typedef struct QueueDefinition
{
	int8_t *pcHead;					/*< Points to the beginning of the queue storage area. */
	int8_t *pcTail;					/*< Points to the byte at the end of the queue storage area.  Once more byte is allocated than necessary to store the queue items, this is used as a marker. */
	int8_t *pcWriteTo;				/*< Points to the free next place in the storage area. */

	union							/* Use of a union is an exception to the coding standard to ensure two mutually exclusive structure members don't appear simultaneously (wasting RAM). */
	{
		int8_t *pcReadFrom;			/*< Points to the last place that a queued item was read from when the structure is used as a queue. */
		UBaseType_t uxRecursiveCallCount;/*< Maintains a count of the number of times a recursive mutex has been recursively 'taken' when the structure is used as a mutex. */
	} u;

	List_t xTasksWaitingToSend;		/*< List of tasks that are blocked waiting to post onto this queue.  Stored in priority order. */
	List_t xTasksWaitingToReceive;	/*< List of tasks that are blocked waiting to read from this queue.  Stored in priority order. */

	volatile UBaseType_t uxMessagesWaiting;/*< The number of items currently in the queue. */
	UBaseType_t uxLength;			/*< The length of the queue defined as the number of items it will hold, not the number of bytes. */
	UBaseType_t uxItemSize;			/*< The size of each items that the queue will hold. */

	volatile BaseType_t xRxLock;	/*< Stores the number of items received from the queue (removed from the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */
	volatile BaseType_t xTxLock;	/*< Stores the number of items transmitted to the queue (added to the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */

	#if ( configUSE_TRACE_FACILITY == 1 )
		UBaseType_t uxQueueNumber;
		uint8_t ucQueueType;
	#endif

	#if ( configUSE_QUEUE_SETS == 1 )
		struct QueueDefinition *pxQueueSetContainer;
	#endif

} xQUEUE;

void printQueue(xQUEUE* target)
{
	printf("pcHead = %x, pcTail = %x, pcWriteTo = %x\n", target->pcHead, target->pcTail, target->pcWriteTo);
	printf("uxLength = %x, uxItemSize = %x\n", target->uxLength, target->uxItemSize);
}

/* USER CODE END 4 */
char buf[10];


//extern FATFS *FatFs[_VOLUMES];
/* StartDefaultTask function */
extern "C" void StartDefaultTask(void const * argument)
{
  /* init code for FATFS */
  //std::function<void(void)> func= [] () { HAL_UART_Transmit(&huart1, (uint8_t*)"Hello\n", 6, 100000); };
	info << "Mounting FatFS";
//	printf("************* &m_fatfs = %x\n", &m_fatfs);
	FRESULT res = f_mount(&m_fatfs, "", 1);
	fl.fs = &m_fatfs;
	//xQUEUE *queue = (xQUEUE *)m_fatfs.sobj;
	//printQueue(queue);
	info << "Done, res = " << (int) res;

	//printf("..... m_fatfs = %x, m_fatfs.sobj = %x\n", &m_fatfs, m_fatfs.sobj);
  //f_mount(&m_fatfs, SD_Path, 1);
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	  static int counter = 0;
	  info << "Os works: " << counter << "\n";
	  counter++;

	  FRESULT res = f_open(&fl, "config.ini", FA_OPEN_EXISTING | FA_READ);
	  //printf("After f_open: fl.fs = %x\n", fl.fs);
	  //fl.fs = &m_fatfs;
	  /*xQUEUE *fQueue = (xQUEUE *)fl.fs->sobj;
	  printQueue(fQueue);
	  xQUEUE *queue = (xQUEUE *)m_fatfs.sobj;
	  printQueue(queue);*/
	  //fl.fs->sobj = queue;

	  // if _FS_REENTRANT is 0, this test is OK, else - not.
	  if (fl.fs != &m_fatfs)
		  info << "||| File descriptor has no pointer to fs!";
	  info << "f_open done";
	  if (res != 0)
		  error << "Error :(\n" << res;
	  else
	  {
		  info << "res == 0";
		  UINT br=0;
		  info << "f_read";
		  res = f_read(&fl, buf, 9, &br);
		  info << "f_read done";
		  buf[9] = '\0';
		  if (res == 0)
			  info << "Readed " << br << " byes: " << buf;
		  else
			  error << "file reading error: " << res;
	  }
	  info << "f_close";
	  f_close(&fl);
	  info << "f_close done";
	  //func();
    osDelay(1000);
    info << "delay done";
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
	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
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
