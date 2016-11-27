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

#ifndef INCLUDE_CORE_PINOUT_HPP_
#define INCLUDE_CORE_PINOUT_HPP_

#include "core/result-code.hpp"
#include <map>
#include <string>


class Pinout
{
public:
	/// Note: GPIOA has port number 0, GPIOB has 1, ...
	struct PinDescr
	{
		/// Special pin number for pins that are disabled on PCB
		constexpr static uint8_t pinNotExistsPin = 255;

		PinDescr() :
			pin(pinNotExistsPin)
		{
		}

		PinDescr(uint8_t _port, uint8_t _pin, bool _inverted = false) :
				port(_port), pin(_pin), inverted(_inverted)
		{
		}

		inline __attribute__ ((always_inline)) bool exists() const
		{
			return pin != pinNotExistsPin;
		}

		inline operator bool() const
		{
			return exists();
		}

		uint8_t port = 0;
		uint8_t pin = 0;
		bool inverted = false;
	};

	// @todo Remove this strict inlines
	Result readIni(const char* filename);
	inline __attribute__ ((always_inline)) void set(const char* pinName, uint8_t port, uint8_t pin)
	{
		m_pins[pinName] = PinDescr(port, pin);
	}

	inline __attribute__ ((always_inline)) void unset(const char* pinName)
	{
		auto it = m_pins.find(pinName);
		if (it != m_pins.end())
			m_pins.erase(it);
	}

	inline const PinDescr& operator[](const char* key) const
	{
		auto it = m_pins.find(key);
		if (it != m_pins.end())
			return it->second;
		else
		{
			static const PinDescr noPin;
			return noPin;
		}
	}

	void printPinout() const;
	DetailedResult<std::string> getParameter(const char* name) const;
private:

	std::map<std::string, PinDescr> m_pins;
	std::map<std::string, std::string> m_other;

};

class UniversalConnectorPinout
{
public:
	struct RFID
	{
		Pinout::PinDescr CE{0, 14};
		Pinout::PinDescr RST{1, 5};
	};
	struct LCD5110
	{
		Pinout::PinDescr D_C{1, 9};
		Pinout::PinDescr RST{1, 10};
		Pinout::PinDescr CE{1, 11};
	};

	const uint8_t SPIindex = 3;
	const RFID rfid;
	const LCD5110 lcd5110;

	static UniversalConnectorPinout& instance();
};

#endif /* INCLUDE_CORE_PINOUT_HPP_ */
