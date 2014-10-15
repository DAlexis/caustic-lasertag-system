/*
 * sound.cpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#include "sound.hpp"
#include <stdio.h>

void SoundPlayer::init()
{
}

void SoundPlayer::playWav(const char* filename)
{

}

void SoundPlayer::stop()
{
}

bool SoundPlayer::readHeader()
{
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
