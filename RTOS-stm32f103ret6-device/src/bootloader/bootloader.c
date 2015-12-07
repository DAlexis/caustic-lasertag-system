/*
 * bootloader.c
 *
 *  Created on: 6 дек. 2015 г.
 *      Author: alexey
 */

#include "bootloader/bootloader.h"

extern void main();
unsigned char testString[] = {0xBE, 0xBE, 0xBE, 0xBE, 0xEF};

char* getTestString()
{
	return testString;
}

void testFunc1(int* arg)
{
	(*arg)++;
}

void bootloaderMain()
{
	main();
}
