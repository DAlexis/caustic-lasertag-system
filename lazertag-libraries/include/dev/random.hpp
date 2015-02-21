/*
 * random.hpp
 *
 *  Created on: 01 февр. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_CORE_RANDOM_HPP_
#define LAZERTAG_RIFLE_INCLUDE_CORE_RANDOM_HPP_

#include <stdint.h>

class Random
{
public:
	static uint16_t random16();
	static uint32_t random32();
	static uint32_t random(uint32_t maxValue);

private:
	static uint32_t m_lastSeed;
};




#endif /* LAZERTAG_RIFLE_INCLUDE_CORE_RANDOM_HPP_ */
