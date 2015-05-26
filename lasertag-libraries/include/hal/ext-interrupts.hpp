/*
 * ext-interrupts.hpp
 *
 *  Created on: 24 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HAL_EXT_INTERRUPTS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HAL_EXT_INTERRUPTS_HPP_

#include <functional>

using EXTICallback = std::function<void(bool)>;

class IExternalInterruptManager
{
public:
	virtual ~IExternalInterruptManager() {}
	virtual void setCallback(EXTICallback) = 0;
	virtual void turnOn() = 0;
	virtual void turnOff() = 0;
	virtual void init(uint8_t portNumber) = 0;
};

class IEXTISPool
{
public:
	virtual ~IEXTISPool() {}
	virtual IExternalInterruptManager* getEXTI(uint8_t pinNumber) = 0;
	virtual IExternalInterruptManager* getEXTI(uint8_t portNumber, uint8_t pinNumber) = 0;
};

class ExternalInterruptManagerBase : public IExternalInterruptManager
{
public:
	void setCallback(EXTICallback callback) { m_callback = callback; }

protected:
	ExternalInterruptManagerBase(uint8_t pinNumber) :
		m_callback(nullptr),
		m_portNumber(0),
		m_pinNumber(pinNumber)
	{}

	EXTICallback m_callback;
	uint8_t m_portNumber, m_pinNumber;
};


class EXTISPoolBase : public IEXTISPool
{
public:
	virtual IExternalInterruptManager* getEXTI(uint8_t pinNumber) = 0;
	IExternalInterruptManager* getEXTI(uint8_t portNumber, uint8_t pinNumber)
	{
		IExternalInterruptManager* exti = getEXTI(pinNumber);
		exti->init(portNumber);
		return exti;
	}
};

extern IEXTISPool* EXTIS;


#endif /* LAZERTAG_RIFLE_INCLUDE_HAL_EXT_INTERRUPTS_HPP_ */
