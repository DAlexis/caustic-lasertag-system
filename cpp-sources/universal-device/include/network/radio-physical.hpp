/*
 * radio-physical.hpp
 *
 *  Created on: 1 июня 2016 г.
 *      Author: alexey
 */

#ifndef INCLUDE_NETWORK_RADIO_PHYSICAL_HPP_
#define INCLUDE_NETWORK_RADIO_PHYSICAL_HPP_

#include "core/os-wrappers.hpp"

class IRadioPhysicalDevice : public IInterrogatable
{
public:
	virtual ~IRadioPhysicalDevice() {}
	using DataReceiveCallback = std::function<void(uint8_t/* channel*/, uint8_t*/* data*/)>;
	using TXDoneCallback = std::function<void(void)>;

	virtual void printStatus() = 0;
	virtual void setDataReceiveCallback(DataReceiveCallback callback) = 0;
	virtual void setTXDoneCallback(TXDoneCallback callback) = 0;

	virtual void setRXAddress(uint8_t channel, uint8_t* address) = 0;
	virtual void setTXAddress(uint8_t* address) = 0;

	virtual void sendData(uint8_t size, uint8_t* data) = 0;
};







#endif /* INCLUDE_NETWORK_RADIO_PHYSICAL_HPP_ */
