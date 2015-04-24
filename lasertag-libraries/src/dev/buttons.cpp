/*
 * buttons.cpp
 *
 *  Created on: 05 янв. 2015 г.
 *      Author: alexey
 */

#include "dev/buttons.hpp"
#include "hal/system-clock.hpp"

ButtonsPool* ButtonsPool::m_buttonsPoolInstance = nullptr;
ButtonsPool::StaticDeinitializer ButtonsPool::m_deinitializer;

ButtonManager::ButtonManager(IIOPin* inputInterrogator) :
    m_inputInterrogator(inputInterrogator)
{
}

void ButtonManager::setExti(IExternalInterruptManager* extiManager)
{
	m_extiManager = extiManager;

	if (m_extiManager) {
		// We work with interruption
		m_extiManager->setCallback(std::bind(&ButtonManager::extiCallback, this, std::placeholders::_1));
		m_extiManager->turnOn();
	}
}

ButtonManager& ButtonManager::setAutoRepeat(bool autoRepeat)
{
	m_autoRepeat = autoRepeat;
	return *this;
}

ButtonManager& ButtonManager::setRepeatPeriod(uint32_t repeatPeriod)
{
	m_repeatPeriod = repeatPeriod;
	return *this;
}

void ButtonManager::turnOn()
{
	if (m_extiManager) {
		m_extiManager->turnOn();
	}
	m_isEnabled = true;
}

void ButtonManager::turnOff()
{
	if (m_extiManager) {
		m_extiManager->turnOff();
	}
	m_isEnabled = false;
}

bool ButtonManager::state()
{
	return !m_inputInterrogator->state();
}

void ButtonManager::extiCallback(bool state)
{
	if (wasBounce()) return;
	uint32_t time = systemClock->getTime();
	if (state == false && m_lastState == true && time - m_lastPressTime >= m_repeatPeriod) {
		m_extiDetected = true;
	}
	if (state == true) {
		// Button depressed
		m_lastState = true;
	}
}

void ButtonManager::interrogate()
{
	if (!m_isEnabled) return;
	if (wasBounce()) return;

	if (m_inputInterrogator->state() == false || m_extiDetected == true)
	{
		// Button is pressed
		uint32_t time = systemClock->getTime();
		if (m_autoRepeat == true)
		{
			// Auto-repeating is enabled
			if (time - m_lastPressTime >= m_repeatPeriod)
			{
				// It was enough time since last call
				m_lastPressTime = time;
				m_callback(m_lastState);
				m_lastState = false;
			}
			// It was NOT enough time since last call
		} else {
			// Auto-repeating is disabled
			if (time - m_lastPressTime >= m_repeatPeriod && m_lastState == true)
			{
				// It was enough time since last call
				// Button was suddenly pressed
				m_lastState = false;
				m_lastPressTime = time;
				m_callback(true);
			}
		}
	}
	if (m_inputInterrogator->state() == true) {
		// Button depressed
		m_lastState = true;
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
ButtonsPool& ButtonsPool::instance()
{
	if (!ButtonsPool::m_buttonsPoolInstance)
		ButtonsPool::m_buttonsPoolInstance = new ButtonsPool;

	return *ButtonsPool::m_buttonsPoolInstance;
}

ButtonsPool::StaticDeinitializer::~StaticDeinitializer()
{
	if (ButtonsPool::m_buttonsPoolInstance)
		delete ButtonsPool::m_buttonsPoolInstance;
}

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

void ButtonsPool::setExti(uint8_t portNumber, uint8_t pinNumber, bool useExti)
{
	auto it = m_buttonManagers.find(std::pair<uint8_t, uint8_t>(portNumber, pinNumber));
	if (it == m_buttonManagers.end())
		return;

	if (useExti)
	{
		IExternalInterruptManager* exti = EXTIS->getEXTI(pinNumber);
		exti->init(portNumber);
		it->second->setExti(exti);
	} else
		it->second->setExti(nullptr);
}
