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

#ifndef INCLUDE_RCSP_BROADCAST_HPP_
#define INCLUDE_RCSP_BROADCAST_HPP_

#include "network/network-base-types.hpp"

class Broadcast
{
public:
	const DeviceAddress headSensorsRed{255, 255, 0};
	const DeviceAddress headSensorsBlue{255, 255, 1};
	const DeviceAddress headSensorsYellow{255, 255, 2};
	const DeviceAddress headSensorsGreen{255, 255, 3};
	const DeviceAddress headSensors{255, 255, 4};

	const DeviceAddress rifles{255, 255, 10};

	const DeviceAddress bluetoothBridges{255, 255, 20};

	const DeviceAddress smartPoint{255, 255, 30};

	//const DeviceAddress statisticsReceivers{255, 255, 40};

	const DeviceAddress any{255, 255, 255};

	/** For all devices except bluetooth bridges for now.
	 * To prevend discovering bridges by themselves
	 */
	const DeviceAddress anyGameDevice{255, 255, 254};

	static bool isBroadcast(const DeviceAddress& addr);

	class IBroadcastTester
	{
	public:
		virtual ~IBroadcastTester() {}
		virtual bool isAcceptableBroadcast(const DeviceAddress& addr) = 0;
	};
};

extern Broadcast broadcast;

#endif /* INCLUDE_RCSP_BROADCAST_HPP_ */
