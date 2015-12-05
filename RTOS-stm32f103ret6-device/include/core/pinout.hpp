/*
 * pinout.hpp
 *
 *  Created on: 22 окт. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_CORE_PINOUT_HPP_
#define INCLUDE_CORE_PINOUT_HPP_

#include "core/result-code.hpp"
#include <map>
#include <string>


class Pinout
{
public:
	struct PinDescr
	{
		/// Special pin number for pins that are disabled on PCB
		constexpr static uint8_t pinNotExistsPin = 255;

		PinDescr() :
			pin(pinNotExistsPin)
		{
		}

		PinDescr(uint8_t _port, uint8_t _pin, bool _inverted = false) :
			port(_port),
			pin(_pin),
			inverted(_inverted)
		{}

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

	Result readIni(const char* filename);
	inline __attribute__ ((always_inline)) void set(const char* pinName, uint8_t port, uint8_t pin)
	{
		m_pins[pinName] = PinDescr(port, pin);
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
	void readConfigLine(const char* key, const char* value);

	std::map<std::string, PinDescr> m_pins;
	std::map<std::string, std::string> m_other;

};







#endif /* INCLUDE_CORE_PINOUT_HPP_ */