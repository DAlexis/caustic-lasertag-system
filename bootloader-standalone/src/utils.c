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
