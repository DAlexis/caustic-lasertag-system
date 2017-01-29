/*
 * rgb-vibro-builtin-base.hpp
 *
 *  Created on: 29 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_SRC_OUTPUT_RGB_VIBRO_BUILTIN_BASE_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_SRC_OUTPUT_RGB_VIBRO_BUILTIN_BASE_HPP_

#include "output/illumination.hpp"

#include "hal/io-pins.hpp"
#include "hal/system-clock.hpp"
#include "utils/interfaces.hpp"

class RGBVibroLocalBase : public IRGBVibroPointPhysical, public IInterrogatable
{
public:
	RGBVibroLocalBase(UintParameter id);

	void applyIlluminationScheme(IllumitationScheme* scheme) override;
	UintParameter getId() override;
	void interrogate() override;

protected:
	virtual void update(uint8_t r, uint8_t g, uint8_t b, uint8_t v) = 0;
	UintParameter m_id;

private:
	void updateState();

	IllumitationScheme* m_currentScheme = nullptr;
	IllumitationScheme::State m_currentState;
	Time m_lastAnimationTime = 0;
	unsigned int m_currentIndex = 0;
};

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_SRC_OUTPUT_RGB_VIBRO_BUILTIN_BASE_HPP_ */
