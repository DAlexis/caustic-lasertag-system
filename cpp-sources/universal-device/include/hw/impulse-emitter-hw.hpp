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


#ifndef LAZERTAG_RIFLE_INCLUDE_HW_FIRE_EMITTER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HW_FIRE_EMITTER_HPP_

#include <hal/impulse-emitter.hpp>
#include "utils/memory.hpp"

class PwmIsrImpulseEmitter : public ImpulseEmitterBase
{
public:
	PwmIsrImpulseEmitter();
	//~LEDFireEmitter() {}
	void init(const Pinout& pinout);
	void startImpulsePack(bool isLedOn, unsigned int delayMs);
	void setPower(UintParameter powerPercent);
	void setCarrierFrequency(uint32_t frequency);

	void IRQHandler();
private:
	void modulationOn();
	void modulationOff();
	void setChannel(unsigned int channel);

	unsigned int m_videoPrescaler;

	bool m_isOn = false;
	uint8_t m_PWMChannel;
	unsigned int m_carrierFrequency = 56000;
	constexpr static unsigned int m_radioTimerPeriod = 10;
	unsigned int m_radioPrescaler;
	uint8_t m_powerLevelsCount = 0;
	uint8_t m_powerChannels[4];
};

class PwmIsrImpulseEmittersPool : public IImpulseEmittersPool
{
public:
	IImpulseEmitter* get();

private:
	IImpulseEmitter* m_emitter = nullptr;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_HW_FIRE_EMITTER_HPP_ */
