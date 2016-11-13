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


#include "core/pinout.hpp"
#include "core/string-utils.hpp"
#include "core/logging.hpp"
#include <functional>
///////////////////
// Pinout
Result Pinout::readIni(const char* filename)
{
	IniParser* parser = new IniParser;
	parser->setCallback(
			[this](const char* key, const char* value)
			{
				static const char portText[] = "_port";
				static const char pinText[] = "_pin";
				static const char invertedText[] = "_inverted";
				static uint8_t portTextLen = strlen(portText);
				static uint8_t pinTextLen = strlen(pinText);
				uint8_t keyLen = strlen(key);
				if (checkSuffix(key, portText)) // If we have port specification
				{
					std::string name = std::string(key).substr(0, keyLen-portTextLen);
					auto it = m_pins.find(name);
					uint8_t portNumber = strtol(value, NULL, 10);
					if (it != m_pins.end())
						it->second.port = portNumber;
					else
						m_pins[name] = PinDescr(portNumber, 0);
				} else if (checkSuffix(key, pinText)) // or pin specification
				{
					std::string name = std::string(key).substr(0, keyLen-pinTextLen);
					auto it = m_pins.find(name);
					uint8_t pinNumber = strtol(value, NULL, 10);

					if (it != m_pins.end())
						it->second.pin = pinNumber;
					else
						m_pins[name] = PinDescr(0, pinNumber);
				} else if (checkSuffix(key, invertedText)) // or 'inverted' specification
				{
					std::string name = std::string(key).substr(0, keyLen-pinTextLen);
					bool isInverted = StringParser<bool>::parse(value);
					auto it = m_pins.find(name);
					if (it != m_pins.end())
						it->second.inverted = isInverted;
					else
						m_pins[name] = PinDescr(0, 0, isInverted);
				} else {
					auto it = m_other.find(key);
					if (it != m_other.end())
					{
						error << "Key duplication in pinout file: " << key;
					}
					m_other[key] = value;
				}
			});
	Result res = parser->parseFile(filename);
	delete parser;
	return res;
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

UniversalConnectorPinout& UniversalConnectorPinout::instance()
{
	static UniversalConnectorPinout ucp;
	return ucp;
}

