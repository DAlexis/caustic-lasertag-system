/*
 * device-address.cpp
 *
 *  Created on: 1 нояб. 2017 г.
 *      Author: dalexies
 */

#include "network/network-base-types.hpp"
#include "core/logging.hpp"

///////////////////////
// DeviceAddress

void DeviceAddress::convertFromString(const char* str)
{
	/// @todo Improve parcer to be absolutely stable
	const char* pos = str;
	constexpr unsigned int tmpSize = 20;
	char tmp[tmpSize];
	for (int i=0; i<size; i++)
	{
		unsigned int cursor = 0;

		while (*pos != '\0' && *pos != '.')
			tmp[cursor++] = *pos++;

		if (cursor == tmpSize)
		{
			error << "Parsing failed: too long line";
			return;
		}

		if (*pos == '\0' && i != size-1)
		{
			error << "Parsing failed: inconsistent address";
			return;
		}

		tmp[cursor] = '\0';

		address[i] = atoi(tmp);
		pos++;
	}
	trace << "Parsed: " << address[0] << "-" << address[1] << "-" << address[2];
}
