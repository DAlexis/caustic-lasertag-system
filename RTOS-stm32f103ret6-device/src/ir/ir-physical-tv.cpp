/*
 * ir-physical-tv.cpp
 *
 *  Created on: 5 янв. 2016 г.
 *      Author: alexey
 */

#include "ir/ir-physical-tv.hpp"
#include "core/logging.hpp"

#include <math.h>

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
	memcpy(m_data, buffer, ceil(m_length / 8.0));
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
	resetReceiver();
	/// For debugging "true" should be replaced by "false" to disable direct call from IRQ
	m_input->setExtiCallback(std::bind(&IRReceiverTV::interruptHandler, this, std::placeholders::_1), true);
}

void IRReceiverTV::setEnabled(bool enabled)
{
		m_input->enableExti(enabled);
}

void IRReceiverTV::interrogate()
{
	Time time = systemClock->getTime();
	if (m_haveSomeData && time - m_lastTime > 2 * bitWaitPeriodMax)
	{
		// We have message and it is done
		if (m_callback)
		{
			m_callback(m_data, getCurrentLength());
		} else {
			error << "IR TV receiver callback not set!";
		}
		resetReceiver();

	}
}

int IRReceiverTV::getCurrentLength()
{
	return (m_pCurrentByte - m_data)*8 + 7-m_currentBit;
}

void IRReceiverTV::resetReceiver()
{
	memset(m_data, 0, sizeof(uint8_t)*bufferMaxSize);
	m_pCurrentByte = m_data;
	m_currentBit = 7;
	m_state = RS_WAITING_HEADER;
	m_falseImpulse = false;
	m_haveSomeData = false;

}

bool IRReceiverTV::isCorrect(unsigned int value, unsigned int min, unsigned int max)
{
	return (value > min && value < max);
}

void IRReceiverTV::saveBit(bool value)
{
	if (m_pCurrentByte - m_data == bufferMaxSize)
		return;

	m_haveSomeData = true;

	if (value)
		*m_pCurrentByte |= (1 << m_currentBit);
	else
		*m_pCurrentByte &= ~(1 << m_currentBit);

	if (m_currentBit == 0) {
		m_currentBit = 7;
		m_pCurrentByte++;
	} else
		m_currentBit--;
}

void IRReceiverTV::interruptHandler(bool state)
{
	//printf("i\n");
	Time time = systemClock->getTime();
	uint32_t lastDtimeCandidate = m_dtime;
	m_dtime = time - m_lastTime;
	// Inverted input:
	state = !state;
	if (m_debug) {
		printf("dt=%lu ",  m_dtime );
		if (state)
			printf("1\n");
		else
			printf("0\n");
	}

	if (m_dtime < 15) {
		if (m_debug) printf ("short\n");
		m_falseImpulse = true;
		return;
	}


	if (m_falseImpulse) {
		if (m_debug) printf ("short back\n");
		m_falseImpulse = false;
		return;
	}
	m_lastDtime = lastDtimeCandidate;
	m_lastTime = time;

	switch(m_state) {
	case RS_WAITING_HEADER:
		// We should have 1
		if (state == 0) {
			resetReceiver();
			return;
		}

		if (m_debug) printf ("hb \n");
		// Header beginned
		m_state = RS_HEADER_BEGINNED;
		break;

	case RS_HEADER_BEGINNED: {
			// We should have 0
			if (state != 0)
			{
				resetReceiver();
				return;
			}

			if (m_debug) printf ("he \n");
			if (isCorrect(m_dtime, headerPeriodMin, headerPeriodMax)) {
				if (m_debug) printf("ac \n");
				m_state = RS_SPACE;

			} else {
				resetReceiver();
				return;
			}
		} break;

	case RS_SPACE: {
			// We should have 1
			if (state == 0)
			{
				resetReceiver();
				return;
			}

			if (m_debug) printf ("sp \n");
			if (isCorrect(m_dtime, bitWaitPeriodMin, bitWaitPeriodMax+bitOnePeriodMax)) {
				if (m_debug) printf("ac \n");
				m_state = RS_BIT;
			} else {
				resetReceiver();
				return;
			}
		} break;

	case RS_BIT: {
			if (state != 0)
			{
				resetReceiver();
				return;
			}
			if (m_debug) printf ("b \n");
			if (isCorrect(m_dtime+m_lastDtime, bitOnePeriodMin+bitWaitPeriodMin, bitOnePeriodMax+bitWaitPeriodMax))
			{
				// We have bit "1"
				if (m_debug) printf("ac 1 \n");
				saveBit(true);
				m_state = RS_SPACE;
			} else if (isCorrect(m_dtime+m_lastDtime, bitZeroPeriodMin+bitWaitPeriodMin, bitZeroPeriodMax+bitWaitPeriodMax)){
				// We have bit "0"
				if (m_debug) printf("ac 0 \n");
				saveBit(false);
				m_state = RS_SPACE;
			} else {
				// Translation broken
				resetReceiver();
				return;
			}
		} break;
	}
}
