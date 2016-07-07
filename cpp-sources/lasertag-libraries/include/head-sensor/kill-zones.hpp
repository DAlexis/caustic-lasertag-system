/*
 * kill-zones.hpp
 *
 *  Created on: 06 мая 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_HEAD_SENSOR_KILL_ZONES_HPP_
#define INCLUDE_HEAD_SENSOR_KILL_ZONES_HPP_

#include "dev/miles-tag-2.hpp"
#include "hal/io-pins.hpp"
#include "rcsp/RCSP-aggregator.hpp"
#include "rcsp/operation-codes.hpp"
#include "core/scheduler.hpp"

using DamageCallback = std::function<void(unsigned int /*teamId*/, unsigned int /*playerId*/, unsigned int /*damage*/)>;

class KillZonesManager
{
public:
	constexpr static uint8_t killZonesMaxCount = 6;
	constexpr static uint32_t callbackDelay = 50000;
	constexpr static uint32_t vibroPeriod = 200000;
	KillZonesManager(const FloatParameter& zone1DamageCoeff,
		const FloatParameter& zone2DamageCoeff,
		const FloatParameter& zone3DamageCoeff,
		const FloatParameter& zone4DamageCoeff,
		const FloatParameter& zone5DamageCoeff,
		const FloatParameter& zone6DamageCoeff
	);
	KillZonesManager(const KillZonesManager&) = delete;
	void enableKillZone(uint8_t zone, IExternalInterruptManager *exti, IIOPin* vibro = nullptr);
	void setCallback(DamageCallback callback);

private:
	void callDamageCallback();
	void IRReceiverShotCallback(uint8_t zoneId, unsigned int teamId, unsigned int playerId, unsigned int damage);
	void vibrate(uint8_t zone);

	const FloatParameter* m_zoneDamageCoeff[killZonesMaxCount];
	MilesTag2Receiver m_killZone[killZonesMaxCount];
	IIOPin* m_vibro[killZonesMaxCount];
	DamageCallback m_callback = nullptr;

	bool m_damageCbScheduled = false;

	unsigned int m_maxDamage = 0;
	unsigned int m_teamId = 0;
	unsigned int m_playerId = 0;
	TaskId m_vibrationStopTask = 0;
};


#endif /* INCLUDE_HEAD_SENSOR_KILL_ZONES_HPP_ */
