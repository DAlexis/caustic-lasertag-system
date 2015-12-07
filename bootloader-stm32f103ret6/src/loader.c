/*
 * loader.c
 *
 *  Created on: 7 дек. 2015 г.
 *      Author: alexey
 */

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
	lowLewelSystemInit();
	Reset_Handler();
}

const char* getBootloaderVersion()
{
	return bootloaderVersion;
}
