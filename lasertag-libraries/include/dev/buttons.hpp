/*
 * buttons.hpp
 *
 *  Created on: 04 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_DEV_BUTTONS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DEV_BUTTONS_HPP_

#include "hal/ext-interrupts.hpp"
#include "hal/io-pins.hpp"

#include <stdint.h>
#include <functional>
#include <map>

using ButtonPressCallback = std::function<void(bool/* is first press if serial */)>;
using ButtonDepressCallback = std::function<void(void)>;

class ButtonManager
{
public:
	ButtonManager(IIOPin* inputInterrogator);
	~ButtonManager() {}
	ButtonManager& setCallback(ButtonPressCallback callback) { m_callback = callback; return *this; }
	ButtonManager& setDepressCallback(ButtonDepressCallback callback) { m_depressCallback = callback; return *this; }
	ButtonManager& setAutoRepeat(bool autoRepeat);
	ButtonManager& setRepeatPeriod(uint32_t repeatPeriod);
	void setExti(IExternalInterruptManager* extiManager);
	void turnOn();
	void turnOff();
	void setPressedState(bool pressedState = false);
	bool useExti() { return m_extiManager ? true : false; }
	bool state();   /// @return true if pressed, false if depressed

	void interrogate();

	constexpr static uint32_t AntiBounceDelay = 5000;
private:
	bool wasBounce();
	void extiCallback(bool state);

	uint32_t m_lastBounceTestTime = 0;

	ButtonPressCallback m_callback = nullptr;
	ButtonDepressCallback m_depressCallback = nullptr;
	IExternalInterruptManager* m_extiManager = nullptr;
	IIOPin* m_inputInterrogator = nullptr;

	uint32_t m_repeatPeriod = 500000;
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
	ButtonManager* getButtonManager(uint8_t portNumber, uint8_t pinNumber);
	void setExti(uint8_t portNumber, uint8_t pinNumber, bool useExti = false);

	static ButtonsPool& instance();

private:
	void createButtonManager(uint8_t portNumber, uint8_t pinNumber);

	std::map<std::pair<uint8_t, uint8_t>, ButtonManager*> m_buttonManagers;

	struct StaticDeinitializer
	{ // Yes, this is for Chuck Norris
	public:
		~StaticDeinitializer();
	};

	static ButtonsPool* m_buttonsPoolInstance;
	static StaticDeinitializer m_deinitializer;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_BUTTONS_HPP_ */
