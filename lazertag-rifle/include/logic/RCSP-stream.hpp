/*
 * RCSP-stream.hpp
 *
 *  Created on: 09 февр. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_RCSP_STREAM_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_RCSP_STREAM_HPP_

#include "logic/RCSP-aggregator.hpp"
#include "logic/RCSP-modem.hpp"

class RCSPStream
{
public:
	RCSPStream(uint16_t size);
	~RCSPStream();
	uint8_t* getStream();
	uint16_t getSize();
	bool addValue(OperationCode code);
	bool addRequest(OperationCode code);
	bool addCall(OperationCode code);

	template<typename Type>
	bool addCall(OperationCode code, const Type& arg)
	{
		printf("Adding value, code %u", code);
		return serializeAnything(
			code,
			[this, &arg] (uint8_t *pos, OperationCode code, uint16_t &addedSize) -> RCSPAggregator::ResultType
			{
				return RCSPAggregator::instance().functionCall(pos, code, m_size - m_cursor, addedSize, arg);
			}
		);
	}

	uint16_t send(DeviceAddress target, bool waitForAck = false, PackageSendingDoneCallback doneCallback = nullptr);

	bool empty();
private:

	using SerializationFunc = std::function<RCSPAggregator::ResultType (uint8_t * /*pos*/, OperationCode /*code*/, uint16_t & /*addedSize*/)>;
	bool serializeAnything(OperationCode code, SerializationFunc serializer);
	uint8_t* m_stream = nullptr;
	uint16_t m_cursor = 0;
	uint16_t m_freeSpace = 0;
	uint16_t m_size = 0;
};




#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RCSP_STREAM_HPP_ */
