/*
 * exti-interrupts.hpp
 *
 *  Created on: 24 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HW_EXTI_INTERRUPTS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HW_EXTI_INTERRUPTS_HPP_

#include "hal/ext-interrupts.hpp"

#define EXTI_LINES_COUNT	9

enum PortNumbers
{
	PN_GPIOA = 0,
	PN_GPIOB = 1,
	PN_GPIOC = 2,
};

class ExternalInterruptManager : public ExternalInterruptManagerBase
{
public:
	ExternalInterruptManager(uint8_t pinNumber);
	void turnOn();
	void turnOff();
	void init(uint8_t portNumber);

private:
	void IRQHandler();
};

class EXTISPool : public IEXTISPool
{
public:
	EXTISPool();
	~EXTISPool();
	ExternalInterruptManagerBase* getEXTI(uint8_t pinNumber);

private:
	ExternalInterruptManager* m_EXTIS[EXTI_LINES_COUNT];
};

#endif /* LAZERTAG_RIFLE_INCLUDE_HW_EXTI_INTERRUPTS_HPP_ */
