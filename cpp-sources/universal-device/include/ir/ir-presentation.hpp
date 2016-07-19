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

#ifndef INCLUDE_IR_IR_PRESENTATION_HPP_
#define INCLUDE_IR_IR_PRESENTATION_HPP_

#include "ir-physical.hpp"
#include "rcsp/RCSP-base-types.hpp"
#include "rcsp/RCSP-stream.hpp"

class IPresentationReceiversGroup;

class IIRPresentationReceiver : public IInterrogatable
{
public:
	virtual ~IIRPresentationReceiver() {}
	virtual void init() = 0;
	virtual void setPhysicalReceiver(IIRPhysicalReceiver* receiver) = 0;
	virtual void assignReceiversGroup(IPresentationReceiversGroup& group) = 0;
	virtual void setDamageCoefficient(const FloatParameter* coefficient) = 0;
	virtual void setVibroEngine(IIOPin* pin) = 0;
};

/**
 * Object to control all kill zones that should not produce double receiving.
 * In typical case it is replacement of KillZonesManager
 */
class IPresentationReceiversGroup : public IInterrogatable
{
public:
	virtual ~IPresentationReceiversGroup() {}
	virtual void connectReceiver(IIRPresentationReceiver& receiver) = 0;
	virtual void enableLoggingUnknownCommands(bool enable = true) = 0;
};

class IIRPresentationTransmitter
{
public:
	virtual ~IIRPresentationTransmitter() {}
	virtual void init() = 0;
	virtual void sendMessage(const RCSPStream& stream) = 0;
	virtual void setPhysicalTransmitter(IIRTransmitter* transmitter) = 0;
};

class IRPresentationReceiverBase : public IIRPresentationReceiver
{
public:
	void setPhysicalReceiver(IIRPhysicalReceiver* receiver) {
		m_physicalReceiver = receiver;
		m_physicalReceiver->setCallback(
				[this](const uint8_t* data, uint16_t size)
				{ receiverCallback(data, size); }
		);
	}
	void setVibroEngine(IIOPin* pin) { m_vibro = pin; }

protected:
	virtual void receiverCallback(const uint8_t* data, uint16_t size) = 0;

	IIRPhysicalReceiver* m_physicalReceiver = nullptr;
	IIOPin* m_vibro = nullptr;
};

class PresentationReceiversGroupBase : public IPresentationReceiversGroup
{
public:
	void enableLoggingUnknownCommands(bool enable = true) { m_logUnknown = enable; }

protected:
	bool m_logUnknown = false;
};

class IRPresentationTransmitterBase : public IIRPresentationTransmitter
{
public:
	void setPhysicalTransmitter(IIRTransmitter* transmitter) { m_physicalTransmitter = transmitter; }

protected:
	IIRTransmitter* m_physicalTransmitter = nullptr;
};

#endif /* INCLUDE_IR_IR_PRESENTATION_HPP_ */
