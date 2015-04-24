/*
 * sdcard-manager.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_SDCARD_MANAGER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_SDCARD_MANAGER_HPP_

#include "ff.h"

class SDCardManager
{
public:
    SDCardManager();
    bool mount();
    void umount();
    bool isReady();

    FIL* getDefaultFIL();
private:
    bool m_ready;
    FATFS m_fatfs;
    FIL m_fil;
};

extern SDCardManager SDCard;

#endif /* LAZERTAG_RIFLE_INCLUDE_SDCARD_MANAGER_HPP_ */
