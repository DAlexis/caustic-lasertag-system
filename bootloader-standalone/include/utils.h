/*
 * utils.h
 *
 *  Created on: 10 дек. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_UTILS_H_
#define INCLUDE_UTILS_H_

#include <stdint.h>

void delay();
uint32_t HashLy(uint32_t hash, const uint8_t * buf, uint32_t size);

#endif /* INCLUDE_UTILS_H_ */
