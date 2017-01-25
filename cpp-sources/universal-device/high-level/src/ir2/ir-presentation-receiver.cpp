#include "ir2/ir-presentation-receiver.hpp"
#include "rcsp/operation-codes.hpp"

void IRReceiverPresentationBase::connectRCSPAggregator(RCSPAggregator& aggregator)
{
	m_rcspAggregator = &aggregator;
}

void IRReceiverPresentationBase::setParser(IIRProtocolParser* parser)
{
	m_parser = parser;
}

void IRReceiverPresentationBase::addPhysicalReceiver(IIRReceiverPhysical* receiver)
{
	m_receivers.push_back(receiver);
}

void IRReceiverPresentationBase::assignReceiverToZone(UintParameter physicalReceiverId, UintParameter zoneId)
{
	m_receiverToZone[physicalReceiverId] = zoneId;
}

void IRReceiverPresentationBase::assignZoneDamageCoefficient(UintParameter zoneId, FloatParameter& damageCoefficient)
{
	m_damageCoefficient[zoneId] = &damageCoefficient;
}

void IRReceiverPresentationBase::interrogate()
{
	for (auto it : m_receivers)
	{
		it->interrogate();
		if (it->isDataReady())
		{
			uint8_t* data;
			uint16_t bitsCount;
			it->getData(data, bitsCount);
			m_parser->parse(m_parseRusult, data, bitsCount, it->getId());
			processReceivedResults(it->getId());
			m_lastMessageTime = systemClock->getTime();
		}
	}
	checkTimeout();
}

void IRReceiverPresentationBase::processReceivedResults(UintParameter receiverId)
{
	switch(m_state)
	{
	case State::empty:
		switch(m_parseRusult.type)
		{
		case IRProtocolParseResult::Type::shot:
			applyCoefficient(m_parseRusult.shot, receiverId);
			m_currentShotMessage = m_parseRusult.shot;
			m_state = State::hasShot;
			break;
		case IRProtocolParseResult::Type::command:
			m_currentCommand = m_parseRusult.commandCallback;
			m_state = State::hasCommand;
			break;
		case IRProtocolParseResult::Type::invalid:
			break;
		}
		break;
	case State::hasCommand:
		// We should ignore anything here...
		break;
	case State::hasShot:
		switch(m_parseRusult.type)
		{
		case IRProtocolParseResult::Type::shot:
			applyCoefficient(m_parseRusult.shot, receiverId);
			if (m_parseRusult.shot.damage > m_currentShotMessage.damage)
				m_currentShotMessage = m_parseRusult.shot;
			break;
		case IRProtocolParseResult::Type::command:
			// Ignore command...
			break;
		case IRProtocolParseResult::Type::invalid:
			break;
		}
		break;
	}
}

void IRReceiverPresentationBase::applyCoefficient(ShotMessage& shot, UintParameter receiverId)
{
	auto it = m_receiverToZone.find(receiverId);
	if (it == m_receiverToZone.end())
		return;
	auto jt = m_damageCoefficient.find(it->second);
	if (jt == m_damageCoefficient.end())
		return;

	shot.damage *= *(jt->second);
}

void IRReceiverPresentationBase::checkTimeout()
{
	if (m_state == State::empty)
		return;
	Time t = systemClock->getTime();
	if (t - m_lastMessageTime > timeout)
	{
		if (m_state == State::hasCommand)
		{
			if (m_currentCommand)
				m_currentCommand();
		} else if (m_state == State::hasShot)
		{
			m_rcspAggregator->doOperation(ConfigCodes::HeadSensor::Functions::catchShot, m_currentShotMessage);
		}
		m_state = State::empty;
	}
}
