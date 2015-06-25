/*
 * string-utils.cpp
 *
 *  Created on: 25 июня 2015 г.
 *      Author: alexey
 */

#include "core/string-utils.hpp"

void printHex(uint8_t* buffer, size_t size)
{
	for (size_t i=0; i<size; i++)
	{
		printf("%x ", buffer[i]);
	}
	printf("\n");
}
