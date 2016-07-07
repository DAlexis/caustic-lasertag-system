/*
 * fatfs-utils.hpp
 *
 *  Created on: 25 авг. 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_UTILS_FATFS_UTILS_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_UTILS_FATFS_UTILS_HPP_

#include "fatfs.h"

/**
 * Workaround for f_read to read buffers of any size
 * @param fp    Pointer to the file object
 * @param buff  Pointer to data buffer
 * @param btr   Number of bytes to read
 * @param br    Pointer to number of bytes read
 * @return See FRESULT description. Can be converted to string by parseFRESULT()
 */

FRESULT f_read_huge(
		FIL* fp, 		/* Pointer to the file object */
		void* buff,		/* Pointer to data buffer */
		UINT btr,		/* Number of bytes to read */
		UINT* br		/* Pointer to number of bytes read */
);


#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_UTILS_FATFS_UTILS_HPP_ */
