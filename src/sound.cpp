/*
 * sound.cpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#include "sound.hpp"
#include "ff.h"
#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>

#define CLOCK_FREQ          24000000//12000000/*2*13439475*/ //2*13434400
#define SAMPLE_RATE         44100

/*
#define SAMPLE_TIMER_PRESCALER      (CLOCK_FREQ / 2 / SAMPLE_RATE)
#define FRAGMENT_TIMER_PERIOD         (2*SOUND_BUFFER_SIZE)-1

#define PWM_PRESCALER       0
#define PWM_PERIOD          ((2*CLOCK_FREQ) / (PWM_PRESCALER + 1) / SAMPLE_RATE)
*/

#define PWM_PRESCALER       0
#define PWM_PERIOD          ( (CLOCK_FREQ) / (PWM_PRESCALER+1) / (SAMPLE_RATE) )

#define FRAGMENT_TIMER_RPESCALER    (PWM_PRESCALER+1)*100-1
#define FRAGMENT_TIMER_PERIOD       (PWM_PERIOD*SOUND_BUFFER_SIZE/100)-1

SoundPlayer sound;

SoundPlayer::SoundPlayer() :
    m_verbose(true),
    m_needStop(false),
    m_pCurrentBuffer(&m_buffer1),
    m_pNextBuffer(&m_buffer2)
{
    memset(&m_buffer1, 0, sizeof(m_buffer1));
    memset(&m_buffer2, 0, sizeof(m_buffer2));
}

void SoundPlayer::setVerbose(bool flag)
{
    m_verbose = flag;
}

void SoundPlayer::printParameters()
{
    printf("FRAGMENT_TIMER_RPESCALER: %d, PWM_PERIOD: %d\n", FRAGMENT_TIMER_RPESCALER, PWM_PERIOD);
}

void SoundPlayer::init()
{
    printf("Sound system initialization...\n");
    printParameters();
    initPWMTimer();
    initSamplerTimer();
    initFragmentTimer();
}

void SoundPlayer::playWav(const char* filename)
{
    FRESULT res;
    res = f_open(&m_fil, filename, FA_OPEN_EXISTING | FA_READ); // open existing file in read and write mode
    if (res)
    {
        if (m_verbose) printf("Cannot open file!\n");
        return;
    }
    if (!readHeader())
        return;
    m_needStop = false;
    loadFragment(m_pCurrentBuffer);
    loadFragment(m_pNextBuffer);
    playCurrentBuffer();
    //printf("First fragment played\n");
}

void SoundPlayer::stop()
{
    m_needStop = true;
}

bool SoundPlayer::readHeader()
{
    FRESULT res;
    UINT readed = 0;
    res = f_read (&m_fil, &m_header, sizeof(m_header), &readed);
    if (readed != sizeof(m_header))
    {
        if (m_verbose) printf("Incomplete wav file.\n");
        return false;
    }

    if (m_verbose) printInfo();

    // Validating m_header
    if (strncmp(m_header.riff, "RIFF", 4) != 0)
    {
        if (m_verbose) printf("Invalid format: not RIFF\n");
        return false;
    }
    if (m_header.audio_format != 1)
    {
        if (m_verbose) printf("Invalid audio format: %u; Only PCM audio supported\n", m_header.audio_format);
        return false;
    }
    if (m_header.num_channels != 1)
    {
        if (m_verbose) printf("Invalid channels count: %u; Only mono sound supported\n", m_header.num_channels);
        return false;
    }
    if (m_header.bits_per_sample != 16)
    {
        if (m_verbose) printf("Invalid bits per sample rate: %u; Only 16 supported\n", m_header.bits_per_sample);
        return false;
    }
    return true;
}

