/*
 * flash.c
 *
 *  Created on: 9 дек. 2015 г.
 *      Author: alexey
 */

#include "flash.h"
#include "fatfs.h"
#include "console.h"
#include "stm32f10x.h"

#include <stdio.h>
#include <string.h>

#define FLASH_BEGIN               0x8000000
#define FLASH_PAGE_SIZE           2048

#define LOADER_SATE_NO_FLASH          0x1234
#define LOADER_SATE_HAS_FLASH         0x4321

extern unsigned int _isr_real;
extern unsigned int _loader_state_addr;
extern unsigned int _estack;

void Reset_Handler();

typedef void (*Callable)();

uint32_t firstPageAddr = 0x8000000;
uint32_t secondPageAddr = 0x8000000 + FLASH_PAGE_SIZE;

uint32_t lastPageAddr = ((uint32_t) &_isr_real) - FLASH_PAGE_SIZE;

uint8_t firstPage[FLASH_PAGE_SIZE];
uint32_t fistPageLen = 0;

uint8_t buffer[FLASH_PAGE_SIZE];
uint32_t bufferLen = 0;

static FATFS fatfs;
static FIL fil;

typedef struct {
	uint32_t state;
	uint32_t mainProgramStackPointer;
	uint32_t mainProgramResetHandler;
} LoaderState;

LoaderState state;

__attribute__ ((section(".loader_state"),used))
uint32_t loaderStateStub[] =
{
		LOADER_SATE_NO_FLASH,
		0x0,
		0x0
};

void erase(uint32_t from, uint32_t to)
{
	for (uint32_t i = from; i <= to; i += FLASH_PAGE_SIZE)
	{
		FLASH_ErasePage(i);
		//printf("Page at %lX erased\n", i);
	}
	printf("Pages from %lX to %lX erased\n", from, to);
}

void saveState()
{
	FLASH_Unlock();
	FLASH_ErasePage((uint32_t) &_loader_state_addr);
	for (uint32_t i=0; i < sizeof(LoaderState) / 4; i++)
	{
		uint32_t targetAddress = (uint32_t) &_loader_state_addr + 4*i;
		uint32_t* pWord = ((uint32_t*) &state) + i;
		FLASH_ProgramWord(targetAddress, *pWord);
	}
	FLASH_Lock();
}

void readState()
{
	memcpy(&state, &_loader_state_addr, sizeof(LoaderState));
}


FRESULT readNextPage(uint8_t *target, uint32_t *readed)
{
	uint16_t blocksCount = 16;
	uint16_t fileBlock = FLASH_PAGE_SIZE / blocksCount;
	*readed = 0;
	UINT readedNow = 0;
	FRESULT res = FR_OK;
	for (uint16_t i = 0; i<blocksCount; i++)
	{
		res = f_read(&fil, target, fileBlock, &readedNow);
		target += readedNow;
		*readed += readedNow;
		if (res != FR_OK || readedNow != fileBlock)
			break;
	}
	return res;
}

void flashWrite(uint32_t position, uint8_t *data, uint32_t size)
{
	for (uint32_t i=0; i<size; i+=4)
	{
		if (position == FLASH_BEGIN)
			printf("%lX\n", position + i);
		FLASH_Status res = FLASH_ProgramWord(position + i, *(uint32_t*)&data[i]);
		if (res != FLASH_COMPLETE)
		{
			printf("Error!! Error!! Flash programming failed: %d\n", res);
		}
	}

}

void flash()
{
	readState();
	if (state.state == LOADER_SATE_HAS_FLASH) {
		printf("MCU is flashed, we can boot\n");
		Callable pReset_Handler = (Callable) state.mainProgramResetHandler;
		deinitConsile();
		HAL_DeInit();
		__disable_irq();
		__set_MSP(state.mainProgramStackPointer);
		pReset_Handler();
	}

	FRESULT res = f_mount(&fatfs, "", 1);
	if (res != FR_OK)
	{
		printf("Cannot mount FS!\n");
		return;
	}

	res = f_open(&fil, "flash.bin", FA_OPEN_EXISTING | FA_READ);
	if (res == FR_OK)
	{
		printf("Flash image found on sd-card\n");
		uint32_t position = FLASH_BEGIN + FLASH_PAGE_SIZE;
		printf("Erasing MCU pages except first...\n");
		FLASH_Unlock();
		erase(secondPageAddr, lastPageAddr);

		if (FR_OK == readNextPage(firstPage, &fistPageLen))
		{
			printf("First page of flash readed successfuly\n");
			// Reading original SP and Reset_Handler
			state.mainProgramStackPointer = *(uint32_t*)&firstPage[0];
			state.mainProgramResetHandler = *(uint32_t*)&firstPage[4];
			// Changing it to bootloader ones
			*(uint32_t*)&firstPage[0] = (uint32_t) &_estack;
			*(uint32_t*)&firstPage[4] = (uint32_t) Reset_Handler;
		} else {
			f_close(&fil);
			return;
		}


		/*
		UINT readed = 0;
		do
		{

			f_read(&fil, buffer, 512, &readed);
			bufferLen += readed;
			printf("Flashing at %lX\n", position);

			for (uint32_t i = 0; i < readed; i += 4)
			{
			  FLASH_Status res = FLASH_ProgramWord(position, *(uint32_t*)&buffer[i]);
			  if (res != FLASH_COMPLETE) {
				  printf("At byte %lX error %d\n", position, res);
			  }
			  position += 4;
			}
		} while (readed != 0);*/
/*
		if (programBytesToRead != programBytesCounter)
		{
		  f_read(&program, readBuffer, (programBytesToRead - programBytesCounter), &readBytes);

		  for (uint32_t i = 0; i < (programBytesToRead - programBytesCounter); i += 4)
		  {
		      FLASH_ProgramWord(currentAddress, *(uint32_t*)&readBuffer[i]);
		      currentAddress += 4;
		  }
		  programBytesCounter = programBytesToRead;
		}*/

		do {
			readNextPage(buffer, &bufferLen);
			flashWrite(position, buffer, bufferLen);
			position += bufferLen;
			printf("Page at %lX written\n", position);
		} while (bufferLen != 0);
		f_close(&fil);

		printf("Erasing first page...\n");
		erase(FLASH_BEGIN, FLASH_BEGIN);

		printf("Writing first page...\n");
		flashWrite(FLASH_BEGIN, firstPage, fistPageLen);

		printf("First page written\n");
		FLASH_Lock();
		state.state = LOADER_SATE_HAS_FLASH;
		saveState();
	} else {
		printf("Flash image not found on sd-card\n");
	}



}
