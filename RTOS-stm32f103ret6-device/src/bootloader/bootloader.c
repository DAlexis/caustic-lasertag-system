/*
 * bootloader.c
 *
 *  Created on: 6 дек. 2015 г.
 *      Author: alexey
 */

#include "bootloader/bootloader.h"

extern void main();

void testFunc1(int* arg)
{
	(*arg)++;
}

void bootloaderMain()
{
	main();
}
