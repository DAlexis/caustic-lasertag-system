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

#ifndef LAZERTAG_RIFLE_INCLUDE_HAL_FIRE_EMITTER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HAL_FIRE_EMITTER_HPP_

#include "rcsp/base-types.hpp"
#include "core/pinout.hpp"
#include <functional>

using FireEmitterCallback = std::function<void(bool)>;

class IImpulseEmitter
{
public:
	virtual ~IImpulseEmitter() {}
	virtual void init(const Pinout& pinout) = 0;
	virtual void setCallback(FireEmitterCallback callback) = 0;
	virtual void startImpulsePack(bool isLedOn, unsigned int delayMs) = 0;
	virtual void setPower(UintParameter powerPercent) = 0;
	virtual void setChannel(unsigned int channel) = 0;
	virtual void setCarrierFrequency(uint32_t frequency) = 0;
};

class ImpulseEmitterBase : public IImpulseEmitter
{
public:
	ImpulseEmitterBase() : m_callback(nullptr) {}
	void setCallback(FireEmitterCallback callback) { m_callback = callback; }

protected:
	FireEmitterCallback m_callback;
};

class IImpulseEmittersPool
{
public:
	virtual ~IImpulseEmittersPool() {}
	virtual IImpulseEmitter* get() = 0;
};

extern IImpulseEmittersPool* fireEmittersPool;

#endif /* LAZERTAG_RIFLE_INCLUDE_HAL_FIRE_EMITTER_HPP_ */
