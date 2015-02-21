/*
 * sdcard-fs.hpp
 *
 *  Created on: 09 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_DEV_SDCARD_FS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DEV_SDCARD_FS_HPP_

#include "hal/ff/ff.h"
#include "core/singleton-macro.hpp"

class SDCardFS
{
public:
	static SDCardFS& instance();
	FATFS& getFATFS();
	bool init();
	void deInit();
private:

	//bool m_isMounted = false;
	FATFS m_fatfs;

	static SDCardFS* m_sdcardFS;

	STATIC_DEINITIALIZER_IN_CLASS_DECLARATION;
};







#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_SDCARD_FS_HPP_ */
