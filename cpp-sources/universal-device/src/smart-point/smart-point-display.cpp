/*
 * smart-point-display.cpp
 *
 *  Created on: 10 июл. 2016 г.
 *      Author: dalexies
 */

#include "smart-point/smart-point-display.hpp"
#include "core/pinout.hpp"
#include "core/logging.hpp"

SmartPointLCD5110Display::SmartPointLCD5110Display()
{

}

void SmartPointLCD5110Display::update()
{
	//info << "display update...";
	m_lcd.configureSPI();
	m_lcd.clearBuffer();
	m_lcd.setFont(LCD5110Controller::fontStandardAscii5x7);
	m_lcd.stringXY(10, 0, "Caustic LTS");
	m_lcd.write();
	//info << "display updated";
}

void SmartPointLCD5110Display::init()
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

	m_lcd.init(lcdio);
}

