/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
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

#include "ir/ir-physical.hpp"
#include "ir/ir-presentation.hpp"

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

	FUNCION_1P(ConfigCodes::HeadSensor::Functions, SmartPoint, catchShot);

private:
	void initSounds();

	TasksPool m_tasksPool{"SmPntPl"};
	SoundPlayer m_systemReadySound;
	SmartPointUI m_ui{state};

	IIRPhysicalReceiver* m_irPhysicalReceiver = nullptr;
	IIRPresentationReceiver* m_irPresentationReceiver = nullptr;
	IPresentationReceiversGroup* m_irPresentationReceiversGroup = nullptr;
	Interrogator m_MT2Interogator{"MT2Itrg"};
};


#endif /* UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_HPP_ */
