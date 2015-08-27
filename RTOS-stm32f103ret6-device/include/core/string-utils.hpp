/*
 * string-conversions.hpp
 *
 *  Created on: 11 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_CORE_STRING_CONVERSIONS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_CORE_STRING_CONVERSIONS_HPP_

#include "result-code.hpp"

#include <functional>
#include "string.h"
#include <stdint.h>
#include <stdlib.h>

void printHex(uint8_t* buffer, size_t size);
const char* parseFRESULT(int res);

inline char lower(char c)
{
	if (c >= 0x41 && c <= 0x5A)
		return c+(0x61-0x41);
	else
		return c;
}

inline bool isSpace(char c)
{
    return c == ' ' || c == '\t';
}

inline bool isCharacter(char c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' || c == '-');
}

inline bool isNumber(char c)
{
    return (c >= 0x30 && c <= 0x39);
}

void printBar(int barLength, int filled);
bool checkSuffix(const char* where, const char* what);

class IniParcer
{
public:
	using AcceptKeyValueCallback = std::function<void(const char* key, const char* value)>;
	void setCallback(AcceptKeyValueCallback callback);

	Result parseFile(const char* filename);

private:
	enum ParcerState
	{
	    PS_COMMENT = 0,
	    PS_WAITING_KEY,
	    PS_READING_KEY,
	    PS_WAITING_EQ,
	    PS_WAITING_VALUE,
	    PS_READING_VALUE,
	};

	constexpr static unsigned int blockSize = 100;
	constexpr static unsigned int keyValMaxLength = 100;

	char buffer[blockSize];
	char key[keyValMaxLength];
	char value[keyValMaxLength];

	AcceptKeyValueCallback m_acceptKeyValueCallback = nullptr;
	char m_errorMessage[128];
};

template<typename Type>
class StringParser
{
public:
	static bool isSupported() { return false; }

	static Type parse(const char*) { return Type(); }
};
/*
template<>
class StringParser<uint32_t>
{
public:
	static bool isSupported() { return true; }

	static uint32_t parse(const char* str) { return (uint32_t) atoi(str); }
};
*/
template<>
class StringParser<uint32_t>
{
public:
	static bool isSupported() { return true; }

	static uint32_t parse(const char* str) { return (uint32_t) atoi(str); }
};

template<>
class StringParser<int32_t>
{
public:
	static bool isSupported() { return true; }

	static int32_t parse(const char* str) { return (int32_t) atoi(str); }
};

template<>
class StringParser<uint16_t>
{
public:
	static bool isSupported() { return true; }

	static uint16_t parse(const char* str) { return (uint16_t) atoi(str); }
};

template<>
class StringParser<int16_t>
{
public:
	static bool isSupported() { return true; }

	static int16_t parse(const char* str) { return (int16_t) atoi(str); }
};

template<>
class StringParser<uint8_t>
{
public:
	static bool isSupported() { return true; }

	static uint8_t parse(const char* str) { return (uint8_t) atoi(str); }
};

template<>
class StringParser<int8_t>
{
public:
	static bool isSupported() { return true; }

	static int8_t parse(const char* str) { return (int8_t) atoi(str); }
};

template<>
class StringParser<float>
{
public:
	static bool isSupported() { return true; }

	static float parse(const char* str) { return (float) atof(str); }
};

template<>
class StringParser<double>
{
public:
	static bool isSupported() { return true; }

	static double parse(const char* str) { return (double) atof(str); }
};

template<>
class StringParser<bool>
{
public:
	static bool isSupported() { return true; }

	static bool parse(const char* str)
	{
		unsigned int cursor = 0;
		while (isSpace(str[cursor])) cursor++;
		const char* begin = &str[cursor];

		while (!isSpace(str[cursor]) && (str[cursor] != '\0')) cursor++;
		unsigned int len = &str[cursor] - begin;

		if (len == 1)
		{
			if (begin[0] == '1')
				return true;
		}

		if (len == 2)
		{
			if (lower(begin[0]) == 'o'
				&& lower(begin[1]) == 'n')
				return true;
		}

		if (len == 4)
		{
			if (lower(begin[0]) == 't'
				&& lower(begin[1]) == 'r'
				&& lower(begin[2]) == 'u'
				&& lower(begin[3]) == 'e')
				return true;
		}

		return false;
	}
};

#endif /* LAZERTAG_RIFLE_INCLUDE_CORE_STRING_CONVERSIONS_HPP_ */
