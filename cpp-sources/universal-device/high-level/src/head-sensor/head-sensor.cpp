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


#include "core/device-initializer.hpp"
#include "core/diagnostic.hpp"
#include "core/logging.hpp"
#include "core/os-wrappers.hpp"
#include "core/power-monitor.hpp"
#include "dev/nrf24l01.hpp"
#include "bluetooth-bridge/bluetooth-bridge.hpp"
#include "head-sensor/head-sensor.hpp"
#include "head-sensor/resources.hpp"
#include "sensors/ir-protocol-parser-MT2-ex.hpp"
#include "sensors/ir-physical-receiver-io-pin.hpp"
#include "output/rgb-vibro-io-pins.hpp"
#include "output/rgb-vibro-pwm.hpp"
#include "rcsp/stream.hpp"
#include "hal/system-controls.hpp"

#include <stdio.h>
#include <math.h>

HeadSensor::HeadSensor() :
	AnyRCSPClientDeviceBase(nullptr)
{
	deviceConfig.deviceType = DeviceTypes::headSensor;
	m_tasksPool.setStackSize(512);
	m_interrogator.setStackSize(512);
}

void HeadSensor::init(const Pinout &_pinout, bool isSdcardOk)
{
	if (!isSdcardOk)
	{
		error << "Fatal error: head sensor cannot operate without sdcard!";
	}
	//debug.enable();

	info << "Parsing config file";
	if (!m_aggregator->readIni("config.ini"))
	{
		error << "Cannot read config file, so setting default values";
		playerConfig.setDefault();
	}

	info << "Restoring last state and config";
	m_stateSaver.setFilename("state-save");
	// State restoring is always after config reading, so not stored data will be default
	if (m_stateSaver.tryRestore())
	{
		info << "  restored";
	} else {
		error << "  restoring failed, using default config";
		// setting player state to default
		playerState.reset();
		m_stateSaver.saveState();
	}

	info << "Network initialization";
	initNetworkClient();
	static_cast<OrdinaryNetworkClient*>(m_networkClient)->registerMyBroadcast(broadcast.anyGameDevice);
	static_cast<OrdinaryNetworkClient*>(m_networkClient)->registerMyBroadcast(broadcast.headSensors);
	static_cast<OrdinaryNetworkClient*>(m_networkClient)->registerMyBroadcastTester(new TeamBroadcastTester(playerConfig.teamId));

	info << "Other initialization";
	m_tasksPool.add(
			[this] { m_taskPoolStager.stage("m_statsCounter.interrogate()"); m_statsCounter.interrogate(); },
			200000
	);

	m_tasksPool.add(
			[this] { m_taskPoolStager.stage("PowerMonitor::interrogate()"); m_powerMonitor.interrogate(); },
			100000
	);

	m_tasksPool.add(
			[this] { m_mfrcWrapper.interrogate(); },
			10000
	);

	m_interrogator.registerObject(&m_weaponsManager);

	info << "Discovering bluetooth module";
	// First, configuring HC-05 bluetooth module to have proper name, UART speed and password
	bool hasBtModule = HC05Configurator::quickTestBluetooth(IOPins->getIOPin(IIOPin::PORTA, 1), UARTs->get(IUARTSPool::UART2));
	if (hasBtModule)
	{
	    info << "Initializing built-in bluetooth bridge device";
	    BluetoothBridge *bb = new BluetoothBridge(m_networkLayer);
	    bb->assignExistingNetworkLayer(m_networkLayer);
	    bb->initAsSecondaryDevice(_pinout, isSdcardOk);
	} else {
	    info << "Bluetooth module not installed, running without it";
	}

	info << "Configuring sensors";
	m_receiverMgr.connectRCSPAggregator(*m_aggregator);
	m_irProtocolParser = new IRProtocolParserMilesTag2Ex(*m_aggregator, m_illuminationSchemes);
	m_receiverMgr.setParser(m_irProtocolParser);

	m_sensorsInitializer.read("sensors.ini");

	m_smartSensorsManager.init(UARTs->get(IUARTSPool::UART3));

	m_interrogator.registerObject(&m_receiverMgr);
	m_interrogator.registerObject(&m_ledVibroMgr);

	info << "Stats restoring";
	m_statsCounter.restoreFromFile();

	m_stateSaver.registerStateSaver(&m_statsCounter);

	m_tasksPool.run();
	m_stateSaver.runSaver(8000);

	m_interrogator.run();
	info << "Head sensor ready to use";


	m_mfrcWrapper.init();
	setFRIDToWriteAddr();

	// Statistics dubug
	m_statsCounter.clear();
	m_statsCounter.registerHit(25);
	m_statsCounter.registerHit(26);
	m_statsCounter.registerHit(27);
	m_statsCounter.registerDamage(25, 8);
	m_statsCounter.registerDamage(26, 9);
	m_statsCounter.registerDamage(27, 10);
	m_statsCounter.registerKill(25);
	m_statsCounter.registerKill(27);

	m_smartSensorsManager.run(
			[this](){ m_ledVibroMgr.applyIlluminationSchemeAllPoints(&(m_illuminationSchemes.init())); }
	);
}

