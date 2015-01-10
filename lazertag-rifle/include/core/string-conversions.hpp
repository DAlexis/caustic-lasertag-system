/*
 * string-conversions.hpp
 *
 *  Created on: 11 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_CORE_STRING_CONVERSIONS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_CORE_STRING_CONVERSIONS_HPP_

#include <stdint.h>
#include <stdlib.h>

template<typename Type>
class StringParser
{
public:
	bool isSupported() { return false; }

	Type parse(const char* str) { }
};

template<>
class StringParser<uint32_t>
{
public:
	bool isSupported() { return true; }

	uint32_t parse(const char* str) { return (uint32_t) atoi(str); }
};


#endif /* LAZERTAG_RIFLE_INCLUDE_CORE_STRING_CONVERSIONS_HPP_ */
