/*
 * hs-rifles-comunicator.hpp
 *
 *  Created on: 15 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HEAD_SENSOR_HS_RIFLES_COMUNICATOR_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HEAD_SENSOR_HS_RIFLES_COMUNICATOR_HPP_

#include "head-sensor/weapon-manager.hpp"
#include "head-sensor/player-config.hpp"
#include "head-sensor/player-state.hpp"
#include "network/network-layer-interface.hpp"
#include "rcsp/aggregator.hpp"
#include "rcsp/base-types.hpp"

#include <list>

class WeaponCommunicator
{
public:
	WeaponCommunicator(
		WeaponsManager2* weaponManager,
		INetworkClient *networkClient,
		INetworkLayer *networkLayer,
		RCSPAggregator* aggregator
	);

	void sendRespawn();
	void sendDie();
	void sendTurnOff();
	void sendTurnOn();
	void sendWeaponAndShock(TimeInterval shockDelay);
	void sendSetTeam();
	void sendPlayEnemyDamaged(uint8_t sound);
	void sendHeartbeat(DeviceAddress target, PlayerConfiguration* config, PlayerState* state);

private:
	void declinePackages(std::list<PackageId>& targetList);

	WeaponsManager2* m_weaponManager;
	INetworkClient *m_networkClient;
	INetworkLayer *m_networkLayer;
	RCSPAggregator *m_aggregator;

	std::list<PackageId> m_respawnPackages;
	std::list<PackageId> m_diePackages;
};






#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HEAD_SENSOR_HS_RIFLES_COMUNICATOR_HPP_ */
