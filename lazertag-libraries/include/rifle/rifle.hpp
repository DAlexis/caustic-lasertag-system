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
#include "core/scheduler.hpp"

#include <stdint.h>

class PlayerDisplayableData
{
public:
	PlayerDisplayableData();
	void syncAll();
	void print();

	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, health);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, armor);

	PARAMETER(ConfigCodes::Player::State, UintParameter, s_health);
	PARAMETER(ConfigCodes::Player::State, UintParameter, s_armor);

	PARAMETER(ConfigCodes::Player::State, UintParameter, s_lifesCount);
	PARAMETER(ConfigCodes::Player::State, UintParameter, pointsCount);
	PARAMETER(ConfigCodes::Player::State, UintParameter, killsCount);
	PARAMETER(ConfigCodes::Player::State, UintParameter, deathsCount);
};

class Rifle
{
public:
	Rifle();

	void configure();

	FUNCION(ConfigCodes::Rifle::Functions, Rifle, turnOn);
	FUNCION(ConfigCodes::Rifle::Functions, Rifle, turnOff);
	FUNCION(ConfigCodes::Rifle::Functions, Rifle, reset);

	RifleConfiguration config;
	RifleState state{&config};
	DeviceParameters device;

	PlayerDisplayableData playerDisplayable;

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


};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_ */
