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

	UIStateBase* m_uiState;
	UIInitScreen initScreen{*this};
	UITimeLeftScreen timeLeftScreen{*this};

private:
	void initKbd();
	void initLCD();
	IIOPin* m_keybRows[4];
	IIOPin* m_keybCols[4];


};


#endif /* UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_UI_HPP_ */
