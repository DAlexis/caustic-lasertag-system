/*
 * buttons-manager-base.hpp
 *
 *  Created on: 14 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_DEVICE_INCLUDE_HAL_BUTTONS_MANAGER_BASE_HPP_
#define LAZERTAG_DEVICE_INCLUDE_HAL_BUTTONS_MANAGER_BASE_HPP_

#include "hal/buttons-manager.hpp"

#define BUTTONS_COUNT       4

class ButtonsManagerBase : public IButtonsManager
{
public:
	ButtonsManagerBase();
	void setButtonCallback(unsigned int button, ButtonCallback callback, void* callbackObject);
	void interrogateAllButtons();

protected:
	virtual void interrogateButton(unsigned int button) = 0;

	ButtonCallback m_callbacks[BUTTONS_COUNT];
	void* m_callbackObjects[BUTTONS_COUNT];
};


#endif /* LAZERTAG_DEVICE_INCLUDE_HAL_BUTTONS_MANAGER_BASE_HPP_ */
