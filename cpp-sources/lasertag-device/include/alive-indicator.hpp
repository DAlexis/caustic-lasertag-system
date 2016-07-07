/*
 * alive-indicator.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_ALIVE_INDICATOR_HPP_
#define LAZERTAG_RIFLE_INCLUDE_ALIVE_INDICATOR_HPP_


class AliveIndicator
{
public:
    AliveIndicator();
    void blink();
private:
    unsigned int m_lastTime;
    bool m_isOn;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_ALIVE_INDICATOR_HPP_ */
