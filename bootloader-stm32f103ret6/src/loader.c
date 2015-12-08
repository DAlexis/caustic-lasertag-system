/*
 * loader.c
 *
 *  Created on: 7 дек. 2015 г.
 *      Author: alexey
 */

#include "system.h"
#include "console.h"
#include "stm32f10x.h"

void Reset_Handler();

char bootloaderVersion[] = "0.1";

unsigned char testString2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char testString3[] = {0xAE, 0xAE, 0xAE, 0xAE, 0xEF};
unsigned char testString[] = {0xAE, 0xAE, 0xAE, 0xAE, 0xEF};

static int testFunc3()
{
	return 0;
}

int testFunc2()
{
	return 321;
}

int testFuncLib()
{
	return testFunc2();
}

unsigned char* testFuncArray()
{
	return testString;
}

void loaderMain()
{
	getISRTable();
	/*lowLewelSystemInit();

	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	initUart();
	print("\n\n\nBootloader started successfuly\n");
	moveISRForMainProgram();

	uint32_t sysTick = *((uint32_t*)0x0800A000+15);
	if (sysTick == 0x0800b3ad)
	{
		print("SysTick ok\n");
	} else {
		print("SysTick bad\n");
	}
	void (*user_code_entry)(void);
	uint32_t entryPoint = *((uint32_t*)0x0800A000+1);

	user_code_entry = (void (*)(void)) (entryPoint);

	user_code_entry();*/
	moveISRForMainProgram();
	//*(int volatile*)0xE000ED08 = 0x0800A000;
	Reset_Handler__();
}

const char* getBootloaderVersion()
{
	return bootloaderVersion;
}
