/*
 * fragment-player.cpp
 *
 *  Created on: 09 янв. 2015 г.
 *      Author: alexey
 */

#include "hw/fragment-player.hpp"

#include <stdio.h>

#define DAC_DHR12R1_ADDRESS   0x40007408
#define DAC_DHR12R2_ADDRESS   0x40007414
#define DAC_DHR8R1_ADDRESS    0x40007410

FragmentPlayer fragmentPlayerInstance;
IFragmentPlayer *fragmentPlayer = &fragmentPlayerInstance;

FragmentPlayer::FragmentPlayer()
{
	// Initialization example can be found at
	// STM32F10x_StdPeriph_Lib_V3.5.0/Project/STM32F10x_StdPeriph_Examples/DAC/OneChannelDMA_Escalator

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	/* Once the DAC channel is enabled, the corresponding GPIO pin is automatically
	 connected to the DAC converter. In order to avoid parasitic consumption,
	 the GPIO pin should be configured in analog */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Prescaler = SystemCoreClock / 44100 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInitStructure.TIM_Period = 1;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStructure);
/*
	TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable);
	TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);
*/

//	TIM_PrescalerConfig(TIM6, 0xF, TIM_PSCReloadMode_Update);
	//TIM_SetAutoreload(TIM6, 0xFF);
	// TIM6 TRGO selection
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

	DAC_InitTypeDef DAC_InitStructure;
	DAC_StructInit(&DAC_InitStructure);

	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	// Filling most part of fields of m_DMA_InitStructure that are always identical
	DMA_StructInit(&m_DMA_InitStructure);
	m_DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_ADDRESS;
	//m_DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
	m_DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	//m_DMA_InitStructure.DMA_BufferSize = 6;
	m_DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	m_DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	m_DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	m_DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	//m_DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	m_DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	m_DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	m_DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void FragmentPlayer::playFragment(SoundSample* buffer)
{
	m_buffer = buffer;
	//printf("Enabling DMA\n");
	DMA_DeInit(DMA2_Channel3);
	m_DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
	m_DMA_InitStructure.DMA_BufferSize = m_size;
	DMA_Init(DMA2_Channel3, &m_DMA_InitStructure);
	DMA_ITConfig(DMA2_Channel3, DMA_IT_TC, ENABLE);
	// Enable DMA2 Channel4
	DMA_Cmd(DMA2_Channel3, ENABLE);

	//printf("Enabling DAC\n");
	// Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
	// automatically connected to the DAC converter.
	DAC_Cmd(DAC_Channel_1, ENABLE);


	// Enable DMA for DAC Channel1
	DAC_DMACmd(DAC_Channel_1, ENABLE);
	//printf("Starting TIM6\n");
	// TIM6 enable counter
	TIM_Cmd(TIM6, ENABLE);

}

void FragmentPlayer::stopFragment()
{
	DMA_ITConfig(DMA2_Channel3, DMA_IT_TC, DISABLE);
	TIM_Cmd(TIM6, DISABLE);
	DAC_DMACmd(DAC_Channel_1, DISABLE);
}

void FragmentPlayer::DMAInterruptionHandler()
{
	stopFragment();
	if (m_callback != nullptr)
		m_callback(m_buffer);
	//printf("DMA int handler\n");
}

extern "C" void DMA2_Channel3_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA2_IT_TC3) != RESET)
	{
		DMA_ClearITPendingBit(DMA2_IT_TC3);
		fragmentPlayerInstance.DMAInterruptionHandler();
	}
}

