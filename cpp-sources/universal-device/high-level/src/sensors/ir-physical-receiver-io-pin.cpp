/*
*    Copyright (C) 2017 by Aleksey Bulatov
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

#include "sensors/ir-physical-receiver-io-pin.hpp"

#include <string.h>

IRReceiverPhysicalIOPin::IRReceiverPhysicalIOPin(IIOPin* input, UintParameter id) :
	m_input(input),
	m_id(id)
{
}

void IRReceiverPhysicalIOPin::init()
{
	resetReceiver();
	/// For debugging "true" should be replaced by "false" to disable direct call from IRQ
	m_input->switchToInput();
	m_input->setExtiCallback(std::bind(&IRReceiverPhysicalIOPin::interruptHandler, this, std::placeholders::_1), true);
	setEnabled(true);
}

void IRReceiverPhysicalIOPin::setEnabled(bool enabled)
{
	m_input->enableExti(enabled);
}

bool IRReceiverPhysicalIOPin::isDataReady()
{
	return m_dataReady;
}

void IRReceiverPhysicalIOPin::getData(uint8_t*& data, uint16_t& size)
{
	if (m_dataReady)
	{
		data = m_dataForUser;
		size = m_bitsCountForUser;
		m_dataReady = false;
	} else {
		data = nullptr;
		size = 0;
	}
}

UintParameter IRReceiverPhysicalIOPin::getId()
{
	return m_id;
}

void IRReceiverPhysicalIOPin::interrogate()
{
	Time time = systemClock->getTime();
	if (m_haveSomeData && time - m_lastTime > 2 * bitWaitPeriodMax)
	{
		memcpy(m_dataForUser, m_data, bufferMaxSize);
		m_bitsCountForUser = getCurrentLength();
		m_dataReady = true;
		resetReceiver();
	}
}

int IRReceiverPhysicalIOPin::getCurrentLength()
{
	return (m_pCurrentByte - m_data)*8 + 7-m_currentBit;
}

void IRReceiverPhysicalIOPin::resetReceiver()
{
	memset(m_data, 0, sizeof(uint8_t)*bufferMaxSize);
	m_pCurrentByte = m_data;
	m_currentBit = 7;
	m_state = RS_WAITING_HEADER;
	m_falseImpulse = false;
	m_haveSomeData = false;
}

bool IRReceiverPhysicalIOPin::isCorrect(unsigned int value, unsigned int min, unsigned int max)
{
	return (value > min && value < max);
}

void IRReceiverPhysicalIOPin::saveBit(bool value)
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

void IRReceiverPhysicalIOPin::interruptHandler(bool state)
{
	if (m_debug)
	    printf("i\n");
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
