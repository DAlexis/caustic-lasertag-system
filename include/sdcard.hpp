/*
 * sdcard-manager.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_SDCARD_MANAGER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_SDCARD_MANAGER_HPP_


class SDCardManager
{
public:
    static void init();
    static SDCardManager& getInstance();

private:
    SDCardManager();
    static SDCardManager* m_self;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_SDCARD_MANAGER_HPP_ */
