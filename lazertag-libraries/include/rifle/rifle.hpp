/*
 * rifle.hpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_

#include "rifle/rifle-config-and-state.hpp"
#include "logic/device.hpp"
#include "rcsp/RCSP-modem.hpp"
#include "dev/buttons.hpp"
#include "dev/miles-tag-2.hpp"
#include "dev/wav-player.hpp"
#include "core/scheduler.hpp"

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

	void configure();
	void registerWeapon();

	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleTurnOn);
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleTurnOff);
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleReset);
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleRespawn);
	FUNCION_NP(ConfigCodes::Rifle::Functions, Rifle, rifleDie);

	RifleConfiguration config;
	RifleOwnerConfiguration rifleOwner;
	RifleState state{&config};
	//DeviceParameters device;

	PlayerDisplayableData playerDisplayable{config.headSensorAddr};

	/////////
	// Buttons mapping
	// Default (rifle-rev.1)
	uint8_t fireButtonPort   =   0;
	uint8_t fireButtonPin    =   0;

	uint8_t reloadButtonPort =   1;
	uint8_t reloadButtonPin  =   2;

	uint8_t automaticButtonPort      =   2;
	uint8_t automaticButtonPin       =   7;

	uint8_t semiAutomaticButtonPort  =   1;
	uint8_t semiAutomaticButtonPin   =   9;


private:
	void loadConfig();
	void initSounds();
	void makeShot(bool isFirst);
	void reload(bool isFirst);

	bool isReloading();
	bool isSafeSwitchSelected();

	void updatePlayerState();

	bool isEnabled = true;

	ButtonManager* m_fireButton = nullptr;
	ButtonManager* m_reloadButton = nullptr;
	ButtonManager* m_automaticFireSwitch = nullptr;
	ButtonManager* m_semiAutomaticFireSwitch = nullptr;

	MilesTag2Transmitter m_mt2Transmitter;

	SoundPlayer m_shootingSound;
	SoundPlayer m_reloadingSound;
	SoundPlayer m_noAmmoSound;
	SoundPlayer m_noMagazines;
	SoundPlayer m_respawnSound;
	SoundPlayer m_dieSound;

	PackageId m_registerWeaponPAckageId = 0;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_ */
