/*
 * ir-physical.hpp
 *
 *  Created on: 5 янв. 2016 г.
 *      Author: alexey
 */

#ifndef INCLUDE_IR_IR_PHYSICAL_HPP_
#define INCLUDE_IR_IR_PHYSICAL_HPP_

#include "rcsp/base-types.hpp"
#include "hal/fire-emitter.hpp"
#include "hal/io-pins.hpp"
#include "utils/interfaces.hpp"
#include <functional>
#include <stdint.h>

using IRReceiverCallback = std::function<void(const uint8_t*, uint16_t)>;

class IIRTransmitter
{
public:
	virtual ~IIRTransmitter() {}
	virtual void init() = 0;
	virtual void setFireEmitter(IFireEmitter* emitter) = 0;
	virtual void setPower(const UintParameter& power) = 0;
	virtual void send(const uint8_t* buffer, uint16_t size) = 0;
	virtual bool busy() = 0;
};

class IIRPhysicalReceiver : public IInterrogatable
{
public:
	virtual ~IIRPhysicalReceiver() {}

	virtual void setCallback(IRReceiverCallback callback) = 0;
	virtual void setIOPin(IIOPin* input) = 0;
	virtual void setEnabled(bool enabled) = 0;

	virtual void init() = 0;
	/// Callback will be called from this function if needed
};

class IRTransmitterBase : public IIRTransmitter
{
public:
	void setPower(const UintParameter& power) { m_power = &power; }
	void setFireEmitter(IFireEmitter* emitter) { m_emitter = emitter; }

protected:
	const UintParameter defaultPower = 100;
	const UintParameter* m_power = &defaultPower;
	IFireEmitter* m_emitter = nullptr;
};

class IRReceiverBase : public IIRPhysicalReceiver
{
public:
	void setCallback(IRReceiverCallback callback) { m_callback = callback; }
	void setIOPin(IIOPin* input) { m_input = input; }

protected:
	IRReceiverCallback m_callback = nullptr;
	IIOPin* m_input = nullptr;
};

#endif /* INCLUDE_IR_IR_PHYSICAL_HPP_ */
