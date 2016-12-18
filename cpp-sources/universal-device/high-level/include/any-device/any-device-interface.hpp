/*
 * any-device-interface.hpp
 *
 *  Created on: 18 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_DEVICE_INTERFACE_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_DEVICE_INTERFACE_HPP_

#include "core/pinout.hpp"

class RCSPAggregator;

class IAnyDevice
{
public:
    virtual ~IAnyDevice() {}
    virtual void init(const Pinout& pinout, bool isSdcardOk) = 0;
    virtual void setDefaultPinout(Pinout& pinout) = 0;
    virtual bool checkPinout(const Pinout& pinout) = 0;
};

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_DEVICE_INTERFACE_HPP_ */
