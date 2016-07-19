/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
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
