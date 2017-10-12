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

#ifndef INCLUDE_NETWORK_RADIO_PHYSICAL_HPP_
#define INCLUDE_NETWORK_RADIO_PHYSICAL_HPP_

#include "core/os-wrappers.hpp"
#include "rcsp/base-types.hpp"

class IRadioPhysicalDevice : public IInterrogatable
{
public:
	using DataReceiveCallback = std::function<void(uint8_t/* channel*/, uint8_t*/* data*/)>;
	using TXDoneCallback = std::function<void(void)>;

	constexpr static unsigned int defaultRFPhysicalPayloadSize = 32;

	virtual ~IRadioPhysicalDevice() {}

	virtual void printStatus() = 0;

	virtual UintParameter getPayloadSize() = 0;

	virtual void setDataReceiveCallback(DataReceiveCallback callback) = 0;
	virtual void setTXDoneCallback(TXDoneCallback callback) = 0;

	virtual void setRXAddress(uint8_t channel, uint8_t* address) = 0;
	virtual void setTXAddress(uint8_t* address) = 0;

	virtual void sendData(uint8_t size, uint8_t* data) = 0;
};

#endif /* INCLUDE_NETWORK_RADIO_PHYSICAL_HPP_ */
