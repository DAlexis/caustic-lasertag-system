/*
 * ir-physical-2.hpp
 *
 *  Created on: 24 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_IR_PHYSICAL_RECEIVER_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_IR_PHYSICAL_RECEIVER_HPP_


#include "hal/impulse-emitter.hpp"
#include "rcsp/base-types.hpp"
#include "hal/io-pins.hpp"
#include "utils/interfaces.hpp"
#include <functional>
#include <stdint.h>

class IIRReceiverPhysical : public IInterrogatable
{
public:
    virtual ~IIRReceiverPhysical() {}
    virtual void getData(uint8_t*& data, uint16_t& size) = 0;
    virtual UintParameter getId() = 0;
    virtual bool isDataReady() = 0;
    virtual void setEnabled(bool enabled) = 0;
};

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_IR_PHYSICAL_RECEIVER_HPP_ */
