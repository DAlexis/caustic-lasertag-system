/*
 * package-former.hpp
 *
 *  Created on: 26 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_

#include "logic/device.hpp"

#pragma pack(push, 1)
struct PackageIdAndTTL
{
	uint16_t packageId : 14;
	uint16_t TTL : 2;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Package
{
	DeviceAddress sender;
	DeviceAddress target;
	PackageIdAndTTL idAndTTL;
};
#pragma pack(pop)

class PackageSender
{
public:
	void send(DeviceAddress, uint8_t* data, uint16_t size);

	DeviceAddress self;
};








#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_ */
