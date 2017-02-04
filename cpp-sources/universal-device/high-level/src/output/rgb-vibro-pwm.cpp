/*
 * rgb-vibro-pwm.cpp
 *
 *  Created on: 4 февр. 2017 г.
 *      Author: dalexies
 */

#include "output/rgb-vibro-pwm.hpp"
#include "core/logging.hpp"

RBGVibroPWM::RBGVibroPWM(UintParameter id, IPWMManager *pwm, IIOPin* v) :
	RGBVibroLocalBase(id),
	m_pwm(pwm),
	m_v(v)
{
	debug << "::::::::::::::::: RBGVibroPWM ";
	m_pwm->init(true, true, true, false);
}

void RBGVibroPWM::update(uint8_t r, uint8_t g, uint8_t b, uint8_t v)
{
	m_pwm->setChannel1(r);
	m_pwm->setChannel2(g);
	m_pwm->setChannel3(b);
/*
	m_pwm->setChannel1(10);
	m_pwm->setChannel2(10);
	m_pwm->setChannel3(10);*/

	if (m_v != nullptr)
	{
		if (v > 128)
			m_v->set();
		else
			m_v->reset();
	}
}
