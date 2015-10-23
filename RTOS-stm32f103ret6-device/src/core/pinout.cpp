/*
 * pinout.cpp
 *
 *  Created on: 22 окт. 2015 г.
 *      Author: alexey
 */

#include "core/pinout.hpp"
#include "core/string-utils.hpp"
#include "core/logging.hpp"
#include <functional>

///////////////////
//

Result Pinout::readIni(const char* filename)
{
	IniParcer* parcer = new IniParcer;
	parcer->setCallback(std::bind(&Pinout::readConfigLine, this, std::placeholders::_1, std::placeholders::_2));
	Result res = parcer->parseFile(filename);
	delete parcer;
	return res;
}

void Pinout::readConfigLine(const char* key, const char* value)
{
	static const char portText[] = "_port";
	static const char pinText[] = "_pin";
	static uint8_t portTextLen = strlen(portText);
	static uint8_t pinTextLen = strlen(pinText);
	uint8_t keyLen = strlen(key);
	if (checkSuffix(key, portText))
	{
		std::string name = std::string(key).substr(0, keyLen-portTextLen);
		auto it = m_pins.find(name);
		/// @todo [stability] replace atoi by ananlog without undefined behavior
		uint8_t portNumber = atoi(value);
		if (it != m_pins.end())
			it->second.port = portNumber;
		else
			m_pins[name] = PinDescr(portNumber, 0);
	} else if (checkSuffix(key, pinText))
	{
		std::string name = std::string(key).substr(0, keyLen-pinTextLen);
		auto it = m_pins.find(name);
		/// @todo [stability] replace atoi by ananlog without undefined behavior
		uint8_t pinNumber = atoi(value);
		if (it != m_pins.end())
			it->second.pin = pinNumber;
		else
			m_pins[name] = PinDescr(0, pinNumber);
	} else {
		auto it = m_other.find(key);
		if (it != m_other.end())
		{
			error << "Key duplication in pinout file: " << key;
		}
		m_other[key] = value;
	}
}

void Pinout::printPinout() const
{
	info << "Pinout: ";
	for (auto it = m_pins.begin(); it != m_pins.end(); it++)
	{
		info << it->first << " port:" << it->second.port << " pin:" << it->second.pin;
	}
}

DetailedResult<std::string> Pinout::getParameter(const char* name) const
{
	auto it = m_other.find(name);
	if (it == m_other.end())
	{
		return DetailedResult<std::string>(nullptr, "Cannot find required pinout parameter");
	}
	return DetailedResult<std::string>(it->second);
}
