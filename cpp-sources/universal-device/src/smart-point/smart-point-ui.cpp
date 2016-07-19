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

#include "smart-point/smart-point-ui.hpp"
#include "core/pinout.hpp"
#include "core/logging.hpp"

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
		char buffer[20];
		sprintf(buffer, "1/2: Cont %d", (int) m_ui.lcd.lcdContrast);
		m_ui.lcd.stringXY(0, 5, buffer);
		m_ui.lcd.write();
		m_screenUpdated = true;
	}


	uint8_t key = m_ui.keyboard.getKeyPressed();
	if (key == 'A')
	{
		m_ui.smartPointState.beginGame();
		m_ui.timeLeftScreen.prepare();
		return &m_ui.timeLeftScreen;
	}

	if (key == 1)
	{
		if (m_ui.lcd.lcdContrast > 1)
		{
			m_ui.lcd.lcdContrast--;
			m_ui.lcd.updateContrast();
			debug << "Contrast dec";
			m_screenUpdated = false;
		}
	}

	if (key == 2)
	{
		if (m_ui.lcd.lcdContrast < 127)
		{
			m_ui.lcd.lcdContrast++;
			m_ui.lcd.updateContrast();
			debug << "Contrast inc";
			m_screenUpdated = false;
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
		m_ui.lcd.stringXY(40, 0, buffer);


		m_ui.lcd.stringXY(0, 1, "Blue:");
		formatTime(buffer, m_ui.smartPointState.team2TimeLeft / 60, m_ui.smartPointState.team2TimeLeft % 60);
		m_ui.lcd.stringXY(40, 1, buffer);


		m_ui.lcd.stringXY(0, 2, "Yellow:");
		formatTime(buffer, m_ui.smartPointState.team3TimeLeft / 60, m_ui.smartPointState.team3TimeLeft % 60);
		m_ui.lcd.stringXY(40, 2, buffer);


		m_ui.lcd.stringXY(0, 3, "Green:");
		formatTime(buffer, m_ui.smartPointState.team4TimeLeft / 60, m_ui.smartPointState.team4TimeLeft % 60);
		m_ui.lcd.stringXY(40, 3, buffer);

		if (m_winRegisered)
		{
			switch (m_ui.smartPointState.currentTeam)
			{
			case 0: m_ui.lcd.stringXY(0, 5, "Red wins!"); break;
			case 1: m_ui.lcd.stringXY(0, 5, "Blue wins!"); break;
			case 2: m_ui.lcd.stringXY(0, 5, "Yellow wins!"); break;
			case 3: m_ui.lcd.stringXY(0, 5, "Green wins!"); break;
			default: break;
			}
		}
		m_ui.lcd.write();

		//m_screenUpdated = true;
	}

	// If leader team changed
	if (m_lastLeader != m_ui.smartPointState.currentTeam)
	{
		m_lastLeader = m_ui.smartPointState.currentTeam;

		m_ui.leds.blink(SmartPointUI::patterns.teamHasPoint);
	}

	if (!m_winRegisered && m_ui.smartPointState.gameState == SmartPointState::gameStateEnd)
	{
		m_ui.leds.blink(SmartPointUI::patterns.teamWin);
		m_winRegisered = true;
	}

	uint8_t key = m_ui.keyboard.getKeyPressed();
	if (key != MatrixKeyboard::keyNotPressed)
	{
		if (key == '#')
		{
			m_ui.smartPointState.stopGame();
			m_ui.leds.blink(SmartPointUI::patterns.hello, RGBLeds::red | RGBLeds::green | RGBLeds::blue);
			m_ui.initScreen.prepare();
			return &m_ui.initScreen;
		}
	}

	return this;
}

void UITimeLeftScreen::prepare()
{
	UIStateBase::prepare();
	m_lastLeader = MT2NotATeam;
	m_winRegisered = false;
}

