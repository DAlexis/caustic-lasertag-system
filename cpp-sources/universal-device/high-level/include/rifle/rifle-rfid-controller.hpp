/*
 * rifle-rfid-controller.hpp
 *
 *  Created on: 7 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_RIFLE_RIFLE_RFID_CONTROLLER_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_RIFLE_RIFLE_RFID_CONTROLLER_HPP_

#include "dev/RC522-wrapper.hpp"
#include "utils/interfaces.hpp"
#include "rcsp/aggregator.hpp"
#include "hal/system-clock.hpp"
#include "network/network-base-types.hpp"

#include <functional>

class RifleRFIDController : public IInterrogatable
{
public:
	enum class Mode {
		nothing = 0,
		readCommandFromCard,
		writeHSAddressToCard,
		writeServiceCard,
	};

	using SingleOperationDoneCallback = std::function<void(Mode lastMode)>;

	RifleRFIDController(RC552Frontend &rfid, RCSPAggregator &aggregator);

	void interrogate() override;

	void setTimeout(Time timeout);

	void setCallback(SingleOperationDoneCallback callback);

	void readCommands();
	void writeServiceCard();
	void writeHSAddress(DeviceAddress addr);

private:
	constexpr static uint16_t RFIDWriteBufferSize = 16;

	void updateTimeout();
	void onCardReaded(uint8_t* buffer, uint16_t size);

	RC552Frontend& m_rfid;
	RCSPAggregator &m_aggregator;
	SingleOperationDoneCallback m_callback = nullptr;
	Mode m_mode = Mode::nothing;

	uint8_t m_RFIDWriteBuffer[RFIDWriteBufferSize];

	Time m_lastOperationTime = 0;
	Time m_timeout = 10000000;
};







#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_RIFLE_RIFLE_RFID_CONTROLLER_HPP_ */
