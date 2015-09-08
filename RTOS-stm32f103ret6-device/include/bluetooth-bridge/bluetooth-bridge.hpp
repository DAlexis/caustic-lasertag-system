/*
 * bluetooth-bridge.cpp
 *
 *  Created on: 7 сент. 2015 г.
 *      Author: alexey
 */

#include "core/device-initializer.hpp"

class BluetoothBridge : public IAnyDevice
{
public:
	void init(const Pinout& pinout);
	void setDafaultPinout(Pinout& pinout);
	bool checkPinout(const Pinout& pinout);
};
