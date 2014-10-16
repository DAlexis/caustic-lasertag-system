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
    static SDCardManager& getInstance();

    bool mount();
    bool isReady();
    SDCardManager();

private:
    bool m_ready;
    FATFS m_fatfs;
    static SDCardManager* m_self;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_SDCARD_MANAGER_HPP_ */
