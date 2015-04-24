/*
 * inputs-interrogator.hpp
 *
 *  Created on: 04 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HAL_INPUTS_INTERROGATOR_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HAL_INPUTS_INTERROGATOR_HPP_

#include <stdint.h>

class IIOPin
{
public:
	virtual ~IIOPin() {}
	virtual bool state() = 0;
	/// Switch to state "1"
	virtual void set() = 0;
	/// Switch to state "0"
	virtual void reset() = 0;

	virtual void switchToOutput() = 0;
	virtual void switchToInput() = 0;
};

class IIOPinsPool
{
public:
	virtual ~IIOPinsPool() {}
	virtual IIOPin* getIOPin(uint8_t portNumber, uint8_t pinNumber) = 0;
};

extern IIOPinsPool* IOPins;


#endif /* LAZERTAG_RIFLE_INCLUDE_HAL_INPUTS_INTERROGATOR_HPP_ */