void SoundPlayer::printInfo()
{
    printf("main header: %c%c%c%c\n", m_header.riff[0], m_header.riff[1], m_header.riff[2], m_header.riff[3]);
    printf("main chunk_size: %d\n", (int)m_header.chunk_size);
    printf("format: %c%c%c%c\n", m_header.format[0], m_header.format[1], m_header.format[2], m_header.format[3]);
    printf("subchunk1_id: %c%c%c%c\n", m_header.subchunk1_id[0], m_header.subchunk1_id[1], m_header.subchunk1_id[2], m_header.subchunk1_id[3]);
    printf("subchunk1 size: %d\n", (int)m_header.subchunk1_size);
    printf("audio_format: %u\n", m_header.audio_format);
    printf("num_channels: %u\n", m_header.num_channels);
    printf("byte_rate: %d\n", (int)m_header.byte_rate);
    printf("sample_rate: %d\n", (int)m_header.sample_rate);
    printf("block_align: %u\n", m_header.block_align);
    printf("bits_per_sample: %u\n", m_header.bits_per_sample);
    printf("subchunk2_id: %c%c%c%c\n", m_header.subchunk2_id[0], m_header.subchunk2_id[1], m_header.subchunk2_id[2], m_header.subchunk2_id[3]);
    printf("subchunk2_size: %d\n", (int)m_header.subchunk2_size);
}

void SoundPlayer::swapBuffers()
{
    if (m_pCurrentBuffer == &m_buffer1) {
        m_pCurrentBuffer = &m_buffer2;
        m_pNextBuffer = &m_buffer1;
    } else {
        m_pCurrentBuffer = &m_buffer1;
        m_pNextBuffer = &m_buffer2;
    }
}

void SoundPlayer::normalizeBuffer(AudioBuffer* buffer)
{/*
    unsigned int div = 32767 / (PWM_PERIOD/2);
    int16_t* source = reinterpret_cast<int16_t*>(buffer->buffer);*/
    for (int i=0; i<buffer->size; i++)
    {
        buffer->buffer[i] = m_tmpBuffer[i] + 32767;
        //to[i] = to[i] >> 4;
        buffer->buffer[i] = ((unsigned int) buffer->buffer[i] * (PWM_PERIOD/2)) / (32767);
        /*
        buffer->buffer[i] = ( ( (int) source[i] ) + 32767) / div;
        */
/*
        buffer->buffer[i] = source[i] + 32767;
        buffer->buffer[i] = ((unsigned int) buffer->buffer[i] * (PWM_PERIOD/2)) / (32767);*/
        //buffer->buffer[i] = ((int)i*PWM_PERIOD)/buffer->size;
        //printf("new: %u, source: %d\n", buffer->buffer[i], m_tmpBuffer[i]    );
    }
}

bool SoundPlayer::loadFragment(AudioBuffer* buffer)
{
    FRESULT res;
    if (m_needStop) {
        //m_needStop = false;
        return true;
    }

    UINT readed = 0;
    res = f_read(&m_fil, /*buffer->buffer*/ m_tmpBuffer, SOUND_BUFFER_SIZE*sizeof(int16_t), &readed);
    //printf("r\n");
    if (res != FR_OK)
    {
        //stop_playing();
        return false;
    }

    buffer->size = readed / sizeof(uint16_t);
    normalizeBuffer(buffer);
    return true;
}

void SoundPlayer::initPWMTimer()
{
    //////////////////////
    // GPIO initialization - PWM output pins
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //////////////////////
    // TIM16 initialization - timer for sampler
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);

    //Time initialization
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //Control with dead zone.
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //Counter direction
    TIM_TimeBaseInitStructure.TIM_Prescaler = PWM_PRESCALER;   //Timer clock = sysclock /(TIM_Prescaler+1) = 168M - ????
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInitStructure.TIM_Period = PWM_PERIOD; //Period = (TIM counter clock / TIM output clock) - 1
    TIM_TimeBaseInit(TIM16,&TIM_TimeBaseInitStructure);

