/*
 * fire-emitter.hpp
 *
 *  Created on: 26 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HW_FIRE_EMITTER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HW_FIRE_EMITTER_HPP_

#include "hal/fire-emitter.hpp"

class LEDFireEmitter : public FireEmitterBase
{
public:
	LEDFireEmitter();
	//~LEDFireEmitter() {}
	void init();
	void startImpulsePack(bool isLedOn, unsigned int delayMs);
	void setPower(uint8_t powerPercent);

	void IRQHandler();
private:
	void modulationOn();
	void modulationOff();

	unsigned int m_videoPrescaler;

	bool m_isOn;
	uint8_t m_powerLevel;
};

class FireEmittersPool : public IFireEmittersPool
{
public:
	IFireEmitter* getFireEmitter(uint8_t emitterNumber);

private:
	IFireEmitter* m_emitter = nullptr;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_HW_FIRE_EMITTER_HPP_ */