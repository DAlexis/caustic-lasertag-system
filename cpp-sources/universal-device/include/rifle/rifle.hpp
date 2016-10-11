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

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_

#include "rifle/rifle-config-and-state.hpp"
#include "rifle/resources.hpp"
#include "rifle/rifle-base-types.hpp"
#include "rifle/rifle-display.hpp"
#include "ir/ir-physical.hpp"
#include "ir/ir-presentation.hpp"
#include "dev/MFRC522-wrapper.hpp"
#include "device/device.hpp"
#include "rcsp/operation-codes.hpp"
#include "dev/buttons.hpp"
#include "dev/wav-player.hpp"
#include "core/os-wrappers.hpp"
#include "core/device-initializer.hpp"

#include <stdint.h>


class Rifle : public IAnyDevice
{
public:
	Rifle();

	void registerWeapon();
	void init(const Pinout& pinout) override;
	void setDafaultPinout(Pinout& pinout) override;
	bool checkPinout(const Pinout& pinout) override;

	FUNCTION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleTurnOn);
	FUNCTION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleTurnOff);
	FUNCTION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleReset);    ///< Make rifle absolutely new
	FUNCTION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleRespawn);  ///< Do all things than needed on respawn (+play sound)
	FUNCTION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleDie);      ///< Say to rifle that player was killed

	/// Heartbeat head sensor -> rifle
	FUNCTION_NP(ConfigCodes::Rifle::Functions, Rifle, headSensorToRifleHeartbeat);
	FUNCTION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleWound);

	FUNCTION_1P(ConfigCodes::Rifle::Functions, Rifle, riflePlayEnemyDamaged);      ///< Play enemy damaged sound
	FUNCTION_1P(ConfigCodes::Rifle::Functions, Rifle, rifleShock);      ///< Play enemy damaged sound
	FUNCTION_1P(ConfigCodes::Rifle::Functions, Rifle, rifleChangeHS);      ///< Play enemy damaged sound

	DeviceConfiguration deviceConfig;
	RifleConfiguration config;
	RifleOwnerConfiguration rifleOwner;
	RifleState state{&config};
	//DeviceParameters device;

	PlayerPartialState playerState{config.headSensorAddr};


private:

	constexpr static uint32_t maxNoHeartbeatDelay = 6500000;

	struct PinoutTexts
	{
		constexpr static const char *trigger = "fireButton";
		constexpr static const char *reload = "reloadButton";
		constexpr static const char *automatic = "automaticButton";
		constexpr static const char *semiAutomatic = "semiAutomaticButton";
		constexpr static const char *mag1Sensor = "magazine1Sensor";
		constexpr static const char *mag2Sensor = "magazine2Sensor";
		constexpr static const char *flash = "flash";
		constexpr static const char *vibro = "vibro";
	};

	struct WeaponState
	{
		constexpr static uint8_t magazineEmpty = 0;
		constexpr static uint8_t magazineRemoved = 1;
		constexpr static uint8_t magazineReturned = 2;
		constexpr static uint8_t otherMagazineInserted = 3;
		constexpr static uint8_t reloading = 4;
		constexpr static uint8_t ready = 5;
	};

	void loadConfig();
	void initSounds();
	void makeShot(bool isFirst);
	void prepareAndSendShotMsg();
	void distortBolt(bool isFirst);
	void magazineSensor(bool isConnected, uint8_t sensorNumber, bool isFirst);
	uint8_t getCurrentMagazineNumber();

	void reloadAndPlay();
	void detectRifleState();

	bool isReloading();
	bool isSafeSwitchSelected();
	bool isShocked();
	bool isHSConnected();

	void updatePlayerState();

	void playDamagerNotification(uint8_t state);
	void scheduleDamageNotification(uint8_t state);
	void checkHeartBeat();

	void onCardReaded(uint8_t* buffer, uint16_t size);
	/// This function could be called any time when head sensor is connected. Double calling does not hurt anything
	void onHSConnected();
	void onHSDisconnected();

	ButtonManager* m_fireButton = nullptr;
	ButtonManager* m_reloadButton = nullptr;
	ButtonManager* m_automaticFireSwitch = nullptr;
	ButtonManager* m_semiAutomaticFireSwitch = nullptr;
	ButtonManager* m_magazine1Sensor = nullptr;
	ButtonManager* m_magazine2Sensor = nullptr;
	IIOPin* m_vibroEngine = nullptr;
	IIOPin* m_fireFlash = nullptr;

	IIRTransmitter *m_irPhysicalTransmitter = nullptr;
	IIRPresentationTransmitter *m_irPresentationTransmitter = nullptr;

	SoundPlayer m_systemReadySound;
	SoundPlayer m_RFIDCardReaded;
	SoundPlayer m_connectedToHeadSensorSound;
	SoundPlayer m_shootingSound;
	SoundPlayer m_reloadingSound;
	SoundPlayer m_noAmmoSound;
	SoundPlayer m_noMagazines;
	SoundPlayer m_respawnSound;
	SoundPlayer m_dieSound;
	SoundPlayer m_woundSound;
	SoundPlayer m_enemyDamaged;
	SoundPlayer m_enemyKilled;
	SoundPlayer m_friendDamaged;
	SoundPlayer m_noHeartbeat;
	SoundPlayer m_noShockedShooting;
	SoundPlayer m_hsSwitchRejected;

	PackageId m_registerWeaponPAckageId = 0;

	uint8_t m_state = WeaponState::ready;
	uint8_t m_currentMagazineNumber = 0; ///< zero means no magazine

	Interrogator m_buttonsInterrogator{"BtnsIntr"};
	TasksPool m_tasksPool{"RiflPool"};
	Time m_lastHSHeartBeat = 0;

	/// If current time > m_unshockTime, player is not shocked now
	Time m_unshockTime = 0;
	RifleLCD5110Display m_display{&rifleOwner, &state, &playerState};

	RC552Wrapper m_mfrcWrapper;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_ */
