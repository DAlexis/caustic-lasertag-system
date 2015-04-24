/*
 * buttons-manager.hpp
 *
 *  Created on: 14 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_DEVICE_INCLUDE_HAL_BUTTONS_MANAGER_HPP_
#define LAZERTAG_DEVICE_INCLUDE_HAL_BUTTONS_MANAGER_HPP_


using ButtonCallback = void (*) (void* object, bool isFirst);

class IButtonsManager
{
public:
	enum ButtonAutoRepeat {
		BUTTON_AUTO_REPEAT_DISABLE = 0,
		BUTTON_AUTO_REPEAT_ENABLE = 1,
	};

	virtual ~IButtonsManager() {}

	virtual void configButton(unsigned int button, ButtonAutoRepeat autoRepeat, unsigned int minRepeatPeriod) = 0;
	virtual void setButtonCallback(unsigned int button, ButtonCallback callback, void* callbackObject) = 0;
	virtual void interrogateAllButtons() = 0;
};

#endif /* LAZERTAG_DEVICE_INCLUDE_HAL_BUTTONS_MANAGER_HPP_ */
