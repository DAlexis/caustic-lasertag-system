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

	Type parse(const char*) { return Type(); }
};

template<>
class StringParser<uint32_t>
{
public:
	bool isSupported() { return true; }

	uint32_t parse(const char* str) { return (uint32_t) atoi(str); }
};

template<>
class StringParser<int32_t>
{
public:
	bool isSupported() { return true; }

	int32_t parse(const char* str) { return (int32_t) atof(str); }
};

template<>
class StringParser<uint16_t>
{
public:
	bool isSupported() { return true; }

	uint16_t parse(const char* str) { return (uint16_t) atoi(str); }
};

template<>
class StringParser<int16_t>
{
public:
	bool isSupported() { return true; }

	int16_t parse(const char* str) { return (int16_t) atoi(str); }
};

template<>
class StringParser<uint8_t>
{
public:
	bool isSupported() { return true; }

	uint8_t parse(const char* str) { return (uint8_t) atoi(str); }
};

template<>
class StringParser<int8_t>
{
public:
	bool isSupported() { return true; }

	int8_t parse(const char* str) { return (int8_t) atoi(str); }
};

template<>
class StringParser<float>
{
public:
	bool isSupported() { return true; }

	float parse(const char* str) { return (float) atof(str); }
};

template<>
class StringParser<double>
{
public:
	bool isSupported() { return true; }

	double parse(const char* str) { return (double) atof(str); }
};



#endif /* LAZERTAG_RIFLE_INCLUDE_CORE_STRING_CONVERSIONS_HPP_ */
