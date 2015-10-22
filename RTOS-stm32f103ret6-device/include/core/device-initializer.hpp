/*
 * device-initializer.hpp
 *
 *  Created on: 16 июня 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_INITIALIZER_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_INITIALIZER_HPP_

#include "stm32f1xx_hal.h"
#include "fatfs.h"
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

		PinDescr(uint8_t _port, uint8_t _pin) :
			port(_port),
			pin(_pin)
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

class IAnyDevice
{
public:
	virtual ~IAnyDevice() {}
	virtual void init(const Pinout& pinout) = 0;
	virtual void setDafaultPinout(Pinout& pinout) = 0;
	virtual bool checkPinout(const Pinout& pinout) = 0;
};

class DeviceInitializer
{
public:
	void startOS();

	void initHW();
	IAnyDevice* initDevice(const char* filename);
	bool isSdcardOk() const;

private:

	void initGPIO();
	void initSDIO();
	void initFatFS();
	void initClock();
	FATFS m_fatfs;
	bool m_fatfsSuccess = false;
};



#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_INITIALIZER_HPP_ */
