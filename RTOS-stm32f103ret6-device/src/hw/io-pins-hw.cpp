/*
 * io-pins.cpp
 *
 *  Created on: 20 июня 2015 г.
 *      Author: alexey
 */

#include "hw/io-pins-hw.hpp"
#include "utils/memory.hpp"
#include "core/logging.hpp"

IOPinsPool pool;
IIOPinsPool* IOPins = &pool;

static constexpr uint8_t pinsPerPort = 16;
IOPin* extisListeners[pinsPerPort];

IOPin::IOPin(uint8_t portNumber, uint8_t pinNumber)
{
	m_gpio = parsePortNumber(portNumber);
	m_pinMask = pinNumberToMask(pinNumber);
	switchToInput();
}

bool IOPin::state()
{
	return HAL_GPIO_ReadPin(m_gpio, m_pinMask) == GPIO_PIN_SET;
}


void IOPin::set()
{
	HAL_GPIO_WritePin(m_gpio, m_pinMask, GPIO_PIN_SET);
}

void IOPin::reset()
{
	HAL_GPIO_WritePin(m_gpio, m_pinMask, GPIO_PIN_RESET);
}

void IOPin::switchToOutput()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	zerify(GPIO_InitStructure);
	GPIO_InitStructure.Pin = m_pinMask;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(m_gpio, &GPIO_InitStructure);
	isOutputMode = true;
}

void IOPin::switchToInput()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	zerify(GPIO_InitStructure);
	GPIO_InitStructure.Pin = m_pinMask;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(m_gpio, &GPIO_InitStructure);
	isOutputMode = false;
}

void IOPin::setExtiCallback(const IOPinCallback& callback, bool direclyFromISR)
{
	m_callback = callback;
	m_runDirectlyFromISR = direclyFromISR;
}

void IOPin::enableExti(bool enable)
{
	m_extiEnabled = enable;

	// Determining irq channel
	IRQn_Type irq;

	switch(maskToPinNumber(m_pinMask))
	{
	case 0: irq = EXTI0_IRQn; break;
	case 1: irq = EXTI1_IRQn; break;
	case 2: irq = EXTI2_IRQn; break;
	case 3: irq = EXTI3_IRQn; break;
	case 4: irq = EXTI4_IRQn; break;
	case 5:	case 6:	case 7:	case 8:	case 9:
		irq = EXTI9_5_IRQn; break;
	case 10: case 11: case 12: case 13: case 14: case 15:
		irq = EXTI15_10_IRQn; break;
	default:
		error << "Attempt to enable EXTI on invalid IOPin object";
	}

	if (m_extiEnabled)
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		zerify(GPIO_InitStructure);
		GPIO_InitStructure.Pin = m_pinMask;
		GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
		GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(m_gpio, &GPIO_InitStructure);

		HAL_NVIC_SetPriority(irq, 5, 0);
		extisListeners[maskToPinNumber(m_pinMask)] = this;
		HAL_NVIC_EnableIRQ(irq);
	} else {
		HAL_NVIC_DisableIRQ(irq);
		switchToInput();
	}
}

void IOPin::extiInterrupt()
{
	//info << "In listener";
	if (m_callback)
	{
		if (m_runDirectlyFromISR)
		{
			m_callback(state());
		} else {
			pool.m_callbackCaller.addFromISR(
					//std::bind(m_callback, state()) //< This line does not work :(
					[this]() { m_callback(state()); }
			);
		}
	}
}


GPIO_TypeDef* IOPin::parsePortNumber(uint8_t portNumber)
{
	switch(portNumber)
	{
	case 0:
		return GPIOA;
	case 1:
		return GPIOB;
	case 2:
		return GPIOC;
	case 3:
		return GPIOD;
	default:
		error << "Invalid GPIO port requested";
		return nullptr;
	}
}

uint8_t IOPin::maskToPinNumber(uint16_t pinMask)
{
	switch(pinMask)
	{
	case GPIO_PIN_0: return 0;
	case GPIO_PIN_1: return 1;
	case GPIO_PIN_2: return 2;
	case GPIO_PIN_3: return 3;
	case GPIO_PIN_4: return 4;
	case GPIO_PIN_5: return 5;
	case GPIO_PIN_6: return 6;
	case GPIO_PIN_7: return 7;
	case GPIO_PIN_8: return 8;
	case GPIO_PIN_9: return 9;
	case GPIO_PIN_10: return 10;
	case GPIO_PIN_11: return 11;
	case GPIO_PIN_12: return 12;
	case GPIO_PIN_13: return 13;
	case GPIO_PIN_14: return 14;
	case GPIO_PIN_15: return 15;
	default:
		error << "Invalid pin number requested";
		return 0xFF;
	}
}


IOPinsPool::IOPinsPool()
{
	for (uint8_t i=0; i<pinsPerPort; i++)
		extisListeners[i] = nullptr;

	m_callbackCaller.setStackSize(512);
    m_callbackCaller.setName("IOcc");
	m_callbackCaller.run();
}

uint16_t IOPin::pinNumberToMask(uint8_t pinNumber)
{
	return (1 << pinNumber);
}

IIOPin* IOPinsPool::getIOPin(uint8_t portNumber, uint8_t pinNumber)
{
	if (portNumber >= 8 || pinNumber >= 17)
		return nullptr;
	auto it = m_interfaces.find(std::pair<uint8_t, uint8_t>(portNumber, pinNumber));
	if (it == m_interfaces.end())
	{
		IIOPin* newPin = new IOPin(portNumber, pinNumber);
		m_interfaces[std::pair<uint8_t, uint8_t>(portNumber, pinNumber)] = newPin;
		return newPin;
	} else
		return it->second;
}

void IOPinsPool::resetAllPins()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	zerify(GPIO_InitStructure);
	uint16_t pinMask = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
			GPIO_PIN_4 | GPIO_PIN_5  | GPIO_PIN_6  | GPIO_PIN_7  | GPIO_PIN_8  |
			GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
			GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitStructure.Pin = pinMask;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOA, pinMask, GPIO_PIN_RESET);

	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOB, pinMask, GPIO_PIN_RESET);

	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOC, pinMask, GPIO_PIN_RESET);

	GPIO_InitStructure.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t pinMask)
{
	uint8_t pinNumber = IOPin::maskToPinNumber(pinMask);
	//info << "HAL_GPIO_EXTI_Callback, pn=" << pinNumber;
	if (nullptr != extisListeners[pinNumber])
	{
		//info << "Going to listener";
		extisListeners[pinNumber]->extiInterrupt();
	}
}

