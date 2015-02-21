/*
 * sdcard-fs.cpp
 *
 *  Created on: 09 янв. 2015 г.
 *      Author: alexey
 */

#include "dev/sdcard-fs.hpp"

#include <stddef.h>

SDCardFS* SDCardFS::m_sdcardFS = nullptr;

STATIC_DEINITIALIZER_IN_CPP_FILE(SDCardFS, m_sdcardFS)

SDCardFS& SDCardFS::instance()
{
	if (!m_sdcardFS)
		m_sdcardFS = new SDCardFS;

	return *m_sdcardFS;
}


FATFS& SDCardFS::getFATFS()
{
	return m_fatfs;
}

bool SDCardFS::init()
{
	f_mount(NULL, "", 1);
	FRESULT res = f_mount(&m_fatfs, "", 1);
	return res == FR_OK ? true : false;
}

void SDCardFS::deInit()
{
	f_mount(NULL, "", 1);
}
