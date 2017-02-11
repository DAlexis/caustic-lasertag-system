/*
 * sensors-initializer.cpp
 *
 *  Created on: 4 февр. 2017 г.
 *      Author: dalexies
 */

#include "head-sensor/sensors-initializer.hpp"
#include "output/rgb-vibro-io-pins.hpp"
#include "output/rgb-vibro-pwm.hpp"
#include "core/string-utils.hpp"
#include "core/logging.hpp"
#include "sensors/ir-physical-receiver-io-pin.hpp"

SensorsInitializer::SensorsInitializer(IRReceiversManager& irMgr, LedVibroManager& lvMgr, KillZonesManager &kzMgr) :
	m_irMgr(irMgr), m_lvMgr(lvMgr), m_kzMgr(kzMgr)
{
	resetVariables();
}

void SensorsInitializer::read(const char* filename)
{
	IniParser parser;
	parser.setCallback([this](const char* group, const char* key, const char* value)
	{
		processKeyValue(group, key, value);
	});
	resetVariables();
	auto res = parser.parseFile(filename);
	processKeyValue("", "", "");
	if (!res.isSuccess)
	{
		error << "Error while initializing sensors: " << res.errorText;
		return;
	}
}

void SensorsInitializer::processKeyValue(const char* group, const char* key, const char* value)
{
	UNUSED_ARG(group);
	switch(m_state)
	{
	case State::nothing:
		if (checkPrefix(key, "sensor"))
			m_state = State::processingBuiltInSensor;
		else if (checkPrefix(key, "point"))
			m_state = State::processingBuiltInPoint;
		else if (checkPrefix(key, "zone"))
			m_state = State::processingZone;

		if (m_state != State::nothing)
			processKeyValue(group, key, value);
		break;
	case State::processingBuiltInSensor:
		if (!checkPrefix(key, "sensor"))
		{
			finishBuiltInSensorProcessing();
			m_state = State::nothing;
			processKeyValue(group, key, value);
			break;
		}
		if (strcmp(key, "sensor_id") == 0)
		{
			// May be we have next sensor in a row
			if (isSensorParamsReady())
				finishBuiltInSensorProcessing();
			m_currentId = atoi(value);
		} else if (strcmp(key, "sensor_port") == 0)
		{
			m_currentPort = IIOPin::parsePort(value);
		} else if (strcmp(key, "sensor_pin") == 0)
		{
			m_currentPin = atoi(value);
		}
		break;
	case State::processingBuiltInPoint:
		if (!checkPrefix(key, "point"))
		{
			finishBuiltInPointProcessing();
			m_state = State::nothing;
			processKeyValue(group, key, value);
			break;
		}
		if (strcmp(key, "point_id") == 0)
		{
			// May be we have next point in a row
			if (isPointParamsReady())
				finishBuiltInPointProcessing();
			m_currentId = atoi(value);
		} else if (strcmp(key, "point_type") == 0)
		{
			strcpy(m_str, value);
		} else if (strcmp(key, "point_timer") == 0)
		{
			if (strcmp(value, "TIM1") == 0)
				m_currentTimer = 1;
			else if (strcmp(value, "TIM2") == 0)
				m_currentTimer = 2;
			else if (strcmp(value, "TIM3") == 0)
				m_currentTimer = 3;
			else if (strcmp(value, "TIM4") == 0)
				m_currentTimer = 4;
			else m_currentTimer = 0xFF;
		} else if (strcmp(key, "point_vibro_port") == 0)
		{
			m_currentPort = IIOPin::parsePort(value);
		} else if (strcmp(key, "point_vibro_pin") == 0)
		{
			m_currentPin = atoi(value);
		} else if (strcmp(key, "point_is_zone_wide") == 0)
		{
			m_zoneWide = StringParser<bool>().parse(value);
		} else if (strcmp(key, "point_is_system_wide") == 0)
		{
			m_systemWide = StringParser<bool>().parse(value);
		}
		break;
	case State::processingZone:
		if (!checkPrefix(key, "zone"))
		{
			finishZoneProcessing();
			m_state = State::nothing;
			processKeyValue(group, key, value);
			break;
		}

		if (strcmp(key, "zone_id") == 0)
		{
			// May be we have next point in a row
			if (isZoneParamsReady())
				finishZoneProcessing();
			m_currentId = atoi(value);
		} else if (strcmp(key, "zone_sensors") == 0)
		{
			strcpy(m_str, value);
		} else if (strcmp(key, "zone_damage") == 0)
		{
			m_damage = atof(value);
		}
		break;
	}
}

void SensorsInitializer::finishBuiltInSensorProcessing()
{
	if (!isSensorParamsReady())
		return;

	IRReceiverPhysicalIOPin* receiver = new IRReceiverPhysicalIOPin(IOPins->getIOPin(m_currentPort, m_currentPin), m_currentId);
	receiver->init();
	m_irMgr.addPhysicalReceiver(receiver);

	resetVariables();
}

void SensorsInitializer::finishBuiltInPointProcessing()
{
	if (!isPointParamsReady())
		return;
	if (strcmp(m_str, "pwm") == 0)
	{
		if (m_currentId != -1 && m_currentTimer != -1)
		{
			IIOPin* vibro = nullptr;
			if (m_currentPin != -1 && m_currentPort != -1)
			{
				vibro = IOPins->getIOPin(m_currentPort, m_currentPin);
			}

			IRGBVibroPointPhysical *rgbv = new RBGVibroPWM(
				m_currentId,
				PWMs->getPWM(m_currentTimer),
				vibro
			);
			if (rgbv == nullptr)
			{
				error << "Cannot initialize PWM illunimation point!";
				resetVariables();
				return;
			}
			m_lvMgr.addPoint(rgbv, m_zoneWide, m_systemWide);
		}
	}
	resetVariables();
}

void SensorsInitializer::finishZoneProcessing()
{
	if (!isZoneParamsReady())
		return;

	m_kzMgr.setDamageCoefficient(m_currentId, m_damage);
	UintParameter sensorId = 0;
	for (int i=0; i < strMaxLen-1 && m_str[i] != '\0'; i++)
	{
		if (isNumber(m_str[i]))
		{
			sensorId = 10*sensorId + (m_str[i] - '0');
		}
		if (!isNumber(m_str[i+1]))
		{
			if (sensorId != 0)
			{
				m_kzMgr.assignSensorToZone(sensorId, m_currentId);
			}
			sensorId = 0;
		}
	}
	resetVariables();
}

bool SensorsInitializer::isSensorParamsReady()
{
	return m_currentId != -1 && m_currentPin != -1 && m_currentPort != -1;
}

bool SensorsInitializer::isPointParamsReady()
{
	return m_currentId != -1 && m_str[0] != '\0';
}

bool SensorsInitializer::isZoneParamsReady()
{
	return m_currentId != -1 && m_str[0] != '\0';
}

void SensorsInitializer::resetVariables()
{
	m_currentId = -1;
	m_currentPort = -1;
	m_currentPin = -1;
	m_currentTimer = -1;
	m_systemWide = false;
	m_zoneWide = false;
	m_damage = 1.0;
	m_str[0] = '\0';
	m_state = State::nothing;
}