void HeadSensor::initSmartZones(const Pinout &pinout)
{
    m_smartSensorsManager.init(UARTs->get(IUARTSPool::UART3));
}

void HeadSensor::resetToDefaults()
{
    taskENTER_CRITICAL();
    m_stateSaver.resetSaves();
    taskEXIT_CRITICAL();
    systemControls->rebootMCU();
    /*
    // Important!! Code below makes hard fault usually. And I dont know why.

	m_callbackStager.stage("reset");
	m_leds.blink(blinkPatterns.anyCommand);
	if (!m_aggregator->readIni("config.ini"))
	{
		error << "Cannot read config file, so setting default values";
		playerConfig.setDefault();
	}

	playerState.reset();
	m_stateSaver.resetSaves();
	m_stateSaver.saveState();

	// Hard fault-making code end
	 */
}

void HeadSensor::rifleToHeadSensorHeartbeat()
{
	if (!m_networkPackagesListener.hasSender())
		return;

	m_weaponsManager.updateWeapon(m_networkPackagesListener.sender());
	sendHeartbeat(m_networkPackagesListener.sender());
}


void HeadSensor::catchShot(ShotMessage msg)
{
	m_callbackStager.stage("shot");
	info << "** Shot - team: " << msg.teamId << ", player: " << msg.playerId << ", damage: " << msg.damage;
	Time currentTime = systemClock->getTime();
	if (currentTime - m_shockDelayBegin < playerConfig.shockDelayImmortal)
	{
		info << "!! Shock time";
		return;
	}
	if (playerState.isAlive())
	{
		if (msg.playerId == playerConfig.playerId)
		{
			debug << "self-shot";
			msg.damage *= playerConfig.selfShotCoeff;
		}
		else if (msg.teamId == playerConfig.teamId)
		{
			debug << "friendly fire";
			msg.damage *= playerConfig.frendlyFireCoeff;
		}

		UintParameter healthBeforeDamage = playerState.healthCurrent;

		debug << "total damage: " << msg.damage;
		playerState.damage(msg.damage);

		info << "health: " <<  playerState.healthCurrent << " armor: " << playerState.armorCurrent;

		// If still is alive
		if (playerState.isAlive())
		{
			if (msg.damage != 0)
			{
				m_shockDelayBegin = systemClock->getTime();
				weaponWoundAndShock();
				m_statsCounter.registerHit(msg.playerId);
				m_statsCounter.registerDamage(msg.playerId, msg.damage);
			}
			m_ledVibroMgr.applyIlluminationSchemeAtPoints(
					&(m_illuminationSchemes.wound()),
					m_receiverMgr.activeReceivers(),
					true);

			if (msg.playerId != playerConfig.playerId)
			{
				notifyDamager(msg.playerId, msg.teamId, DamageNotification::injured);
			}
		} else {
			//Player was killed
			info << "xx Player died";
			dieWeapons();
			if (msg.playerId != playerConfig.playerId)
			{
				notifyDamager(msg.playerId, msg.teamId, DamageNotification::killed);
			}
			m_statsCounter.registerKill(msg.playerId);
			m_statsCounter.registerDamage(msg.playerId, healthBeforeDamage);

			m_ledVibroMgr.applyIlluminationSchemeAtPoints(
								&(m_illuminationSchemes.death()),
								m_receiverMgr.activeReceivers(),
								true);
			/// @todo reenable
			//Scheduler::instance().addTask(std::bind(&StateSaver::saveState, &StateSaver::instance()), true, 0, 0, 1000000);
		}
	}
}

void HeadSensor::playerRespawn()
{
	m_callbackStager.stage("respawn");
	if (!playerState.respawn())
	{
		info << "Respawn limit is over!";
		// @todo Add any notification that respawn limit is over
		return;
	}

	respawnWeapons();
	info << "Player spawned";
	m_ledVibroMgr.applyIlluminationSchemeAllPoints(&(m_illuminationSchemes.anyCommand()));

	// @todo add delayed respawn
}

void HeadSensor::playerReset()
{
	playerState.reset();
	respawnWeapons();
	info << "Player reseted";
}

void HeadSensor::playerKill()
{
	if (!playerState.isAlive())
		return;
	playerState.kill();
	m_ledVibroMgr.applyIlluminationSchemeAllPoints(&(m_illuminationSchemes.death()));

	dieWeapons();
	info << "Player killed with kill command";
}

void HeadSensor::resetStats()
{
	m_statsCounter.clear();
}

void HeadSensor::readStats()
{
	debug << "Stats reading requested";
	if (!m_networkPackagesListener.hasSender())
	{
		warning << "Stats reading request not over network!";
	}
	m_statsCounter.sendStats(m_networkPackagesListener.sender());
}

