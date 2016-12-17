/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_

#include "rcsp/state-saver.hpp"
#include "core/device-initializer.hpp"
#include "dev/MFRC522-wrapper.hpp"
#include "dev/RC522.hpp"
#include "dev/rgb-leds.hpp"
#include "device/device.hpp"
#include "game/game-log.hpp"
#include "head-sensor/head-sensor-base-types.hpp"
#include "head-sensor/player-config-and-state.hpp"
#include "head-sensor/resources.hpp"
#include "ir/ir-physical.hpp"
#include "ir/ir-presentation.hpp"
#include "network/broadcast.hpp"
#include "network/network-client.hpp"
#include "rcsp/operation-codes.hpp"
#include <set>

class WeaponManager : public IWeaponObresver
{
public:
	~WeaponManager();
	void assign(const DeviceAddress& addr);

	void dropAllPackages();

	void respawn(INetworkClient* client);
	void die(INetworkClient* client);
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
	void init(const Pinout &pinout, bool isSdcardOk)  override;
	void setDefaultPinout(Pinout& pinout) override;
	bool checkPinout(const Pinout& pinout) override;

	DeviceConfiguration deviceConfig;
	PlayerConfiguration playerConfig;
	PlayerState playerState{&playerConfig};
	//DeviceParameters device;

	FUNCTION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerRespawn);
	FUNCTION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerReset);
	FUNCTION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerKill);
	FUNCTION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, resetStats);
	FUNCTION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, readStats);
	FUNCTION_NP(ConfigCodes::AnyDevice::Functions, HeadSensor, resetToDefaults);

	FUNCTION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, registerWeapon);
	FUNCTION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, deregisterWeapon);
	FUNCTION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, setTeam);

	FUNCTION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, addMaxHealth);

	/// Feedback when player was damaged or killed
	FUNCTION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, notifyIsDamager);

	/// Replacement for receive shot callback
	FUNCTION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, catchShot);

private:

	constexpr static uint32_t heartbeatPeriod = 2000000;
	//constexpr static uint32_t heartbeatPeriod = 200000;
	constexpr static uint8_t killZonesCount = 6;
	constexpr static uint16_t RFIDWriteBufferSize = 16;

	// Test functions
	void testDie(const char*);

	void dieWeapons();
	void respawnWeapons();
	void turnOffWeapons();
	void notifyDamager(PlayerGameId player, uint8_t damagerTeam, uint8_t state = 0);
	void weaponWoundAndShock();
	void setFRIDToWriteAddr();

	void sendHeartbeat();

	WeaponManagerFactory weaponManagerFactory;

	RGBLeds m_leds{playerConfig.teamId};


	Time m_shockDelayBegin = 0;
	TasksPool m_tasksPool{"HStpool"};

	class TeamBroadcastTester : public Broadcast::IBroadcastTester
	{
	public:
		TeamBroadcastTester(const TYPE_OF(ConfigCodes::HeadSensor::Configuration, playerId)& pId) :
			m_pId(&pId)
		{}

		bool isAcceptableBroadcast(const DeviceAddress& addr);
	private:

		const TYPE_OF(ConfigCodes::HeadSensor::Configuration, playerId)* m_pId;
	};

	OrdinaryNetworkClient m_networkClient;
	RCSPNetworkListener m_networkPackagesListener;

	IIRPhysicalReceiver* m_irPhysicalReceivers[killZonesCount];
	IIRPresentationReceiver* m_irPresentationReceivers[killZonesCount];
	IPresentationReceiversGroup* m_irPresentationReceiversGroup = nullptr;

	Interrogator m_killZonesInterogator;

	GameLog::BaseStatsCounter m_statsCounter{playerConfig.playerId, &m_networkClient};
	Stager m_taskPoolStager{"HS task pool"};
	Stager m_callbackStager{"HS callbacks"};
	RC552Wrapper m_mfrcWrapper;

	uint8_t m_RFIDWriteBuffer[RFIDWriteBufferSize];
};



#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_ */
