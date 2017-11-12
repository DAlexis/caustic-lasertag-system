#include "rifle/rifle-rfid-controller.hpp"
#include "rcsp/operation-codes.hpp"
#include "core/logging.hpp"

RifleRFIDController::RifleRFIDController(RC552Frontend& rfid, RCSPAggregator &aggregator) :
	m_rfid(rfid), m_aggregator(aggregator)
{
}

void RifleRFIDController::interrogate()
{
	if (m_mode != Mode::readCommandFromCard && systemClock->getTime() - m_lastOperationTime > m_timeout)
	{
		m_mode = Mode::nothing;
		if (m_callback)
			m_callback(m_mode);
	}
	m_rfid.interrogate();
}

void RifleRFIDController::setTimeout(Time timeout)
{
	m_timeout = timeout;
}


void RifleRFIDController::setCallback(SingleOperationDoneCallback callback)
{
	m_callback = callback;
}

void RifleRFIDController::readCommands()
{
	m_mode = Mode::readCommandFromCard;

	m_rfid.readBlock(
		[this](uint8_t* data, uint16_t size)
			{ onCardReaded(data, size); },
		18
	);
}

void RifleRFIDController::writeServiceCard()
{
	/**
	 *  @Todo: When reading programmed card, it produce
	 *  [warn ]  Unknown request code: 6340
	 *
	 *  due to card containment
	 *  0 e6 0 0 0 0 0 0 0 0 0 0 0 0 0 0 c4 18
	 *
	 *  That is strange.
	 */
	updateTimeout();
	m_mode = Mode::writeServiceCard;

	Buffer buf(RFIDWriteBufferSize, 0);
	RCSPAggregator::serializeCall(
		ConfigCodes::Rifle::Functions::rifleRFIDProgramHSAddr,
		buf
	);
	m_rfid.writeBlock(
		buf.data(),
		buf.size(),
		[this](uint8_t* data, uint16_t size)
		{
			UNUSED_ARG(data); UNUSED_ARG(size);
			if (m_callback)
				m_callback(m_mode);
		}
	);
}

void RifleRFIDController::writeHSAddress(DeviceAddress addr)
{
	updateTimeout();
	m_mode = Mode::writeHSAddressToCard;
	Buffer buf(RFIDWriteBufferSize, 0);

	RCSPAggregator::serializeCall(
		ConfigCodes::Rifle::Functions::rifleChangeHS,
		buf,
		addr
	);
	m_rfid.writeBlock(
		buf.data(),
		buf.size(),
		[this](uint8_t* data, uint16_t size)
		{
			UNUSED_ARG(data); UNUSED_ARG(size);
			if (m_callback)
				m_callback(m_mode);
		}
	);
}


void RifleRFIDController::updateTimeout()
{
	m_lastOperationTime = systemClock->getTime();
}

void RifleRFIDController::onCardReaded(uint8_t* buffer, uint16_t size)
{
	if (m_mode != Mode::readCommandFromCard)
		return;

	info << "Processing data from RFID card...";
	debug << "data: " << hexStr(buffer, size);

	m_callback(m_mode);
	m_aggregator.dispatchStreamNew(buffer, size);
}
