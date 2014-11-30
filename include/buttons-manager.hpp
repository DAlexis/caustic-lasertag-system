/*
 * buttons-manager.hpp
 *
 *  Created on: 21 нояб. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_BUTTONS_MANAGER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_BUTTONS_MANAGER_HPP_

#include <stdint.h>

#define BUTTONS_COUNT       4

using ButtonCallback = void (*) (void* object, bool isFirst);

class ButtonsManager
{
public:

    enum ButtonAutoRepeat {
        BUTTON_AUTO_REPEAT_DISABLE = 0,
        BUTTON_AUTO_REPEAT_ENABLE = 1,
    };

    ButtonsManager();
    void configButton(unsigned int button, ButtonAutoRepeat autoRepeat, unsigned int minRepeatPeriod);

    void setButtonCallback(unsigned int button, ButtonCallback callback, void* callbackObject);
    void interrogateAllButtons();
    void interruptionHandler(unsigned int button);

private:
    void doAction(unsigned int button);
    void interrogateButton(unsigned int button);

    ButtonCallback m_callbacks[BUTTONS_COUNT];
    void* m_callbackObjects[BUTTONS_COUNT];
    ButtonAutoRepeat m_autoRepeat[BUTTONS_COUNT];
    uint8_t m_buttonLastState[BUTTONS_COUNT];

    unsigned int m_buttonPeriod[BUTTONS_COUNT];
    unsigned int m_buttonLastPressed[BUTTONS_COUNT];
};

extern ButtonsManager buttons;




#endif /* LAZERTAG_RIFLE_INCLUDE_BUTTONS_MANAGER_HPP_ */
