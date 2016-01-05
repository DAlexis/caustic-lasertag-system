/*
 * ir-physical-tv.cpp
 *
 *  Created on: 5 янв. 2016 г.
 *      Author: alexey
 */

#include "ir/ir-physical-tv.hpp"
#include "core/logging.hpp"

//////////////////////////
/// Transmitter

IRTransmitterTV::IRTransmitterTV()
{
	cursorToStart();
}

void IRTransmitterTV::init()
{
	m_emitter->setCallback(std::bind(&IRTransmitterTV::emitterCallback, this, std::placeholders::_1));
}

void IRTransmitterTV::send(const uint8_t* buffer, uint16_t size)
{
	if (size > bufferMaxSize)
	{
		error << "Too long message for TV transmitter";
		return;
	}

	m_length = size;
	memcpy(m_data, buffer, m_length);
	m_busy = true;
	m_emitter->setPower(*m_power);
	cursorToStart();
	m_sendingHeader = true;
	m_emitter->startImpulsePack(true, headerPeriod);
}

bool IRTransmitterTV::busy()
{
	return m_busy;
}

void IRTransmitterTV::cursorToStart()
{
	m_currentLength = 0;
	m_currentBit = 7;
	m_pCurrentByte = m_data;
}

bool IRTransmitterTV::nextBit()
{
    bool result = (*m_pCurrentByte & (1 << m_currentBit)) != 0;
    m_currentLength++;
    if (m_currentBit == 0) {
        m_currentBit = 7;
        m_pCurrentByte++;
    } else
        m_currentBit--;
    return result;
}

void IRTransmitterTV::emitterCallback(bool wasOnState)
{
	if (m_sendingHeader)
	{
		m_sendingHeader = false;
		m_emitter->startImpulsePack(false, bitWaitPeriod);
		return;
	}

	if (wasOnState)
	{
		if (m_currentLength == m_length)
		{
			m_busy = false;
			return;
		}
		m_emitter->startImpulsePack(false, bitWaitPeriod);
	} else {
		if (nextBit())
			m_emitter->startImpulsePack(true, bitOnePeriod);
		else
			m_emitter->startImpulsePack(true, bitZeroPeriod);
	}
}

//////////////////////////
/// Receiver

void IRReceiverTV::init()
{
}

void IRReceiverTV::interrogate()
{
}
