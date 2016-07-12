/*
 * smart-point-ui.cpp
 *
 *  Created on: 11 июл. 2016 г.
 *      Author: alexey
 */

#include "smart-point/smart-point-ui.hpp"
#include "core/pinout.hpp"

SmartPointBlinkPatterns SmartPointUI::patterns;

UIStateBase::UIStateBase(SmartPointUI& ui) :
	m_ui(ui)
{
}


SmartPointUI::SmartPointUI(SmartPointState& sps) :
	smartPointState(sps),
	leds(smartPointState.currentTeam)
{

}

void SmartPointUI::init()
{
	initKbd();
	initLCD();
	initLEDs();
	m_uiState = &initScreen;
	leds.blink(SmartPointUI::patterns.hello, RGBLeds::red | RGBLeds::green | RGBLeds::blue);
}

void SmartPointUI::initKbd()
{
	m_keybRows[0] = IOPins->getIOPin(2, 0);
	m_keybRows[1] = IOPins->getIOPin(2, 1);
	m_keybRows[2] = IOPins->getIOPin(2, 2);
	m_keybRows[3] = IOPins->getIOPin(2, 3);

	m_keybCols[0] = IOPins->getIOPin(0, 1);
	m_keybCols[1] = IOPins->getIOPin(0, 2);
	m_keybCols[2] = IOPins->getIOPin(0, 3);
	m_keybCols[3] = IOPins->getIOPin(0, 5);

	keyboard.init(4, 4, m_keybRows, m_keybCols);
}

void SmartPointUI::initLCD()
{
	LCD5110Controller::LcdIO lcdio;
	lcdio.spi = SPIs->getSPI(UniversalConnectorPinout::instance().SPIindex);
	lcdio.dataCommand = IOPins->getIOPin(
		UniversalConnectorPinout::instance().lcd5110.D_C.port,
		UniversalConnectorPinout::instance().lcd5110.D_C.pin
	);
	lcdio.reset = IOPins->getIOPin(
		UniversalConnectorPinout::instance().lcd5110.RST.port,
		UniversalConnectorPinout::instance().lcd5110.RST.pin
	);
	lcdio.chipEnable = IOPins->getIOPin(
		UniversalConnectorPinout::instance().lcd5110.CE.port,
		UniversalConnectorPinout::instance().lcd5110.CE.pin
	);

	lcd.init(lcdio);
}

void SmartPointUI::initLEDs()
{
	leds.init(
		IOPins->getIOPin(1, 0),
		IOPins->getIOPin(1, 1),
		IOPins->getIOPin(1, 2)
	);
}


void SmartPointUI::doIteration()
{
	m_uiState = m_uiState->doIteration();
}

//////////////////////
//
UIInitScreen::UIInitScreen(SmartPointUI& ui) :
	UIStateBase(ui)
{

}

UIStateBase* UIInitScreen::doIteration()
{
	if (!m_screenUpdated)
	{
		m_ui.lcd.configureSPI();
		m_ui.lcd.clearBuffer();
		m_ui.lcd.setFont(LCD5110Controller::fontStandardAscii5x7);
		m_ui.lcd.stringXY(10, 0, "Caustic LTS");
		m_ui.lcd.stringXY(0, 1, "Select mode:");
		m_ui.lcd.stringXY(0, 2, "A: Start");
		m_ui.lcd.stringXY(0, 3, "B: Program");
		m_ui.lcd.write();
		m_screenUpdated = true;
	}


	uint8_t key = m_ui.keyboard.getKeyPressed();
	if (key != MatrixKeyboard::keyNotPressed)
	{
		if (key == 'A')
		{
			m_ui.smartPointState.beginGame();
			//m_ui.leds.blink(SmartPointUI::patterns.hello, RGBLeds::red | RGBLeds::green | RGBLeds::blue);
			m_ui.leds.blink(SmartPointUI::patterns.hello);
			return &m_ui.timeLeftScreen;
		}
	}

	return this;
}

//////////////////////
//
UITimeLeftScreen::UITimeLeftScreen(SmartPointUI& ui) :
	UIStateBase(ui)
{

}

UIStateBase* UITimeLeftScreen::doIteration()
{
	if (!m_screenUpdated)
	{
		m_ui.lcd.configureSPI();
		m_ui.lcd.clearBuffer();
		m_ui.lcd.setFont(LCD5110Controller::fontStandardAscii5x7);
		char buffer[10];

		m_ui.lcd.stringXY(0, 0, "Red:");
		formatTime(buffer, m_ui.smartPointState.team1TimeLeft / 60, m_ui.smartPointState.team1TimeLeft % 60);
		m_ui.lcd.stringXY(0, 1, buffer);
		m_ui.lcd.write();

		m_ui.lcd.stringXY(0, 2, "Blue:");
		formatTime(buffer, m_ui.smartPointState.team2TimeLeft / 60, m_ui.smartPointState.team2TimeLeft % 60);
		m_ui.lcd.stringXY(0, 3, buffer);
		m_ui.lcd.write();

		//m_screenUpdated = true;
	}

	// If leader team changed
	if (m_lastLeader != m_ui.smartPointState.currentTeam)
	{
		m_lastLeader = m_ui.smartPointState.currentTeam;

		m_ui.leds.blink(SmartPointUI::patterns.teamHasPoint);
	}

	return this;
}