void HeadSensor::dieWeapons()
{
	m_callbackStager.stage("die weapons");
	m_weaponCommunicator.sendDie();
}

void HeadSensor::respawnWeapons()
{
	m_callbackStager.stage("respawn weapons");
	m_weaponCommunicator.sendRespawn();
}

void HeadSensor::weaponWoundAndShock()
{
	m_callbackStager.stage("weaponWoundAndShock");
	info << "Weapons shock delay notification";
	m_weaponCommunicator.sendWeaponAndShock(playerConfig.shockDelayInactive);
}

void HeadSensor::sendHeartbeat(DeviceAddress target)
{
	trace << "Sending heartbeat";
	m_weaponCommunicator.sendHeartbeat(target, &playerConfig, &playerState);
}

void HeadSensor::setTeam(uint8_t teamId)
{
	info << "Setting team id";
	playerConfig.teamId = teamId;
	m_ledVibroMgr.applyIlluminationSchemeAllPoints(&(m_illuminationSchemes.anyCommand()));
	m_weaponCommunicator.sendSetTeam();
}

void HeadSensor::addMaxHealth(int16_t delta)
{
	info << "Adding health: " << delta;
	if (delta < 0 && playerConfig.healthMax < -delta)
	{
		debug << "Max health is " << playerConfig.healthMax << ", so can not add " << delta;
		return;
	}
	playerConfig.healthMax += delta;
	if (delta < 0 && playerConfig.healthStart < -delta)
	{
		debug << "Start health is " << playerConfig.healthStart << ", so can not add " << delta;
		return;
	}
	playerConfig.healthStart += delta;
	m_ledVibroMgr.applyIlluminationSchemeAllPoints(&(m_illuminationSchemes.anyCommand()));
}

void HeadSensor::notifyDamager(PlayerGameId damager, uint8_t damagerTeam, uint8_t state)
{
	UNUSED_ARG(damagerTeam);
	DamageNotification notification;
	notification.damager = damager;
	notification.damagedTeam = playerConfig.teamId;
	notification.state = state;
	notification.target = playerConfig.playerId;
	info << "Notifying damager";
	RCSPStream::remoteCall(
	        m_networkClient,
			broadcast.headSensors,
			ConfigCodes::HeadSensor::Functions::notifyIsDamager,
			notification,
			true,
			nullptr,
			std::move(headSensorPackageTimings.damagerNotificationBroadcast)
			);
}

void HeadSensor::notifyIsDamager(DamageNotification notification)
{
	info << "By the time " << notification.damager << " damaged " << notification.target;
	if (notification.damager != playerConfig.playerId)
	{
	    debug << "not for me: my id is " << playerConfig.playerId;
		return;
	}

	uint8_t sound =	(notification.damagedTeam == playerConfig.teamId) ? // testing for friendly fire
					NotificationSoundCase::friendInjured :
					notification.state;

	m_weaponCommunicator.sendPlayEnemyDamaged(sound);
}

void HeadSensor::setFRIDToWriteAddr()
{
	uint16_t size = sizeof(m_RFIDWriteBuffer[0])*RFIDWriteBufferSize;
	memset(m_RFIDWriteBuffer, 0, size);
	uint16_t actualSize = 0;
	RCSPAggregator::serializeCall(
			m_RFIDWriteBuffer,
			ConfigCodes::Rifle::Functions::rifleChangeHS,
			size,
			actualSize,
			deviceConfig.devAddr
		);
	m_mfrcWrapper.writeBlock(
			m_RFIDWriteBuffer,
			RFIDWriteBufferSize,
			[this](uint8_t* data, uint16_t size)
			{
				UNUSED_ARG(data); UNUSED_ARG(size);
				m_ledVibroMgr.applyIlluminationSchemeAllPoints(&(m_illuminationSchemes.anyCommand()));
			}
		);
}

void HeadSensor::setDefaultPinout(Pinout& pinout)
{
	UNUSED_ARG(pinout);
}

bool HeadSensor::checkPinout(const Pinout& pinout)
{
	UNUSED_ARG(pinout);

	return true;
}


/////////////////////
// HeadSensor::TeamBroadcastTester
bool HeadSensor::TeamBroadcastTester::isAcceptableBroadcast(const DeviceAddress& addr)
{
	if (addr == broadcast.headSensorsRed && *m_pId == 0)
		return true;
	if (addr == broadcast.headSensorsBlue && *m_pId == 1)
		return true;
	if (addr == broadcast.headSensorsYellow && *m_pId == 2)
		return true;
	if (addr == broadcast.headSensorsGreen && *m_pId == 3)
		return true;
	return false;
}


/////////////////////
// Test functions
void HeadSensor::testDie(const char*)
{
	playerState.healthCurrent = 0;
	catchShot(ShotMessage());
}


