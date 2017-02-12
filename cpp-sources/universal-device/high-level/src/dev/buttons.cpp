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


#include "dev/buttons.hpp"
#include "hal/system-clock.hpp"
#include "core/logging.hpp"

SINGLETON_IN_CPP(ButtonsPool)

ButtonManager::ButtonManager(IIOPin* inputInterrogator) :
    m_pin(inputInterrogator)
{
	if (!inputInterrogator)
	{
		error << "Cannot initialize button: input interrogator is nullptr!";
		return;
	}
	m_pin->setExtiCallback([this] (bool arg) { extiCallback(arg); }, true);
}

ButtonManager& ButtonManager::setAutoRepeat(bool autoRepeat)
{
	m_autoRepeat = autoRepeat;
	return *this;
}

ButtonManager& ButtonManager::setRepeatPeriod(uint32_t& repeatPeriod)
{
	m_repeatPeriod = &repeatPeriod;
	return *this;
}

void ButtonManager::turnOn()
{
	m_isEnabled = true;
}

void ButtonManager::turnOff()
{
	m_isEnabled = false;
}

void ButtonManager::useEXTI(bool enabled)
{
	m_pin->enableExti(enabled);
}

void ButtonManager::setPressedState(bool pressedState)
{
	m_pressedState = pressedState;
}

bool ButtonManager::state() const
{
	if (!m_pin)
	{
		error << "Cannot check buttons state: m_inputInterrogator == nullptr";
		return false;
	}
	return m_pressedState ? m_pin->state() : !m_pin->state();
}

void ButtonManager::extiCallback(bool state)
{
	if (wasBounce() || !m_isEnabled) return;
	uint32_t time = systemClock->getTime();
	if (state == m_pressedState && m_isFirst == !m_pressedState && time - m_lastPressTime >= *m_repeatPeriod) {
		m_extiDetected = true;
	}
	if (state == !m_pressedState) {
		// Button depressed
		m_isFirst = !m_pressedState;
	}
}

void ButtonManager::interrogate()
{
	if (!m_pin)
	{
		error << "Cannot interrogate button: m_inputInterrogator == nullptr";
		return;
	}

	if (!m_isEnabled) return;
	if (wasBounce()) return;

	if (m_pin->state() == m_pressedState || m_extiDetected == true)
	{
		// Button is pressed
		uint32_t time = systemClock->getTime();
		if (m_autoRepeat == true)
		{
			// Auto-repeating is enabled
			if (time - m_lastPressTime >= *m_repeatPeriod)
			{
				// It was enough time since last call
				m_lastPressTime = time;
				m_callback(m_isFirst);
				m_pressedAndNotDepressed = true;
				m_isFirst = false;
			}
			// It was NOT enough time since last call
		} else {
			// Auto-repeating is disabled
			if (time - m_lastPressTime >= *m_repeatPeriod && m_isFirst == true)
			{
				// It was enough time since last call
				// Button was suddenly pressed
				m_isFirst = false;
				m_lastPressTime = time;
				m_callback(true);
				m_pressedAndNotDepressed = true;
			}
		}
	}
	if (m_pin->state() == !m_pressedState) {
		// Button depressed
		m_isFirst = true;
		if (m_pressedAndNotDepressed) {
			m_pressedAndNotDepressed = false;
			if (m_depressCallback)
				m_depressCallback();
		}
	}
	m_extiDetected = false;
}

bool ButtonManager::wasBounce()
{
	uint32_t time = systemClock->getTime();
	if (time - m_lastBounceTestTime < AntiBounceDelay)
		return true;
	m_lastBounceTestTime = time;
	return false;
}

/////////////////////
// ButtonsPool


void ButtonsPool::createButtonManager(uint8_t portNumber, uint8_t pinNumber)
{
	m_buttonManagers[std::pair<uint8_t, uint8_t>(portNumber, pinNumber)]
		= new ButtonManager(IOPins->getIOPin(portNumber, pinNumber));
}

ButtonManager* ButtonsPool::getButtonManager(uint8_t portNumber, uint8_t pinNumber)
{
	auto it = m_buttonManagers.find(std::pair<uint8_t, uint8_t>(portNumber, pinNumber));
	if (it == m_buttonManagers.end())
	{
		createButtonManager(portNumber, pinNumber);
		it = m_buttonManagers.find(std::pair<uint8_t, uint8_t>(portNumber, pinNumber));
	}
	return it->second;
}
