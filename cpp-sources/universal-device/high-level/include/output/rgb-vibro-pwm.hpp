/*
 * rgb-vibro-pwm.hpp
 *
 *  Created on: 4 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_OUTPUT_RGB_VIBRO_PWM_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_OUTPUT_RGB_VIBRO_PWM_HPP_

#include "output/rgb-vibro-builtin-base.hpp"
#include "hal/pwm.hpp"

class RBGVibroPWM : public RGBVibroLocalBase
{
public:
	RBGVibroPWM(UintParameter id, IPWMManager *pwm, IIOPin* v);

private:
	void update(uint8_t r, uint8_t g, uint8_t b, uint8_t v) override;

	IPWMManager *m_pwm;
	IIOPin *m_v;
};




#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_OUTPUT_RGB_VIBRO_PWM_HPP_ */
