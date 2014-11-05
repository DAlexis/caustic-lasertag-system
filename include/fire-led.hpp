/*
 * fire-led.hpp
 *
 *  Created on: 04 нояб. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_FIRE_LED_HPP_
#define LAZERTAG_RIFLE_INCLUDE_FIRE_LED_HPP_

using FireLEDMgrCallback = void (*) (void* object, bool wasOnState);

class FireLEDManager
{
public:
    FireLEDManager();
    void init();
    void IRQHandler();

    void setCallback(FireLEDMgrCallback callback, void* object);
    void startImpulsePack(bool isLedOn, unsigned int delayMs);

private:
    void modulationOn();
    void modulationOff();

    unsigned int m_videoPrescaler;

    FireLEDMgrCallback m_callback;
    void* m_callbackObject;
    bool m_isOn;
};

extern FireLEDManager fireLED;


#endif /* LAZERTAG_RIFLE_INCLUDE_FIRE_LED_HPP_ */
