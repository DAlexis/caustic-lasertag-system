#include "head-sensor/hs-rifles-comunicator.hpp"
#include "head-sensor/resources.hpp"
#include "rcsp/operation-codes.hpp"
#include "rcsp/stream.hpp"
#include "core/logging.hpp"

WeaponCommunicator::WeaponCommunicator(
	WeaponsManager& weaponManager,
	INetworkClientSender& networkClientSender,
	INetworkLayer *networkLayer,
	RCSPAggregator& aggregator
) :
	m_weaponManager(weaponManager),
	m_networkClientSender(networkClientSender),
	m_networkLayer(networkLayer),
	m_aggregator(aggregator)
{
}

void WeaponCommunicator::sendRespawn()
{
	declinePackages(m_respawnPackages);
	declinePackages(m_diePackages);

	m_weaponManager.applyToAny(
		[this](DeviceAddress addr)
		{
			m_respawnPackages.push_back(
				RCSPStream::call(
					&m_networkClientSender,
					addr,
					ConfigCodes::Rifle::Functions::rifleRespawn
				)
			);
		}
	);
}

void WeaponCommunicator::sendDie()
{
	declinePackages(m_diePackages);
	declinePackages(m_respawnPackages);

	m_weaponManager.applyToAny(
		[this](DeviceAddress addr)
		{
			m_diePackages.push_back(
				RCSPStream::call(
					&m_networkClientSender,
					addr,
					ConfigCodes::Rifle::Functions::rifleDie,
					true, nullptr, std::move(headSensorPackageTimings.killPlayer)
				)
			);
		}
	);
}

void WeaponCommunicator::sendTurnOff()
{
	/// todo implement here
}

void WeaponCommunicator::sendTurnOn()
{
	/// todo implement here
}

void WeaponCommunicator::sendWeaponAndShock(TimeInterval shockDelay)
{
	m_weaponManager.applyToAny(
		[this, shockDelay](DeviceAddress addr)
		{
			RCSPStream stream(&m_aggregator);
			// We have 23 bytes free in one stream (and should try to use only one)
			stream.addCall(ConfigCodes::Rifle::Functions::rifleShock, shockDelay); // 3b + 4b (16 free)
			stream.addCall(ConfigCodes::Rifle::Functions::rifleWound); // 3b (13 free)
			stream.addPush(ConfigCodes::HeadSensor::State::healthCurrent); // 3b + 2b (8 free)
			stream.addPush(ConfigCodes::HeadSensor::State::armorCurrent); // 3b + 2b (3 free)
			stream.send(&m_networkClientSender, addr, true, nullptr, std::move(headSensorPackageTimings.woundPlayer));
		}
	);
}

void WeaponCommunicator::sendSetTeam()
{
	m_weaponManager.applyToAny(
		[this](DeviceAddress addr)
		{
			RCSPStream::push(
					&m_aggregator,
					&m_networkClientSender,
					addr,
					ConfigCodes::HeadSensor::Configuration::teamId
			);
		}
	);
}

void WeaponCommunicator::sendPlayEnemyDamaged(uint8_t sound)
{
	m_weaponManager.applyToAny(
		[this, sound](DeviceAddress addr)
		{
			RCSPStream::call(
				&m_networkClientSender,
				addr,
				ConfigCodes::Rifle::Functions::riflePlayEnemyDamaged,
				sound
			);
		}
	);
}

void WeaponCommunicator::sendHeartbeat(DeviceAddress target, PlayerConfiguration* config, PlayerState* state)
{
	RCSPStream stream(&m_aggregator);
	HSToRifleHeartbeat hb;
	hb.setEnabled(state->isAlive());
	hb.team = config->teamId;
	hb.player = config->playerId;
	hb.healthCurrent = state->healthCurrent;
	hb.healthMax = config->healthMax;
	stream.addCall(ConfigCodes::Rifle::Functions::headSensorToRifleHeartbeat, hb);
	stream.send(
		&m_networkClientSender,
		target,
		false,
		nullptr,
		std::forward<PackageTimings>(headSensorPackageTimings.heartbeatToRifle)
	);
}

void WeaponCommunicator::declinePackages(std::list<PackageId>& targetList)
{
	for (auto &it : targetList)
	{
		m_networkLayer->stopSending(it);
	}
	targetList.clear();
}
