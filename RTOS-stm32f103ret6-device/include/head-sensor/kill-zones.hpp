/*
 * kill-zones.hpp
 *
 *  Created on: 06 мая 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_HEAD_SENSOR_KILL_ZONES_HPP_
#define INCLUDE_HEAD_SENSOR_KILL_ZONES_HPP_

#include "dev/miles-tag-2.hpp"
#include "hal/system-clock.hpp"
#include "hal/io-pins.hpp"
#include "rcsp/RCSP-aggregator.hpp"
#include "rcsp/operation-codes.hpp"
#include "core/os-wrappers.hpp"

using DamageCallback = std::function<void(unsigned int /*teamId*/, unsigned int /*playerId*/, unsigned int /*damage*/)>;

class KillZonesManager
{
public:
	constexpr static uint8_t killZonesMaxCount = 6;
	constexpr static uint32_t callbackDelay = 50;
	constexpr static uint32_t vibroPeriod = 200;
	KillZonesManager(const FloatParameter& zone1DamageCoeff,
		const FloatParameter& zone2DamageCoeff,
		const FloatParameter& zone3DamageCoeff,
		const FloatParameter& zone4DamageCoeff,
		const FloatParameter& zone5DamageCoeff,
		const FloatParameter& zone6DamageCoeff
	);
	KillZonesManager(const KillZonesManager&) = delete;
	void enableKillZone(uint8_t zone, IIOPin *input, IIOPin* vibro = nullptr);
	void setCallback(DamageCallback callback);

private:
	struct KillZone
	{
		const FloatParameter* zoneDamageCoeff = nullptr;
		MilesTag2Receiver* killZone = nullptr;
		IIOPin* vibro = nullptr;
		Time vibrationStopTime = 0;
	};
	void interrogate();
	void IRReceiverShotCallback_ISR(uint8_t zoneId, unsigned int teamId, unsigned int playerId, unsigned int damage);

	KillZone m_killZones[killZonesMaxCount];
	DamageCallback m_callback = nullptr;

	Time m_lastDamageMoment = 0;
	unsigned int m_maxDamage = 0;
	unsigned int m_teamId = 0;
	unsigned int m_playerId = 0;

	TaskCycled m_interrogateTask;
};


#endif /* INCLUDE_HEAD_SENSOR_KILL_ZONES_HPP_ */
