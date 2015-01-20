/*
 * input-interrogators.hpp
 *
 *  Created on: 04 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HW_INPUT_INTERROGATORS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HW_INPUT_INTERROGATORS_HPP_

#include "hal/io-pins.hpp"
#include "stm32f10x.h"
#include "map"

class IOPin : public IIOPin
{
public:
	IOPin(GPIO_TypeDef* gpio, uint16_t pinMask);

	bool state();

	/// Switch to state "1"
	void set();
	/// Switch to state "0"
	void reset();

	void switchToOutput();
	void switchToInput();
private:

	GPIO_TypeDef* m_gpio;
	uint16_t m_pinMask;
	bool isOutputMode = true;
};

class IOPinsPool : public IIOPinsPool
{
public:
	IIOPin* getIOPin(uint8_t portNumber, uint8_t pinNumber);

private:
	void createIOPin(uint8_t portNumber, uint8_t pinNumber);
	std::map<std::pair<uint8_t, uint8_t>, IOPin*> m_interrogators;
};




#endif /* LAZERTAG_RIFLE_INCLUDE_HW_INPUT_INTERROGATORS_HPP_ */
