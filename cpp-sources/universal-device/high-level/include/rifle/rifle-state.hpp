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
	RifleState(RifleConfiguration* config);
	void reset();
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::State, bulletsInMagazineCurrent);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::State, magazinesCountCurrent);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::State, heatnessCurrent);

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
	        OrdinaryNetworkClient* networkClient,
	        RCSPAggregator* aggregator
	        );
	void syncAll();
	void print();
	bool isAlive();

	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::Configuration, healthMax);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::Configuration, armorMax);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, healthCurrent);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, armorCurrent);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, lifesCountCurrent);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, pointsCount);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, killsCount);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, deathsCount);

private:
	const DeviceAddress* m_headSensorAddress;
	OrdinaryNetworkClient* m_networkClient;
	RCSPAggregator* m_aggregator;
};

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_RIFLE_RIFLE_STATE_HPP_ */
