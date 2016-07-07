/*
 * broadcast.hpp
 *
 *  Created on: 14 апр. 2015 г.
 *      Author: alexey
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

	const DeviceAddress any{255, 255, 255};

	bool isBroadcast(const DeviceAddress& addr);

	class IBroadcastTester
	{
	public:
		virtual ~IBroadcastTester() {}
		virtual bool isAcceptableBroadcast(const DeviceAddress& addr) = 0;
	};
};

extern Broadcast broadcast;

#endif /* INCLUDE_RCSP_BROADCAST_HPP_ */
