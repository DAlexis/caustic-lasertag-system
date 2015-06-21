/*
 * memory.hpp
 *
 *  Created on: 21 июня 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_UTILS_MEMORY_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_UTILS_MEMORY_HPP_

#include <string.h>

template <typename T>
void zerify(T& object)
{
	memset(&object, 0, sizeof(T));
}



#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_UTILS_MEMORY_HPP_ */
