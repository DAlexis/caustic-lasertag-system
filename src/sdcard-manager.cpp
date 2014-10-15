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

SDCardManager* SDCardManager::m_self = nullptr;

void SDCardManager::init()
{
    if (m_self == nullptr) {
        m_self = (SDCardManager*) malloc(sizeof(SDCardManager));
        new (m_self) SDCardManager();
    }
}

