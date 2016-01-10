/*
 * ir-presentation.hpp
 *
 *  Created on: 8 янв. 2016 г.
 *      Author: alexey
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

class IRPresentationTransmitterBase : public IIRPresentationTransmitter
{
public:
	void setPhysicalTransmitter(IIRTransmitter* transmitter) { m_physicalTransmitter = transmitter; }

protected:
	IIRTransmitter* m_physicalTransmitter = nullptr;
};

#endif /* INCLUDE_IR_IR_PRESENTATION_HPP_ */
