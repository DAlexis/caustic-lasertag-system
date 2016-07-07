/*
 * ir-physical-tv.hpp
 *
 *  Created on: 5 янв. 2016 г.
 *      Author: alexey
 */

#ifndef INCLUDE_IR_IR_PHYSICAL_TV_HPP_
#define INCLUDE_IR_IR_PHYSICAL_TV_HPP_

#include "ir/ir-physical.hpp"
#include "hal/system-clock.hpp"

class IRTransmitterTV : public IRTransmitterBase
{
public:
	IRTransmitterTV();
	~IRTransmitterTV();
	void init();
	void send(const uint8_t* buffer, uint16_t size);
	bool busy();

private:
	constexpr static int bufferMaxSize = 100;

	constexpr static int headerPeriod = 2400;
	constexpr static int bitOnePeriod = 1200;
	constexpr static int bitZeroPeriod = 600;
	constexpr static int bitWaitPeriod = 600;

	bool m_sendingHeader = false;
	uint8_t *m_pCurrentByte;
	uint8_t m_currentBit;
	uint8_t m_data[bufferMaxSize];
	uint16_t m_currentLength;
	/// User message length
	uint16_t m_length = 0;

	bool m_busy = false;

	bool nextBit();
	void cursorToStart();
	void emitterCallback(bool wasOnState);
};

class IRReceiverTV : public IRReceiverBase
{
public:
	~IRReceiverTV() {}

	void init();
	void setEnabled(bool enabled);
	void interrogate();

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

	uint8_t m_data[bufferMaxSize];
	uint8_t *m_pCurrentByte;
	uint8_t m_currentBit;
	bool m_falseImpulse;
	ReceivingState m_state;

	Time m_lastTime = 0;
	uint32_t m_dtime = 0;
	uint32_t m_lastDtime = 0;

	bool m_haveSomeData = false;

	bool m_debug = false;


	void resetReceiver();
	bool isCorrect(unsigned int value, unsigned int min, unsigned int max);
	void saveBit(bool value);
	int getCurrentLength();

	void interruptHandler(bool state);
};




#endif /* INCLUDE_IR_IR_PHYSICAL_TV_HPP_ */
