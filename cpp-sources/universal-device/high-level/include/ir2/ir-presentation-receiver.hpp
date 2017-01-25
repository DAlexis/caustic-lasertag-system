
#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_IR_PRESENTATION_RECEIVER_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_IR_PRESENTATION_RECEIVER_HPP_

#include "ir2/ir-physical-receiver.hpp"
#include "rcsp/base-types.hpp"
#include "game/game-base-types.hpp"
#include "hal/system-clock.hpp"
#include "rcsp/aggregator.hpp"

#include <map>
#include <list>

struct IRProtocolParseResult
{
	enum class Type {
		command,
		shot,
		invalid
	};
	Type type;
	ShotMessage shot;
	std::function<void(void)> commandCallback = nullptr;
};

class IIRProtocolParser
{
public:
	virtual ~IIRProtocolParser() {}
	virtual void parse(IRProtocolParseResult& result, uint8_t* data, uint16_t bitsCount, UintParameter sensorId) = 0;
};

class IRReceiverPresentationBase : public IInterrogatable
{
public:
	void connectRCSPAggregator(RCSPAggregator& aggregator);
	void setParser(IIRProtocolParser* parser);
	void addPhysicalReceiver(IIRReceiverPhysical* receiver);
	void assignReceiverToZone(UintParameter physicalReceiverId, UintParameter zoneId);
	void assignZoneDamageCoefficient(UintParameter zoneId, FloatParameter& damageCoefficient);
	void interrogate() override;

private:
	constexpr static uint32_t timeout = 5000;
	enum class State {
		empty,
		hasShot,
		hasCommand
	};

	void processReceivedResults(UintParameter receiverId);
	void applyCoefficient(ShotMessage& shot, UintParameter receiverId);
	void checkTimeout();

	std::list<IIRReceiverPhysical*> m_receivers;
	std::map<UintParameter, UintParameter> m_receiverToZone;
	std::map<UintParameter, FloatParameter*> m_damageCoefficient;

	IIRProtocolParser* m_parser = nullptr;

	Time m_lastMessageTime;
	State m_state = State::empty;

	ShotMessage m_currentShotMessage;
	std::function<void(void)> m_currentCommand = nullptr;

	IRProtocolParseResult m_parseRusult;

	RCSPAggregator* m_rcspAggregator = nullptr;
};


#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_IR_PRESENTATION_RECEIVER_HPP_ */
