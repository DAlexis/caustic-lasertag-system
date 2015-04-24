/*
 * exti-initialization-options.h
 *
 *  Created on: 29 нояб. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_EXTI_INITIALIZATION_OPTIONS_H_
#define LAZERTAG_RIFLE_INCLUDE_EXTI_INITIALIZATION_OPTIONS_H_

#include "stm32f10x.h"

struct InputDescription
{
    uint32_t RCC_APB2Periph_GPIOx;
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin_x;
    uint8_t GPIO_PortSourceGPIOx;
    uint8_t GPIO_PinSourcex;
    uint32_t EXTI_Linex;
    uint8_t NVIC_IRQChannel;
};


static constexpr InputDescription inputDescription[] = {
    {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_0, GPIO_PortSourceGPIOB, GPIO_PinSource0, EXTI_Line0, EXTI0_IRQn},
    {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_1, GPIO_PortSourceGPIOB, GPIO_PinSource1, EXTI_Line1, EXTI1_IRQn},
    {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_2, GPIO_PortSourceGPIOB, GPIO_PinSource2, EXTI_Line2, EXTI2_IRQn},
    {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_3, GPIO_PortSourceGPIOB, GPIO_PinSource3, EXTI_Line3, EXTI3_IRQn},
    {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_4, GPIO_PortSourceGPIOB, GPIO_PinSource4, EXTI_Line4, EXTI4_IRQn},
};



#endif /* LAZERTAG_RIFLE_INCLUDE_EXTI_INITIALIZATION_OPTIONS_H_ */
