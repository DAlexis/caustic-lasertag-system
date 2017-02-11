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

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_DEV_KEY_SEQUENCE_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_DEV_KEY_SEQUENCE_HPP_

#include "dev/buttons.hpp"
#include "hal/system-clock.hpp"
#include "utils/interfaces.hpp"

#include <list>
#include <functional>
#include <stdint.h>

class KeySequenceDetector : public IInterrogatable
{
public:
	using SequenceDoneCallback = std::function<void(void)>;
	constexpr static uint32_t minDelay = 1000;
	constexpr static uint32_t maxDelay = 1000000;

	void addKeyToSequence(ButtonManager* pin);
	void interrogate();
	void setCallback(SequenceDoneCallback callback);

private:
	void reset();
	bool isSomethingReceived();

	std::list<ButtonManager*> m_seq;
	std::list<ButtonManager*>::iterator m_it{m_seq.end()};
	bool m_lastButtonState = true;
	Time m_lastChange = 0;
	SequenceDoneCallback m_callback = nullptr;
};






#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_DEV_KEY_SEQUENCE_HPP_ */
