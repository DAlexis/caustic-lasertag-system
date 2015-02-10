/*
 * RCSP-stream.hpp
 *
 *  Created on: 09 февр. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_RCSP_STREAM_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_RCSP_STREAM_HPP_

#include "logic/RCSP-aggregator.hpp"

class RCSPStream
{
public:
	RCSPStream(uint16_t size);
	~RCSPStream();
	uint8_t* getStream();
	uint16_t getSize();
	bool add(OperationCode code, bool needArgumentLookup = true);
	bool empty();

private:
	uint8_t* m_stream = nullptr;
	uint16_t m_cursor = 0;
	uint16_t m_size = 0;
};




#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RCSP_STREAM_HPP_ */
