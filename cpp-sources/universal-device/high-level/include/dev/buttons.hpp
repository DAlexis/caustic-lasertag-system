/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/

#ifndef LAZERTAG_RIFLE_INCLUDE_DEV_BUTTONS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DEV_BUTTONS_HPP_

#include "hal/io-pins.hpp"
#include "utils/macro.hpp"
#include "core/os-wrappers.hpp"

#include <stdint.h>
#include <functional>
#include <map>

using ButtonPressCallback = std::function<void(bool/* is first press if serial */)>;
using ButtonDepressCallback = std::function<void(void)>;

class ButtonManager : public IInterrogatable
{
public:
	ButtonManager(IIOPin* pin);
	~ButtonManager() {}
	ButtonManager& setCallback(ButtonPressCallback callback) { m_callback = callback; return *this; }
	ButtonManager& setDepressCallback(ButtonDepressCallback callback) { m_depressCallback = callback; return *this; }
	ButtonManager& setAutoRepeat(bool autoRepeat);
	ButtonManager& setRepeatPeriod(uint32_t& repeatPeriod);
	void useEXTI(bool enabled = true);
	void turnOn();
	void turnOff();
	void setPressedState(bool pressedState = false);
	//bool useExti() { return m_extiManager ? true : false; }
	bool state() const;   /// @return true if pressed, false if depressed

	void interrogate();

	constexpr static uint32_t AntiBounceDelay = 5000;
private:
	bool wasBounce();
	void extiCallback(bool state);

	uint32_t m_lastBounceTestTime = 0;

	ButtonPressCallback m_callback = nullptr;
	ButtonDepressCallback m_depressCallback = nullptr;
	IIOPin* m_pin = nullptr;

	uint32_t* m_repeatPeriod = nullptr;
	uint32_t m_lastPressTime = 0;
	bool m_autoRepeat = false;
	bool m_extiDetected = false;
	bool m_isFirst = false;           /// Last detected button state (pressed or not)
	bool m_isFirstPress = true;
	bool m_isEnabled = true;
	bool m_pressedState = false;
	bool m_pressedAndNotDepressed = false;
};

class ButtonsPool
{
public:
	ButtonsPool() {}
	ButtonManager* getButtonManager(uint8_t portNumber, uint8_t pinNumber);

	SINGLETON_IN_CLASS(ButtonsPool)

private:
	void createButtonManager(uint8_t portNumber, uint8_t pinNumber);

	std::map<std::pair<uint8_t, uint8_t>, ButtonManager*> m_buttonManagers;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_BUTTONS_HPP_ */
