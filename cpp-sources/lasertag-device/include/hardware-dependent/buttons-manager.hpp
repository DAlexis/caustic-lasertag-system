/*
 * buttons-manager.hpp
 *
 *  Created on: 21 нояб. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_BUTTONS_MANAGER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_BUTTONS_MANAGER_HPP_

#include <stdint.h>
#include "hal/buttons-manager-base.hpp"



class ButtonsManager : public ButtonsManagerBase
{
	/// @todo implement "slow buttons" that do not need interruptions
public:
    ButtonsManager();
    void configButton(unsigned int button, ButtonAutoRepeat autoRepeat, unsigned int minRepeatPeriod);

    void interruptionHandler(unsigned int button);

private:
    void interrogateButton(unsigned int button);


    ButtonAutoRepeat m_autoRepeat[BUTTONS_COUNT];
    uint8_t m_buttonLastState[BUTTONS_COUNT];

    unsigned int m_buttonPeriod[BUTTONS_COUNT];
    unsigned int m_buttonLastPressed[BUTTONS_COUNT];
};




#endif /* LAZERTAG_RIFLE_INCLUDE_BUTTONS_MANAGER_HPP_ */
