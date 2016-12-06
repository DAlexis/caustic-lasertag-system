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

#ifndef UNIVERSAL_DEVICE_INCLUDE_BLUETOOTH_BRIDGE_BLUETOOTH_BRIDGE_CONFIG_HPP_
#define UNIVERSAL_DEVICE_INCLUDE_BLUETOOTH_BRIDGE_BLUETOOTH_BRIDGE_CONFIG_HPP_

#include "rcsp/RCSP-base-types.hpp"
#include "rcsp/base-types.hpp"
#include "rcsp/operation-codes.hpp"
#include "rcsp/RCSP-aggregator.hpp"
#include "network/network-base-types.hpp"

class BluetoothBridgeConfiguration
{
public:
	BluetoothBridgeConfiguration();
	void setDefault();

	PAR_ST(RESTORABLE, ConfigCodes::BluetoothBridge::Configuration, bluetoothPin);
};

#endif /* UNIVERSAL_DEVICE_INCLUDE_BLUETOOTH_BRIDGE_BLUETOOTH_BRIDGE_CONFIG_HPP_ */
