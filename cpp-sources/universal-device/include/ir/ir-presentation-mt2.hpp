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


#ifndef INCLUDE_IR_IR_PRESENTATION_MT2_HPP_
#define INCLUDE_IR_IR_PRESENTATION_MT2_HPP_

#include "ir/ir-presentation.hpp"
#include "game/game-base-types.hpp"

class PresentationReceiversGroupMT2;

using OperationCallback = std::function<void(void)>;

class IRPresentationTransmitterMT2 : public IRPresentationTransmitterBase
{
public:
	IRPresentationTransmitterMT2(RCSPAggregator& rcspAggregator);
	void init();
	void sendMessage(const RCSPStream& stream);

private:
	constexpr static uint32_t maxMessageSize = 100;
	void sendGenericRCSPMessageDefault(const RCSPStream& stream);

	void sendCommand(uint8_t commandCode);
	uint8_t encodeDamage(uint8_t damage);

	uint8_t m_data[maxMessageSize];
	RCSPAggregator& m_rcspAggregator;
};

class IRPresentationReceiverMT2 : public IRPresentationReceiverBase
{
public:
	IRPresentationReceiverMT2(RCSPAggregator& rcspAggregator);
	void init();
	void assignReceiversGroup(IPresentationReceiversGroup& group);
	void interrogate();
	void setDamageCoefficient(const FloatParameter* coefficient);

private:
	constexpr static uint32_t vibroPeriod = 200000;
	void receiverCallback(const uint8_t* data, uint16_t size);
	void parseShot(const uint8_t* data, uint16_t size);
	void parseMT2Command(const uint8_t* data, uint16_t size);
	void parseSetTeam(const uint8_t* data, uint16_t size);
	void parseAddHealth(const uint8_t* data, uint16_t size);
	void parseRCSP(const uint8_t* data, uint16_t size);
	void enableVibro();


	uint8_t decodeDamage(uint8_t damage);
	void updateOperationCallback(OperationCallback callback);

	PresentationReceiversGroupMT2* m_group = nullptr;
	static FloatParameter defaultDamageCoefficient;
	const FloatParameter* m_damageCoefficient = &defaultDamageCoefficient;
	Time m_vibroEnabledTime = 0;
	RCSPAggregator& m_rcspAggregator;
};

class PresentationReceiversGroupMT2 : public PresentationReceiversGroupBase
{
friend IRPresentationReceiverMT2;
public:
	void connectReceiver(IIRPresentationReceiver& receiver);
	void interrogate();

private:
	constexpr static uint32_t messageParsingDelay = 3000;
	constexpr static uint16_t argumentBufferSize = 100;
	std::list<IRPresentationReceiverMT2*> m_receivers;

	bool m_hasWaitingShot = false;

	uint8_t m_bufferForArgument[argumentBufferSize];

	OperationCallback m_operation = nullptr;
	UintParameter m_maxDamage = 0;
	ShotMessage m_shotMessage;

	Time m_lastReceivedMessage = 0;
};


#endif /* INCLUDE_IR_IR_PRESENTATION_MT2_HPP_ */
