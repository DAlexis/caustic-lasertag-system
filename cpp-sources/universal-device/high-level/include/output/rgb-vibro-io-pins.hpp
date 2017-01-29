/*
 * rgb-vibro-io-pin.hpp
 *
 *  Created on: 29 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_OUTPUT_RGB_VIBRO_IO_PINS_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_OUTPUT_RGB_VIBRO_IO_PINS_HPP_

#include "output/rgb-vibro-builtin-base.hpp"

class RBGVibroIOPins : public RGBVibroLocalBase
{
public:
	RBGVibroIOPins(UintParameter id, IIOPin* r, IIOPin* g, IIOPin* b, IIOPin* v);

private:
	void update(uint8_t r, uint8_t g, uint8_t b, uint8_t v) override;

	IIOPin *m_r, *m_g, *m_b, *m_v;
};

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_OUTPUT_RGB_VIBRO_IO_PINS_HPP_ */
