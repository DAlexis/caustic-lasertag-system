/*
 * inputs-interrogator.hpp
 *
 *  Created on: 04 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HAL_INPUTS_INTERROGATOR_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HAL_INPUTS_INTERROGATOR_HPP_

#include <stdint.h>

class IInputInterrogator
{
public:
	virtual ~IInputInterrogator() {}
	virtual bool interrogate() = 0;
};

class IInputInterrogatorsPool
{
public:
	virtual ~IInputInterrogatorsPool() {}
	virtual IInputInterrogator* getInputInterrogator(uint8_t portNumber, uint8_t pinNumber) = 0;
};

extern IInputInterrogatorsPool* inputInterrogators;


#endif /* LAZERTAG_RIFLE_INCLUDE_HAL_INPUTS_INTERROGATOR_HPP_ */
