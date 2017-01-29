/*
*    Copyright (C) 2017 by Aleksey Bulatov
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
