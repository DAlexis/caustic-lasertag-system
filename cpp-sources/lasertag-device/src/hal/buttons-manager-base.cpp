/*
 * buttons-manager-base.cpp
 *
 *  Created on: 14 дек. 2014 г.
 *      Author: alexey
 */

#include "hal/buttons-manager-base.hpp"

ButtonsManagerBase::ButtonsManagerBase()
{
	for (unsigned int i=0; i<BUTTONS_COUNT; i++)
    {
        m_callbacks[i] = nullptr;
        m_callbackObjects[i] = nullptr;
    }
}
void ButtonsManagerBase::setButtonCallback(unsigned int button, ButtonPressCallback callback, void* callbackObject)
{
    m_callbackObjects[button] = callbackObject;
    m_callbacks[button] = callback;
}

void ButtonsManagerBase::interrogateAllButtons()
{
    for (unsigned int i=0; i<BUTTONS_COUNT; i++)
    {
        if (m_callbacks[i])
            interrogateButton(i);
    }
}
