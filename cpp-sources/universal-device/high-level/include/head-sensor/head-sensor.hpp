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

#include <dev/RC522-wrapper.hpp>
#include "any-device/device.hpp"
#include "any-device/any-rcsp-device-base.hpp"
#include "rcsp/state-saver.hpp"
#include "core/device-initializer.hpp"
#include "dev/RC522.hpp"
#include "game/game-log.hpp"
#include "head-sensor/player-config.hpp"
#include "head-sensor/player-state.hpp"
#include "head-sensor/head-sensor-base-types.hpp"
#include "head-sensor/resources.hpp"
#include "head-sensor/sensors-initializer.hpp"
#include "head-sensor/weapon-manager.hpp"
#include "head-sensor/hs-rifles-comunicator.hpp"
#include "sensors/smart-sensors.hpp"
#include "sensors/ir-physical-receiver.hpp"
#include "sensors/ir-presentation-receiver.hpp"
#include "sensors/kill-zones-manager.hpp"
#include "output/illumination.hpp"
#include "network/broadcast.hpp"
#include "network/network-client.hpp"
#include "rcsp/operation-codes.hpp"
#include <set>

class HeadSensor : public AnyRCSPClientDeviceBase
{
public:
	HeadSensor();
	void init(const Pinout &pinout, bool isSdcardOk) override;
	void setDefaultPinout(Pinout& pinout) override;
	bool checkPinout(const Pinout& pinout) override;

	PlayerConfiguration playerConfig{m_aggregator};
	PlayerState playerState{&playerConfig, m_aggregator};
	//DeviceParameters device;

	FUNCTION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerRespawn, m_aggregator);
	FUNCTION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerReset, m_aggregator);
	FUNCTION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerKill, m_aggregator);
	FUNCTION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, resetStats, m_aggregator);
	FUNCTION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, readStats, m_aggregator);
	FUNCTION_NP(ConfigCodes::AnyDevice::Functions, HeadSensor, resetToDefaults, m_aggregator);
	FUNCTION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, rifleToHeadSensorHeartbeat, m_aggregator);

	FUNCTION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, setTeam, m_aggregator);

	FUNCTION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, addMaxHealth, m_aggregator);

	/// Feedback when player was damaged or killed
	FUNCTION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, notifyIsDamager, m_aggregator);

	/// Replacement for receive shot callback
	FUNCTION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, catchShot, m_aggregator);

private:

	constexpr static uint32_t heartbeatPeriod = 2000000;
	constexpr static uint8_t killZonesCount = 6;
	constexpr static uint16_t RFIDWriteBufferSize = 16;

	void initSmartZones(const Pinout &pinout);

	void dieWeapons();
	void respawnWeapons();
	void notifyDamager(PlayerGameId player, uint8_t damagerTeam, uint8_t state = 0);
	void weaponWoundAndShock();
	void setFRIDToWriteAddr();

	void sendHeartbeat(DeviceAddress target);

	Time m_shockDelayBegin = 0;
	TasksPool m_tasksPool{"HStpool"};

	class TeamBroadcastTester : public Broadcast::IBroadcastTester
	{
	public:
		TeamBroadcastTester(const TYPE_OF(ConfigCodes::HeadSensor::Configuration, playerId)& teamId) :
			m_teamId(teamId)
		{}

		bool isAcceptableBroadcast(const DeviceAddress& addr);
	private:

		const TYPE_OF(ConfigCodes::HeadSensor::Configuration, playerId)& m_teamId;
	};

	// Work with sensors
	std::list<IIRReceiverPhysical*> m_receiversPhysical;
	IIRProtocolParser *m_irProtocolParser = nullptr;
	KillZonesManager m_killZonesManager;
	IRReceiversManager m_receiverMgr{m_killZonesManager};
	LedVibroManager m_ledVibroMgr{m_killZonesManager};
	IlluminationSchemesManager m_illuminationSchemes{playerConfig.teamId};
	SmartSensorsManager m_smartSensorsManager{m_ledVibroMgr, m_receiverMgr};
	SensorsInitializer m_sensorsInitializer{m_receiverMgr, m_ledVibroMgr, m_killZonesManager};

	Interrogator m_interrogator;

	// Work with weapons
	WeaponsManager m_weaponsManager;
	WeaponCommunicator m_weaponCommunicator{m_weaponsManager, *m_networkClientSender, m_networkLayer, m_aggregator};

	GameLog::BaseStatsCounter m_statsCounter{playerConfig.playerId, *m_networkClientSender};
	Stager m_taskPoolStager{"HS task pool"};
	Stager m_callbackStager{"HS callbacks"};
	RC552Frontend m_mfrcWrapper;

	IAnyDevice* m_bluetoothBridge = nullptr;
};



#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_ */
