/*
 * sdcard-manager.cpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#include "sdcard.hpp"
#include "dynamic-memory.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <new>

SDCardManager* SDCardManager::m_self = nullptr;

SDCardManager::SDCardManager() :
    m_ready(false)
{
}

SDCardManager& SDCardManager::getInstance()
{
    if (m_self == nullptr) {
        createInstance(m_self);
        if (!m_self)
             printf("SDCardManager failed to instanciate! need %d bytes\n", sizeof(SDCardManager));
    }
    return *m_self;
}

bool SDCardManager::mount()
{
    printf("SD-card (re)initialization...\n");
    FRESULT res;

    f_mount(NULL, "", 1);
    //m_pFatfs = (FATFS*) malloc(sizeof(FATFS));

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

bool SDCardManager::isReady()
{
    return m_ready;
}
