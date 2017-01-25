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


#ifndef INCLUDE_IR_IR_PHYSICAL_HPP_
#define INCLUDE_IR_IR_PHYSICAL_HPP_

#include <hal/impulse-emitter.hpp>
#include "rcsp/base-types.hpp"
#include "hal/io-pins.hpp"
#include "utils/interfaces.hpp"
#include <functional>
#include <stdint.h>

using IRReceiverCallback = std::function<void(const uint8_t* data, uint16_t size)>;

class IIRTransmitter
{
public:
	virtual ~IIRTransmitter() {}
	virtual void init(const Pinout& pinout) = 0;
	virtual void setFireEmitter(IImpulseEmitter* emitter) = 0;
	virtual void setPower(const UintParameter& power) = 0;
	virtual void send(const uint8_t* buffer, uint16_t size) = 0;
	virtual bool busy() = 0;
};

class IIRPhysicalReceiver : public IInterrogatable
{
public:
	virtual ~IIRPhysicalReceiver() {}

	virtual void setCallback(IRReceiverCallback callback) = 0;
	virtual void setEnabled(bool enabled) = 0;

	virtual void init() = 0;
};

class IRTransmitterBase : public IIRTransmitter
{
public:
	void setPower(const UintParameter& power) { m_power = &power; }
	void setFireEmitter(IImpulseEmitter* emitter) { m_emitter = emitter; }

protected:
	const UintParameter defaultPower = 100;
	const UintParameter* m_power = &defaultPower;
	IImpulseEmitter* m_emitter = nullptr;
};

class IRReceiverBase : public IIRPhysicalReceiver
{
public:
	void setCallback(IRReceiverCallback callback) override { m_callback = callback; }

protected:
	IRReceiverCallback m_callback = nullptr;
};

#endif /* INCLUDE_IR_IR_PHYSICAL_HPP_ */
