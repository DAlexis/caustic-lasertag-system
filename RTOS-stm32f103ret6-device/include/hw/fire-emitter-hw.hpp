/*
 * fire-emitter.hpp
 *
 *  Created on: 26 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HW_FIRE_EMITTER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HW_FIRE_EMITTER_HPP_

#include "hal/fire-emitter.hpp"
#include "utils/memory.hpp"

class LEDFireEmitter : public FireEmitterBase
{
public:
	LEDFireEmitter();
	//~LEDFireEmitter() {}
	void init();
	void startImpulsePack(bool isLedOn, unsigned int delayMs);
	void setPower(UintParameter powerPercent);
	void setChannel(unsigned int channel);
	void setCarrierFrequency(uint32_t frequency);

	void IRQHandler();
private:
	void modulationOn();
	void modulationOff();

	unsigned int m_videoPrescaler;

	bool m_isOn = false;
	uint8_t m_PWMChannel;
	unsigned int m_carrierFrequency = 56000;
	constexpr static unsigned int m_radioTimerPeriod = 10;
	unsigned int m_radioPrescaler;
};

class FireEmittersPool : public IFireEmittersPool
{
public:
	IFireEmitter* getFireEmitter(uint8_t emitterNumber);

private:
	IFireEmitter* m_emitter = nullptr;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_HW_FIRE_EMITTER_HPP_ */
