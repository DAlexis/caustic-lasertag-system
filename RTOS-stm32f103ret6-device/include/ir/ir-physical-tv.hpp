/*
 * ir-physical-tv.hpp
 *
 *  Created on: 5 янв. 2016 г.
 *      Author: alexey
 */

#ifndef INCLUDE_IR_IR_PHYSICAL_TV_HPP_
#define INCLUDE_IR_IR_PHYSICAL_TV_HPP_

#include "ir/ir-physical.hpp"

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
	void interrogate();
private:
	constexpr static int bufferMaxSize = 100;
	uint8_t m_buffer[bufferMaxSize];
};




#endif /* INCLUDE_IR_IR_PHYSICAL_TV_HPP_ */
