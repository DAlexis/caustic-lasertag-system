/*
 * ext-interrupts.cpp
 *
 *  Created on: 24 дек. 2014 г.
 *      Author: alexey
 */

#include "hw/exti-interrupts.hpp"
#include "stm32f10x.h"

EXTISPool EXTISPoolInstance;
IEXTISPool* EXTIS = &EXTISPoolInstance;

using EXTI_IRQHandler = std::function<void()>;

EXTI_IRQHandler exti_IRQHandlers[EXTI_LINES_COUNT];

namespace {

	struct PinDescription
	{
		uint16_t GPIO_Pin_x;
		uint8_t GPIO_PinSourcex;
		uint32_t EXTI_Linex;
		uint8_t NVIC_IRQChannel;
	};

	struct PortDescription
	{
		uint32_t RCC_APB2Periph_GPIOx;
		GPIO_TypeDef* GPIOx;
		uint8_t GPIO_PortSourceGPIOx;
	};

	constexpr PinDescription pinDescription[] = {
		{GPIO_Pin_0, GPIO_PinSource0, EXTI_Line0, EXTI0_IRQn},
		{GPIO_Pin_1, GPIO_PinSource1, EXTI_Line1, EXTI1_IRQn},
		{GPIO_Pin_2, GPIO_PinSource2, EXTI_Line2, EXTI2_IRQn},
		{GPIO_Pin_3, GPIO_PinSource3, EXTI_Line3, EXTI3_IRQn},
		{GPIO_Pin_4, GPIO_PinSource4, EXTI_Line4, EXTI4_IRQn},
		{GPIO_Pin_5, GPIO_PinSource5, EXTI_Line5, EXTI9_5_IRQn},
		{GPIO_Pin_6, GPIO_PinSource6, EXTI_Line6, EXTI9_5_IRQn},
		{GPIO_Pin_7, GPIO_PinSource7, EXTI_Line7, EXTI9_5_IRQn},
		{GPIO_Pin_8, GPIO_PinSource8, EXTI_Line8, EXTI9_5_IRQn},
		{GPIO_Pin_9, GPIO_PinSource9, EXTI_Line9, EXTI9_5_IRQn},
	};

	constexpr PortDescription portDescription[] = {
		{RCC_APB2Periph_GPIOA, GPIOA, GPIO_PortSourceGPIOA},
		{RCC_APB2Periph_GPIOB, GPIOB, GPIO_PortSourceGPIOB},
		{RCC_APB2Periph_GPIOC, GPIOC, GPIO_PortSourceGPIOC},
	};
}

ExternalInterruptManager::ExternalInterruptManager(uint8_t pinNumber) :
		ExternalInterruptManagerBase(pinNumber)
{
}

void ExternalInterruptManager::turnOn()
{
	// Enable and set EXTIx Interrupt to the lowest priority
	NVIC_InitTypeDef   NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = pinDescription[m_pinNumber].NVIC_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void ExternalInterruptManager::turnOff()
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = pinDescription[m_pinNumber].NVIC_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void ExternalInterruptManager::init(uint8_t portNumber)
{
	m_portNumber = portNumber;

	exti_IRQHandlers[m_pinNumber] = std::bind(&ExternalInterruptManager::IRQHandler, this);

	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;


	RCC_APB2PeriphClockCmd(portDescription[m_portNumber].RCC_APB2Periph_GPIOx, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = pinDescription[m_pinNumber].GPIO_Pin_x;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(portDescription[m_portNumber].GPIOx, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_EXTILineConfig(portDescription[m_portNumber].GPIO_PortSourceGPIOx, pinDescription[m_pinNumber].GPIO_PinSourcex);

	// Configure EXTIx line
	EXTI_InitStructure.EXTI_Line = pinDescription[m_pinNumber].EXTI_Linex;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;

	EXTI_Init(&EXTI_InitStructure);

	//turnOn();
}

void ExternalInterruptManager::IRQHandler()
{
	uint8_t status = GPIO_ReadInputDataBit(portDescription[m_portNumber].GPIOx, pinDescription[m_pinNumber].GPIO_Pin_x);
	m_callback(status ? true : false);
}

extern "C" {
	void EXTI0_IRQHandler()
	{
		if(EXTI_GetITStatus(EXTI_Line0) != RESET)
		{
			exti_IRQHandlers[0]();
			EXTI_ClearITPendingBit(EXTI_Line0);
		}
	}

	void EXTI1_IRQHandler()
	{
		if(EXTI_GetITStatus(EXTI_Line1) != RESET)
		{
			exti_IRQHandlers[1]();
			EXTI_ClearITPendingBit(EXTI_Line1);
		}
	}

	void EXTI2_IRQHandler()
	{
		if(EXTI_GetITStatus(EXTI_Line2) != RESET)
		{
			exti_IRQHandlers[2]();
			EXTI_ClearITPendingBit(EXTI_Line2);
		}
	}

	void EXTI3_IRQHandler()
	{
		if(EXTI_GetITStatus(EXTI_Line3) != RESET)
		{
			exti_IRQHandlers[3]();
			EXTI_ClearITPendingBit(EXTI_Line3);
		}
	}

	void EXTI4_IRQHandler()
	{
		if(EXTI_GetITStatus(EXTI_Line4) != RESET)
		{
			exti_IRQHandlers[4]();
			EXTI_ClearITPendingBit(EXTI_Line4);
		}
	}

	void EXTI9_5_IRQHandler()
	{
		if(EXTI_GetITStatus(EXTI_Line5) != RESET)
		{
			exti_IRQHandlers[5]();
			EXTI_ClearITPendingBit(EXTI_Line5);
		}

		if(EXTI_GetITStatus(EXTI_Line6) != RESET)
		{
			exti_IRQHandlers[6]();
			EXTI_ClearITPendingBit(EXTI_Line6);
		}

		if(EXTI_GetITStatus(EXTI_Line7) != RESET)
		{
			exti_IRQHandlers[7]();
			EXTI_ClearITPendingBit(EXTI_Line7);
		}

		if(EXTI_GetITStatus(EXTI_Line8) != RESET)
		{
			exti_IRQHandlers[8]();
			EXTI_ClearITPendingBit(EXTI_Line8);
		}

		if(EXTI_GetITStatus(EXTI_Line9) != RESET)
		{
			exti_IRQHandlers[9]();
			EXTI_ClearITPendingBit(EXTI_Line9);
		}
	}

}

//////////////////
// EXTISPool
EXTISPool::EXTISPool()
{
	for (int i=0; i<EXTI_LINES_COUNT; i++)
		m_EXTIS[i] = nullptr;
}

EXTISPool::~EXTISPool()
{
	for (int i=0; i<EXTI_LINES_COUNT; i++)
		if (m_EXTIS[i])
			delete m_EXTIS[i];
}

ExternalInterruptManagerBase* EXTISPool::getEXTI(uint8_t pinNumber)
{
	if (!m_EXTIS[pinNumber]) {
		m_EXTIS[pinNumber] = new ExternalInterruptManager(pinNumber);
	}
	return m_EXTIS[pinNumber];
}
