/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/

#ifndef LAZERTAG_RIFLE_INCLUDE_CORE_RETURN_CODES_HPP_
#define LAZERTAG_RIFLE_INCLUDE_CORE_RETURN_CODES_HPP_

#include <stdint.h>
#include <stdio.h>

struct Result
{
public:
	Result() {}
    ~Result() {}
	Result(const char* str)
	{
		fail(str);
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

	operator T() { return details; }
	T details;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_CORE_RETURN_CODES_HPP_ */
