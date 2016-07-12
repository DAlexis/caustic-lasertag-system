/*
 * ui.hpp
 *
 *  Created on: 11 июл. 2016 г.
 *      Author: alexey
 */

#ifndef UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_UI_HPP_
#define UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_UI_HPP_

#include "smart-point/smart-point-config-and-state.hpp"
#include "dev/matrix-keyboard.hpp"
#include "dev/lcd5110.hpp"
#include "hal/io-pins.hpp"
#include "dev/rgb-leds.hpp"

class SmartPointUI;

class UIStateBase
{
public:
	UIStateBase(SmartPointUI& ui);
	virtual ~UIStateBase() {}
	virtual UIStateBase* doIteration() = 0;

protected:
	SmartPointUI& m_ui;
	bool m_screenUpdated = false;
};

class UIInitScreen : public UIStateBase
{
public:
	UIInitScreen(SmartPointUI& ui);
	UIStateBase* doIteration() override;
};

class UITimeLeftScreen : public UIStateBase
{
public:
	UITimeLeftScreen(SmartPointUI& ui);
	UIStateBase* doIteration() override;

private:
	TeamMT2Id m_lastLeader = MT2NotATeam;
};

class SmartPointBlinkPatterns
{
public:
	const RGBLeds::BlinkPattern hello{100000, 100000, 5};
	const RGBLeds::BlinkPattern teamHasPoint{100000, 500000, 10000};
};

class SmartPointUI
{
public:
	SmartPointUI(SmartPointState& sps);
	void init();
	void doIteration();

	SmartPointState& smartPointState;
	MatrixKeyboard keyboard;
	LCD5110Controller lcd;

	UIStateBase* m_uiState = nullptr;
	UIInitScreen initScreen{*this};
	UITimeLeftScreen timeLeftScreen{*this};
	RGBLeds leds;

	static SmartPointBlinkPatterns patterns;
private:
	void initKbd();
	void initLCD();
	void initLEDs();
	IIOPin* m_keybRows[4];
	IIOPin* m_keybCols[4];

};


#endif /* UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_UI_HPP_ */
