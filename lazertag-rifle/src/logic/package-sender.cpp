/*
 * package-sender.cpp
 *
 *  Created on: 28 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/package-sender.hpp"

#include <stdio.h>
#include <string.h>

void PackageSender::init()
{
	printf("Package sender initialization...\n");
	nrf.setTXDoneCallback(std::bind(&PackageSender::TXDoneCallback, this));
	nrf.init(
		IOPins->getIOPin(1, 7),
		IOPins->getIOPin(1, 12),
		IOPins->getIOPin(1, 8),
		nullptr,
		SPIs->getSPI(1)
	);
	nrf.printStatus();
}

uint16_t PackageSender::send(DeviceAddress target, uint8_t* data, uint16_t size, bool waitForAck, PackageSendingDoneCallback doneCallback)
{
	if (waitForAck)
	{
		if (size>Package::payloadLength)
			return 0;
		PackageIdAndTTL idAndTTL;
		idAndTTL.packageId = (systemClock->getTime()) & 0x3FFF;
		idAndTTL.TTL = 0;

		m_packagesNoAck.push_back(Package());
		m_packagesNoAck.back().sender = self;
		m_packagesNoAck.back().target = target;
		m_packagesNoAck.back().idAndTTL = idAndTTL;
		memcpy(m_packagesNoAck.back().payload, data, size);
		if (size<Package::payloadLength)
			memset(m_packagesNoAck.back().payload+size, 0, size-Package::payloadLength);
		return 0;
	} else {

	}
}

void PackageSender::TXDoneCallback()
{

}

void PackageSender::interrogate()
{
}
