/*
 * memory-utils.cpp
 *
 *  Created on: 7 апр. 2016 г.
 *      Author: alexey
 */

#include "utils/memory.hpp"

uint32_t hashLyC(uint32_t hash, const uint8_t * buf, uint32_t size)
{
	for(uint32_t i=0; i<size; i++)
		hash = (hash * 1664525) + buf[i] + 1013904223;

	return hash;
}
