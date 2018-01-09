/*
 * rifle-state.hpp
 *
 *  Created on: 7 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_RIFLE_RIFLE_STATE_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_RIFLE_RIFLE_STATE_HPP_

#include "rifle-config.hpp"

struct RFIDProgrammingState
{
	enum class Mode {
		disabled,
		programHSAddr,
		programMasterCard
	};

	Mode mode = Mode::disabled;

	bool enabled() { return mode != Mode::disabled; }
};

class RifleState
{
public:
	RifleState(RifleConfiguration* config, RCSPAggregator& aggregator);
	void reset();

private:
	RCSPAggregator& m_aggregator;

public:

	PAR_ST(RESTORABLE, ConfigCodes::Rifle::State, bulletsInMagazineCurrent, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::State, magazinesCountCurrent, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::State, heatnessCurrent, m_aggregator);

	//RFIDProgrammingState rfidProgrammingState;

	uint32_t lastReloadTime;
	bool isEnabled = false;
	bool isHSConnected = false;

	enum class RFIDSate {
		reading,
		programHSAddr,
		programMasterCard
	};

	RFIDSate rfidState = RFIDSate::reading;

	const RifleConfiguration& config() const { return *m_config; }

private:
	RifleConfiguration* m_config = nullptr;
};


class PlayerPartialState
{
public:
	PlayerPartialState(
	        const DeviceAddress& headSensorAddress,
	        INetworkClientSender& networkClient,
	        RCSPAggregator* aggregator
	        );
	void syncAll();
	void print();
	bool isAlive();
private:
	RCSPAggregator* m_aggregator;

public:

	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::Configuration, healthMax, *m_aggregator);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::Configuration, armorMax, *m_aggregator);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, healthCurrent, *m_aggregator);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, armorCurrent, *m_aggregator);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, lifesCountCurrent, *m_aggregator);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, deathsCount, *m_aggregator);

private:
	const DeviceAddress* m_headSensorAddress;
	INetworkClientSender& m_networkClientSender;
};

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_RIFLE_RIFLE_STATE_HPP_ */
