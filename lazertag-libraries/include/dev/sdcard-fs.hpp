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
	struct ScopedLocker
	{
		ScopedLocker() { SDCardFS::instance().lock(); }
		~ScopedLocker() { SDCardFS::instance().unlock(); }
	};
	static SDCardFS& instance();
	FATFS& getFATFS();
	bool init();
	void deInit();
	inline void lock() { m_isLocked = true; }
	inline void unlock() { m_isLocked = false; }
	inline bool isLocked() const { return m_isLocked; }
private:

	//bool m_isMounted = false;
	FATFS m_fatfs;
	bool m_isLocked = false;

	static SDCardFS* m_sdcardFS;

	STATIC_DEINITIALIZER_IN_CLASS_DECLARATION;
};







#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_SDCARD_FS_HPP_ */
