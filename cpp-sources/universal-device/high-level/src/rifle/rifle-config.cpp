#include "rifle/rifle-config.hpp"
#include "rifle/rifle-state.hpp"
#include "rcsp/stream.hpp"


PlayerPartialState::PlayerPartialState(
        const DeviceAddress& headSensorAddress,
		INetworkClient* networkClient,
        RCSPAggregator* aggregator
        ) :
	m_aggregator(aggregator),
	m_headSensorAddress(&headSensorAddress),
	m_networkClient(networkClient)
{
	healthMax = 0;
	armorMax = 0;

	healthCurrent = 0;
	armorCurrent = 0;

	lifesCountCurrent = 0;
	pointsCount = 0;
	killsCount = 0;
	deathsCount = 0;
}

void PlayerPartialState::syncAll()
{
	RCSPStreamNew stream(m_aggregator);
	stream.addPull(ConfigCodes::HeadSensor::Configuration::healthMax);
	stream.addPull(ConfigCodes::HeadSensor::Configuration::armorMax);
	stream.addPull(ConfigCodes::HeadSensor::State::healthCurrent);
	stream.addPull(ConfigCodes::HeadSensor::State::armorCurrent);
	stream.addPull(ConfigCodes::HeadSensor::State::lifesCountCurrent);
	stream.addPull(ConfigCodes::HeadSensor::State::pointsCount);
	stream.addPull(ConfigCodes::HeadSensor::State::killsCount);
	stream.addPull(ConfigCodes::HeadSensor::State::deathsCount);

	stream.send(m_networkClient, *m_headSensorAddress, false);
}

void PlayerPartialState::print()
{
	printf("\nCurrent player's state:\n");
	constexpr uint8_t barLength = 10;
	if (healthMax != 0)
	{
		printf("Health:  ");
		printBar(barLength, barLength * healthCurrent / healthMax);
		printf(" %u/%u\n", healthCurrent, healthMax);
	}
	if (armorMax != 0)
	{
		printf("Armor:   ");
		printBar(barLength, barLength * armorCurrent / armorMax);
		printf(" %u/%u\n", armorCurrent, armorMax);
	}
	printf("Lifes:  %u\n", lifesCountCurrent);
	printf("Points: %u\n", pointsCount);
	printf("Kills:  %u\n", killsCount);
	printf("Deaths: %u\n", deathsCount);
}

bool PlayerPartialState::isAlive()
{
	return healthCurrent > 0;
}

RifleConfiguration::RifleConfiguration(RCSPAggregator& aggregator) :
		m_aggregator(aggregator)
{
	setDefault();
}

bool RifleConfiguration::isAutoReloading()
{
	return (!reloadIsMagazineSmart
			&& !reloadNeedMagDisconnect
			&& !reloadNeedMagChange
			&& !reloadNeedBolt);
}

bool RifleConfiguration::isReloadingByDistortingTheBolt()
{
	return (!reloadNeedMagDisconnect
			&& !reloadNeedMagChange
			&& reloadNeedBolt);
}

void RifleConfiguration::setDefault()
{
	slot = 1;
	weightInSlot = 0;

	damageMin = 25;
	damageMax = 25;
	firePeriod = 100000;
	shotDelay = 0;

	jamProb = 0;
	criticalProb = 0;
	criticalCoeff = 1;

	semiAutomaticAllowed = true;
	automaticAllowed = true;

	reloadIsMagazineSmart = false;
	reloadNeedMagDisconnect = false;
	reloadNeedMagChange = false;
	reloadNeedBolt = false;
	reloadPlaySound = true;

	magazinesCountMax = 10;
	magazinesCountStart = magazinesCountMax;

	bulletsInMagazineMax = 30;
	bulletsInMagazineStart = bulletsInMagazineMax;
	reloadingTime = 3000000;

	heatPerShot = 0;
	heatLossPerSec = 0;

	fireFlashPeriod = 100000;
	fireVibroPeriod = 100000;

	outputPower = 100;
}

RifleState::RifleState(RifleConfiguration* config, RCSPAggregator& aggregator) :
	m_aggregator(aggregator),
	m_config(config)
{
	if (config)
		reset();
}

void RifleState::reset()
{
	bulletsInMagazineCurrent = m_config->bulletsInMagazineStart;
	magazinesCountCurrent = m_config->magazinesCountStart;
	heatnessCurrent = 0;
	lastReloadTime = 0;
}
