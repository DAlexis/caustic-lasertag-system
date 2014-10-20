/*
 * radio.hpp
 *
 *  Created on: 19 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_RADIO_HPP_
#define LAZERTAG_RIFLE_INCLUDE_RADIO_HPP_

#include "spi.hpp"

class RadioManager
{
public:
    void init();
    void readTXAdress();
    void writeTXAdress();
private:
    unsigned char writeReg(unsigned char reg, unsigned char size, unsigned char *data);
    unsigned char readReg(unsigned char reg, unsigned char size, unsigned char *data);
    void chipEnableOn();
    void chipEnableOff();
    void chipSelect();
    void chipDeselect();

};

extern RadioManager radio;


#endif /* LAZERTAG_RIFLE_INCLUDE_RADIO_HPP_ */

