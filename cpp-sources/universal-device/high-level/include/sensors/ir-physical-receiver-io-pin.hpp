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

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_IR_PHYSICAL_RECEIVER_IO_PIN_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_IR_PHYSICAL_RECEIVER_IO_PIN_HPP_

#include "sensors/ir-physical-receiver.hpp"
#include "hal/io-pins.hpp"
#include "hal/system-clock.hpp"

class IRReceiverPhysicalIOPin : public IIRReceiverPhysical
{
public:
	IRReceiverPhysicalIOPin(IIOPin* input, UintParameter id);

	void init();
	void getData(uint8_t*& data, uint16_t& size) override;
	UintParameter getId() override;
	bool isDataReady() override;
	void setEnabled(bool enabled) override;
	void interrogate() override;

private:
	enum ReceivingState
	{
		RS_WAITING_HEADER = 0,
		RS_HEADER_BEGINNED = 1,
		RS_SPACE = 2,
		RS_BIT = 3
	};

	constexpr static int bufferMaxSize = 100;

	// Receiver constants
	constexpr static int headerPeriodMin     = 1700;
	constexpr static int headerPeriodMax     = 2600;

	constexpr static int bitOnePeriodMin    = 1100;
	constexpr static int bitOnePeriodMax    = 1500;

	constexpr static int bitZeroPeriodMin   = 500;
	constexpr static int bitZeroPeriodMax   = 750;

	constexpr static int bitWaitPeriodMin   = 500;
	constexpr static int bitWaitPeriodMax   = 900;

	void resetReceiver();
	bool isCorrect(unsigned int value, unsigned int min, unsigned int max);
	void saveBit(bool value);
	int getCurrentLength();

	void interruptHandler(bool state);

	uint8_t m_data[bufferMaxSize];
	uint8_t *m_pCurrentByte = nullptr;
	uint8_t m_currentBit = 7;
	bool m_falseImpulse = false;
	ReceivingState m_state = RS_WAITING_HEADER;

	Time m_lastTime = 0;
	uint32_t m_dtime = 0;
	uint32_t m_lastDtime = 0;

	uint8_t m_dataForUser[bufferMaxSize];
	uint16_t m_bitsCountForUser = 0;

	bool m_haveSomeData = false;

	bool m_debug = false;
	bool m_dataReady = false;

	IIOPin* m_input = nullptr;

	UintParameter m_id;
};




#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_IR_PHYSICAL_RECEIVER_IO_PIN_HPP_ */
