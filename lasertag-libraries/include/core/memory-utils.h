/*
 * memory-utils.h
 *
 *  Created on: 04 февр. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_CORE_MEMORY_UTILS_H_
#define LAZERTAG_RIFLE_INCLUDE_CORE_MEMORY_UTILS_H_

extern char isMemoryCorrupted;

#define WHERE_AM_I      {char tmp=0; extern char _Main_Stack_Limit; \
                         printf("pos:%p, free stack:%d\n", (void*)&tmp, (&tmp - &_Main_Stack_Limit)); }


#endif /* LAZERTAG_RIFLE_INCLUDE_CORE_MEMORY_UTILS_H_ */
