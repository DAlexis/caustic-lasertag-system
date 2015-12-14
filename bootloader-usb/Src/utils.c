/*
 * utils.c
 *
 *  Created on: 10 дек. 2015 г.
 *      Author: alexey
 */

#include "utils.h"
#include <stdint.h>

void delay()
{
	for (volatile uint32_t i=0; i<10000000; i++) {}
}

uint32_t HashLy(uint32_t hash, const uint8_t * buf, uint32_t size)
{
	for(uint32_t i=0; i<size; i++)
		hash = (hash * 1664525) + buf[i] + 1013904223;

	return hash;
}
