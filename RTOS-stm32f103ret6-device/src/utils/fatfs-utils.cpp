/*
 * fatfs-utils.cpp
 *
 *  Created on: 25 авг. 2015 г.
 *      Author: alexey
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
