/*
 * utils.cpp
 *
 *  Created on: 31 янв. 2015 г.
 *      Author: alexey
 */

#include "tests/utils.hpp"
#include <stdio.h>

void printHex(uint8_t* buffer, size_t size)
{
	for (size_t i=0; i<size; i++)
	{
		printf("%x ", buffer[i]);
	}
	printf("\n");
}
