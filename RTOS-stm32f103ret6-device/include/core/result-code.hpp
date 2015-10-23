/*
 * return-codes.hpp
 *
 *  Created on: 07 февр. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_CORE_RETURN_CODES_HPP_
#define LAZERTAG_RIFLE_INCLUDE_CORE_RETURN_CODES_HPP_

#include <stdint.h>
#include <stdio.h>

struct Result
{
public:
	Result() {}
	Result(const char* str) : isSuccess(false), errorText(str)
	{
		//printf("Error result: %s\n", errorText);
	}

	inline void fail(const char* str)
	{
		isSuccess = false;
		errorText = str;
	}

	bool isSuccess = true;
	const char* errorText = nullptr;

	operator bool () { return isSuccess; }
};

template <typename T>
struct DetailedResult : public Result
{
public:
	DetailedResult(const T& _details) :
		Result(),
		details(_details)
	{}
	DetailedResult(const T& _details, const char* str) :
		Result(str),
		details(_details)
	{}
	DetailedResult(const T* _details, const char* str) :
		Result(str)
	{
		if (_details != nullptr)
			details = *_details;
	}

	T details;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_CORE_RETURN_CODES_HPP_ */
