/*
 * fire-led.hpp
 *
 *  Created on: 04 нояб. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_FIRE_LED_HPP_
#define LAZERTAG_RIFLE_INCLUDE_FIRE_LED_HPP_

using FireLEDMgrCallback = void (*) (void* object);

class FireLEDManager
{
public:
    enum BlinkingState
    {
        BS_OFF = 0,
        BS_ON = 1
    };

    FireLEDManager();
    void init();
    void IRQHandler();

    void setCallback(FireLEDMgrCallback callback, void* object);
    void startImpulsePack(BlinkingState state, unsigned int delayMs);

private:
    void modulationOn();
    void modulationOff();

    unsigned int m_videoPrescaler;

    FireLEDMgrCallback m_callback;
    void* m_callbackObject;
};

extern FireLEDManager fireLED;


#endif /* LAZERTAG_RIFLE_INCLUDE_FIRE_LED_HPP_ */
