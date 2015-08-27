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
#include "logic/device.hpp"
#include "rcsp/RCSP-modem.hpp"
#include "dev/buttons.hpp"
#include "dev/miles-tag-2.hpp"
#include "dev/wav-player.hpp"
#include "core/scheduler.hpp"
#include "logic/head-sensor-rifle-communication.hpp"

#include <stdint.h>

class PlayerDisplayableData
{
public:
	PlayerDisplayableData(const DeviceAddress& headSensorAddress);
	void syncAll();
	void print();

	PAR_ST(ConfigCodes::HeadSensor::Configuration, UintParameter, healthMax);
	PAR_ST(ConfigCodes::HeadSensor::Configuration, UintParameter, armorMax);

	PAR_ST(ConfigCodes::HeadSensor::State, UintParameter, healthCurrent);
	PAR_ST(ConfigCodes::HeadSensor::State, UintParameter, armorCurrent);

	PAR_ST(ConfigCodes::HeadSensor::State, UintParameter, lifesCountCurrent);
	PAR_ST(ConfigCodes::HeadSensor::State, UintParameter, pointsCount);
	PAR_ST(ConfigCodes::HeadSensor::State, UintParameter, killsCount);
	PAR_ST(ConfigCodes::HeadSensor::State, UintParameter, deathsCount);

private:
	const DeviceAddress* m_headSensorAddress;
};

class Rifle
{
public:
	Rifle();

	void configure(RiflePinoutMapping& pinout);
	void registerWeapon();

	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleTurnOn);
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleTurnOff);
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleReset);    ///< Make rifle absolutely new
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleRespawn);  ///< Do all things than needed on respawn (+play sound)
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleDie);      ///< Say to rifle that player was killed

	FUNCION_1P(ConfigCodes::Rifle::Functions, Rifle, riflePlayEnemyDamaged, uint8_t);      ///< Play enemy damaged sound
	RifleConfiguration config;
	RifleOwnerConfiguration rifleOwner;
	RifleState state{&config};
	//DeviceParameters device;

	PlayerDisplayableData playerDisplayable{config.headSensorAddr};


private:
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

	bool m_isEnabled = true;

	ButtonManager* m_fireButton = nullptr;
	ButtonManager* m_reloadButton = nullptr;
	ButtonManager* m_automaticFireSwitch = nullptr;
	ButtonManager* m_semiAutomaticFireSwitch = nullptr;
	ButtonManager* m_magazine1Sensor = nullptr;
	ButtonManager* m_magazine2Sensor = nullptr;
	IIOPin* m_vibroEngine = nullptr;
	IIOPin* m_fireFlash = nullptr;


	MilesTag2Transmitter m_mt2Transmitter;

	SoundPlayer m_shootingSound;
	SoundPlayer m_reloadingSound;
	SoundPlayer m_noAmmoSound;
	SoundPlayer m_noMagazines;
	SoundPlayer m_respawnSound;
	SoundPlayer m_dieSound;
	SoundPlayer m_enemyDamaged;
	SoundPlayer m_enemyKilled;
	SoundPlayer m_friendDamaged;


	PackageId m_registerWeaponPAckageId = 0;

	uint8_t m_state = WeaponState::ready;
	uint8_t m_currentMagazineNumber = 0; ///< zero means no magazine
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_ */