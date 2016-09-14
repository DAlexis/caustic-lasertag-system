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
	virtual void prepare() { m_screenUpdated = false; }

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
	void prepare() override;

private:
	TeamGameId m_lastLeader = teamGameIdNotATeam;
	bool m_winRegisered = false;
};

class SmartPointBlinkPatterns
{
public:
	const RGBLeds::BlinkPattern hello{100000, 100000, 5};
	const RGBLeds::BlinkPattern teamHasPoint{100000, 500000, 10000};
	const RGBLeds::BlinkPattern teamWin{1000000, 1000000, 10000};
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
