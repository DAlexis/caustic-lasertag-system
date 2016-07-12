/*
 * smart-point.hpp
 *
 *  Created on: 9 июл. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_HPP_
#define UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_HPP_

#include "smart-point/smart-point-config-and-state.hpp"
#include "smart-point/smart-point-display.hpp"
#include "smart-point/smart-point-ui.hpp"
#include "device/device.hpp"
#include "core/os-wrappers.hpp"
#include "core/device-initializer.hpp"
#include "dev/wav-player.hpp"
#include "dev/matrix-keyboard.hpp"



class SmartPoint : public IAnyDevice
{
public:
	SmartPoint();
	~SmartPoint() {}
	void init(const Pinout& pinout) override;
	void setDafaultPinout(Pinout& pinout) override;
	bool checkPinout(const Pinout& pinout) override;

	DeviceConfiguration deviceConfig;
	SmartPointConfig samrtPointConfig;
	SmartPointState state{samrtPointConfig};
	SmartPointUI ui{state};

private:
	void initSounds();

	TasksPool m_tasksPool{"SmPntPl"};
	SoundPlayer m_systemReadySound;
	SmartPointUI m_ui{state};
};


#endif /* UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_HPP_ */
