/*
 * sound.cpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#include "sound.hpp"
#include "ff.h"
#include "dynamic-memory.hpp"
#include <stdio.h>
#include <string.h>

SoundPlayer* SoundPlayer::m_self = nullptr;

SoundPlayer& SoundPlayer::getInstance()
{
    if (m_self == nullptr) {
        createInstance(m_self);
    }
    return *m_self;
}

void SoundPlayer::setVerbose(bool flag)
{
}

void SoundPlayer::init()
{
}

void SoundPlayer::playWav(const char* filename)
{
    FRESULT res;
    res = f_open(&fil, filename, FA_OPEN_EXISTING | FA_READ); // open existing file in read and write mode
    if (res)
    {
        if (m_verbose) printf("Cannot open file!\n");
        return;
    }

}

void SoundPlayer::stop()
{
}

bool SoundPlayer::readHeader()
{
    FRESULT res;
    UINT readed = 0;
    res = f_read (&fil, &m_header, sizeof(m_header), &readed);
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
