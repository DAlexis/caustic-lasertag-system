/*
 * head-sensor.hpp
 *
 *  Created on: 24 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_

#include "head-sensor/resources.hpp"
#include "head-sensor/player-config-and-state.hpp"
#include "device/device.hpp"
#include "head-sensor/head-sensor-base-types.hpp"
#include "rcsp/operation-codes.hpp"
#include "rcsp/RCSP-state-saver.hpp"
#include "network/broadcast.hpp"
#include "game/game-log.hpp"
#include "dev/rgb-leds.hpp"
#include "dev/RC522.hpp"
#include "core/device-initializer.hpp"


#include "ir/ir-physical.hpp"
#include "ir/ir-presentation.hpp"
#include <set>

class WeaponManager : public IWeaponObresver
{
public:
	~WeaponManager();
	void assign(const DeviceAddress& addr);

	void dropAllPackages();

	void respawn();
	void die();
private:
	PackageId m_respawnPackage = 0;
	PackageId m_diePackage = 0;

	DeviceAddress m_addr;
};

class WeaponManagerFactory : public IWeaponObserverFactory
{
public:
	IWeaponObresver *create() const;
};

class HeadSensor : public IAnyDevice
{
public:
	HeadSensor();
	void init(const Pinout &pinout);
	void setDafaultPinout(Pinout& pinout);
	bool checkPinout(const Pinout& pinout);

	DeviceConfiguration deviceConfig;
	PlayerConfiguration playerConfig;
	PlayerState playerState{&playerConfig};
	//DeviceParameters device;

	FUNCION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerRespawn);
	FUNCION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerReset);
	FUNCION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerKill);
	FUNCION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, resetStats);
	FUNCION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, readStats);
	FUNCION_NP(ConfigCodes::AnyDevice::Functions, HeadSensor, resetToDefaults);

	FUNCION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, registerWeapon);
	FUNCION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, setTeam);

	FUNCION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, addMaxHealth);

	/// Feedback when player was damaged or killed
	FUNCION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, notifyIsDamager);

	/// Replacement for receive shot callback
	FUNCION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, catchShot);

private:

	constexpr static uint32_t heartbeatPeriod = 2000000;
	//constexpr static uint32_t heartbeatPeriod = 200000;
	constexpr static uint8_t killZonesCount = 6;
	// Test functions
	void testDie(const char*);

	void dieWeapons();
	void respawnWeapons();
	void turnOffWeapons();
	void notifyDamager(PlayerMT2Id player, uint8_t damagerTeam, uint8_t state = 0);
	void weaponWoundAndShock();

	void sendHeartbeat();

	WeaponManagerFactory weaponManagerFactory;

	RGBLeds m_leds{playerConfig.teamId};


	Time m_shockDelayBegin = 0;
	TasksPool m_tasksPool{"HStpool"};

	class TeamBroadcastTester : public Broadcast::IBroadcastTester
	{
	public:
		TeamBroadcastTester(const TYPE_OF(ConfigCodes::HeadSensor::Configuration, plyerMT2Id)& pId) :
			m_pId(&pId)
		{}

		bool isAcceptableBroadcast(const DeviceAddress& addr);
	private:

		const TYPE_OF(ConfigCodes::HeadSensor::Configuration, plyerMT2Id)* m_pId;
	};

	IIRPhysicalReceiver* m_irPhysicalReceivers[killZonesCount];
	IIRPresentationReceiver* m_irPresentationReceivers[killZonesCount];
	IPresentationReceiversGroup* m_irPresentationReceiversGroup = nullptr;

	Interrogator m_killZonesInterogator;

	GameLog::BaseStatsCounter m_statsCounter;
	Stager m_taskPoolStager{"HS task pool"};
	Stager m_callbackStager{"HS callbacks"};
	MFRC522 m_mfrc;
};



#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_ */
