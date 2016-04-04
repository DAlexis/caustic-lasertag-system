/*
 * rifle.hpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_

#include "rifle/rifle-config-and-state.hpp"
#include "rifle/resources.hpp"
#include "rifle/rifle-base-types.hpp"
#include "rifle/rifle-display.hpp"
#include "device/device.hpp"
#include "rcsp/operation-codes.hpp"
#include "dev/buttons.hpp"
#include "dev/miles-tag-2.hpp"
#include "dev/wav-player.hpp"
#include "core/os-wrappers.hpp"
#include "core/device-initializer.hpp"

#include <stdint.h>


class Rifle : public IAnyDevice
{
public:
	Rifle();

	void registerWeapon();
	void init(const Pinout& pinout);
	void setDafaultPinout(Pinout& pinout);
	bool checkPinout(const Pinout& pinout);

	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleTurnOn);
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleTurnOff);
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleReset);    ///< Make rifle absolutely new
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleRespawn);  ///< Do all things than needed on respawn (+play sound)
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleDie);      ///< Say to rifle that player was killed

	/// Heartbeat head sensor -> rifle
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, headSensorToRifleHeartbeat);
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleWound);

	FUNCION_1P(ConfigCodes::Rifle::Functions, Rifle, riflePlayEnemyDamaged);      ///< Play enemy damaged sound
	FUNCION_1P(ConfigCodes::Rifle::Functions, Rifle, rifleShock);      ///< Play enemy damaged sound

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
	void distortBolt(bool isFirst);
	void magazineSensor(bool isConnected, uint8_t sensorNumber, bool isFirst);
	uint8_t getCurrentMagazineNumber();

	void reloadAndPlay();
	void detectRifleState();

	bool isReloading();
	bool isSafeSwitchSelected();

	void updatePlayerState();

	void playDamagerNotification(uint8_t state);
	void scheduleDamageNotification(uint8_t state);
	void checkHeartBeat();
	bool isShocked();


	ButtonManager* m_fireButton = nullptr;
	ButtonManager* m_reloadButton = nullptr;
	ButtonManager* m_automaticFireSwitch = nullptr;
	ButtonManager* m_semiAutomaticFireSwitch = nullptr;
	ButtonManager* m_magazine1Sensor = nullptr;
	ButtonManager* m_magazine2Sensor = nullptr;
	IIOPin* m_vibroEngine = nullptr;
	IIOPin* m_fireFlash = nullptr;


	MilesTag2Transmitter m_mt2Transmitter{config.outputPower};

	SoundPlayer m_systemReadySound;
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

	PackageId m_registerWeaponPAckageId = 0;

	uint8_t m_state = WeaponState::ready;
	uint8_t m_currentMagazineNumber = 0; ///< zero means no magazine

	Interrogator m_buttonsInterrogator{"BtnsIntr"};
	TasksPool m_tasksPool{"RiflPool"};
	Time m_lastHSHeartBeat = 0;

	/// If current time > m_unshockTime, player is not shocked now
	Time m_unshockTime = 0;
	RifleLCD5110Display m_display{&rifleOwner, &state, &playerState};
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_ */
