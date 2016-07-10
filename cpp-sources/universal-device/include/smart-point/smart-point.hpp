/*
 * smart-point.hpp
 *
 *  Created on: 9 июл. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_HPP_
#define UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_HPP_

#include "device/device.hpp"
#include "smart-point/smart-point-state.hpp"
#include "core/os-wrappers.hpp"
#include "core/device-initializer.hpp"

class SmartPoint : public IAnyDevice
{
public:
	SmartPoint();
	~SmartPoint() {}
	void init(const Pinout& pinout) override;
	void setDafaultPinout(Pinout& pinout) override;
	bool checkPinout(const Pinout& pinout) override;

	DeviceConfiguration deviceConfig;
	SmartPointState state;

private:
	TasksPool m_tasksPool{"SmPntPl"};
};


#endif /* UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_HPP_ */
