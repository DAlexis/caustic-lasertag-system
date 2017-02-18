/*
 * newlib.h
 *
 *  Created on: 16 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_LOW_LEVEL_NEWLIB_NEWLIB_DRIVER_H_
#define UNIVERSAL_DEVICE_LOW_LEVEL_NEWLIB_NEWLIB_DRIVER_H_

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

extern int freeHeap;

void lockOutputPort();
void unlockOutputPort();

#ifdef __cplusplus
    }
#endif


#endif /* UNIVERSAL_DEVICE_LOW_LEVEL_NEWLIB_NEWLIB_DRIVER_H_ */
