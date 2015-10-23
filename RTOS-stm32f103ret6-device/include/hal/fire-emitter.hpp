/*
 * fire-emitter.hpp
 *
 *  Created on: 25 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HAL_FIRE_EMITTER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HAL_FIRE_EMITTER_HPP_

#include "rcsp/base-types.hpp"
#include "core/pinout.hpp"
#include <functional>

using FireEmitterCallback = std::function<void(bool)>;

class IFireEmitter
{
public:
	virtual ~IFireEmitter() {}
	virtual void init(const Pinout& pinout) = 0;
	virtual void setCallback(FireEmitterCallback callback) = 0;
	virtual void startImpulsePack(bool isLedOn, unsigned int delayMs) = 0;
	virtual void setPower(UintParameter powerPercent) = 0;
	virtual void setChannel(unsigned int channel) = 0;
	virtual void setCarrierFrequency(uint32_t frequency) = 0;
};

class FireEmitterBase : public IFireEmitter
{
public:
	FireEmitterBase() : m_callback(nullptr) {}
	void setCallback(FireEmitterCallback callback) { m_callback = callback; }

protected:
	FireEmitterCallback m_callback;
};

class IFireEmittersPool
{
public:
	virtual ~IFireEmittersPool() {}
	virtual IFireEmitter* getFireEmitter(uint8_t emitterNumber) = 0;
};

extern IFireEmittersPool* fireEmittersPool;

#endif /* LAZERTAG_RIFLE_INCLUDE_HAL_FIRE_EMITTER_HPP_ */
