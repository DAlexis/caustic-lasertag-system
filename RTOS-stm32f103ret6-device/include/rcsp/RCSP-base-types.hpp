/*
 * rcsp-base-types.hpp
 *
 *  Created on: 4 сент. 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_RCSP_RCSP_BASE_TYPES_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_RCSP_RCSP_BASE_TYPES_HPP_

#include <stdint.h>

#define ADDRESS_TO_STREAM(addr)      (addr).address[0] << "-" << (addr).address[1] << "-" << (addr).address[2]

using OperationSize = uint8_t;
using OperationCode = uint16_t;


#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_RCSP_RCSP_BASE_TYPES_HPP_ */
