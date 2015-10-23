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

class FireEmittersPool : public IFireEmittersPool
{
public:
	IFireEmitter* getFireEmitter(uint8_t emitterNumber);

private:
	IFireEmitter* m_emitter = nullptr;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_HW_FIRE_EMITTER_HPP_ */