//    TIM_SelectOutputTrigger(TIM16, TIM_TRGOSource_Update);

    TIM_OCInitTypeDef TIM_OCInitStructure;// = {0,};
    TIM_OCStructInit(&TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = PWM_PERIOD / 2;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM16,&TIM_OCInitStructure);

    // Automatic Output enable, Break, dead time and lock configuration
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
    TIM_BDTRStructInit(&TIM_BDTRInitStructure);

    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
    TIM_BDTRInitStructure.TIM_DeadTime = 11;
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;

    TIM_BDTRConfig(TIM16, &TIM_BDTRInitStructure);

    TIM_OC1PreloadConfig(TIM16, TIM_OCPreload_Enable);
    TIM_Cmd(TIM16, ENABLE);
    TIM_CtrlPWMOutputs(TIM16, ENABLE);
}

void SoundPlayer::initSamplerTimer()
{/*
    // Adjusting TIM7
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Period        = 1; // -> frequency
    TIM_TimeBaseStructure.TIM_Prescaler     = SAMPLE_TIMER_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
    TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update); // Trigger on
    TIM_Cmd(TIM7, ENABLE);
    */
}

void SoundPlayer::initFragmentTimer()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_TimeBaseStructInit(&TIM_InitStructure);
    TIM_InitStructure.TIM_Prescaler = FRAGMENT_TIMER_RPESCALER;
    TIM_InitStructure.TIM_Period = FRAGMENT_TIMER_PERIOD;
    TIM_TimeBaseInit(TIM6, &TIM_InitStructure);


    // To prevent interruption after initialization
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    TIM_ITConfig(TIM6, TIM_DIER_UIE, ENABLE);
}

void SoundPlayer::initDMA(AudioBuffer* source)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel6);

    DMA_InitTypeDef DMA_InitStructure;
    DMA_StructInit(&DMA_InitStructure);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(TIM16->CCR1) ;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(source->buffer);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = (source->size);
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel6, ENABLE);
}

void SoundPlayer::playCurrentBuffer()
{
    if (m_needStop || m_pCurrentBuffer->size == 0) {
        m_needStop = true;
        return;
    }


    //DAC_Cmd(DAC_CHANNEL, ENABLE);
    //DMA_Cmd(DAC_DMA_STREAM, ENABLE);      // Включение DMA
    //DAC_DMACmd(DAC_CHANNEL, ENABLE);  // Включение связки DAC-DMA

    // PWM part

    initDMA(m_pCurrentBuffer);
    PWMTimerEnable();
    fragmentTimerEnable();
}

void SoundPlayer::fragmentTimerEnable()
{
    TIM_Cmd(TIM6, ENABLE);
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
}


void SoundPlayer::fragmentTimerDisable()
{
    TIM_Cmd(TIM6, DISABLE);
    NVIC_DisableIRQ(TIM6_DAC_IRQn);
}


void SoundPlayer::PWMTimerEnable()
{
    TIM_Cmd(TIM16, ENABLE);
    TIM_CtrlPWMOutputs(TIM16, ENABLE);
    DMA_Cmd(DMA1_Channel6, ENABLE);
    TIM_DMACmd(TIM16, TIM_DMA_CC1, ENABLE);
}

void SoundPlayer::PWMTimerDisable()
{
    TIM_Cmd(TIM16, DISABLE);
    //TIM_CtrlPWMOutputs(TIM16, DISABLE);
    DMA_Cmd(DMA1_Channel6, DISABLE);
    TIM_DMACmd(TIM16, TIM_DMA_CC1, DISABLE);
}

void SoundPlayer::stopTimers()
{
    PWMTimerDisable();
    fragmentTimerDisable();
    printf("sound end\n");
}

void SoundPlayer::fragmentTimerIRQ()
{
    //PWMTimerDisable();

    if (m_needStop)
    {
        stopTimers();
        return;
    }

    swapBuffers();
    playCurrentBuffer();
    if (!loadFragment(m_pNextBuffer)) {

        m_needStop = true;
        stopTimers();
    }
}

extern "C" {
    void TIM6_DAC_IRQHandler(void)
    {
        if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
        {
            TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
            sound.fragmentTimerIRQ();
        }
    }
}
