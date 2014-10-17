/*
 * sdcard-manager.cpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#include "sdcard.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <new>

SDCardManager SDCard;

SDCardManager::SDCardManager() :
    m_ready(false)
{
}


bool SDCardManager::mount()
{
    printf("SD-card (re)initialization...\n");
    FRESULT res;

    umount();

    printf("Mounting volume...\n");
    res = f_mount(&m_fatfs, "", 1); // mount the drive
    if (res)
    {
        printf("error %d occured!\n", res);
        m_ready = false;
        return false;
    } else {
        m_ready = true;
        printf("success!\n");
    }
    return m_ready;
}

void SDCardManager::umount()
{
    f_mount(NULL, "", 1);
}

bool SDCardManager::isReady()
{
    return m_ready;
}
