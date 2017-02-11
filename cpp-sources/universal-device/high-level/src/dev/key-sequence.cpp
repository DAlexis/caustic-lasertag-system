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

#include "dev/key-sequence.hpp"
#include "core/logging.hpp"

void KeySequenceDetector::addKeyToSequence(ButtonManager* pin)
{
	m_seq.push_back(pin);
	reset();
}

void KeySequenceDetector::interrogate()
{
	if (m_seq.empty())
		return;
	Time now = systemClock->getTime();

	if (now - m_lastChange < minDelay) // Anti-shock
		return;

	if (m_lastButtonState == false && (*m_it)->state() == true)
	{
		debug << "n->p";
		// user pressed key
		m_lastButtonState = true;
		m_lastChange = now;
		return;
	}
	if (m_lastButtonState == true && (*m_it)->state() == false)
	{
		debug << "p->n";
		// user released key
		m_lastButtonState = false;
		m_lastChange = now;
		m_it++;
		if (m_it == m_seq.end())
		{
			debug << "finish";
			if (m_callback)
				m_callback();
			reset();
		}
		return;
	}

	if (now - m_lastChange > maxDelay)
		reset();
}

void KeySequenceDetector::setCallback(SequenceDoneCallback callback)
{
	m_callback = callback;
}

void KeySequenceDetector::reset()
{
	m_it = m_seq.begin();
	m_lastButtonState = false;
}

bool KeySequenceDetector::isSomethingReceived()
{
	return !(m_it == m_seq.begin() && m_lastButtonState == false);
}
