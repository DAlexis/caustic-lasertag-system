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


#include "utils/fatfs-utils.hpp"

FRESULT f_read_huge(
		FIL* fp, 		/* Pointer to the file object */
		void* buff,		/* Pointer to data buffer */
		UINT btr,		/* Number of bytes to read */
		UINT* br		/* Pointer to number of bytes read */
)
{
	*br = 0;
	constexpr uint16_t blockSize = 400;
	UINT count = btr / blockSize;
	UINT tail = btr % blockSize;

	UINT readed = 0;

	FRESULT res = FR_OK;

	// Reading round blocks
	for (uint16_t i=0; i<count; i++)
	{
		res = f_read(fp, buff, blockSize, &readed);
		*br += readed;
		if (res != FR_OK)
		{
			return res;
		}
		buff += readed;

		if (readed != blockSize)
			return res;
	}

	if (tail != 0)
	{
		res = f_read(fp, buff, tail, &readed);
		*br += readed;
	}
	return res;
}
