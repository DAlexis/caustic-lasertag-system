/*
 * wav-player.cpp
 *
 *  Created on: 09 янв. 2015 г.
 *      Author: alexey
 */

#include "dev/wav-player.hpp"
#include <stdio.h>
#include <string.h>

WavPlayer* WavPlayer::m_wavPlayer = nullptr;
STATIC_DEINITIALIZER_IN_CPP_FILE(WavPlayer, m_wavPlayer);

WavPlayer& WavPlayer::instance()
{
	if (!m_wavPlayer)
		m_wavPlayer = new WavPlayer;
	return *m_wavPlayer;
}

void WavPlayer::setVerbose(bool verbose)
{
	m_verbose = verbose;
}

bool WavPlayer::loadFile(const char* fileName)
{
	FRESULT res;
	printf("Opening file...\n");
	res = f_open(&m_fil, fileName, FA_OPEN_EXISTING | FA_READ);
	if (res)
		return false;

	if (!readHeader())
		return false;

	if (m_verbose)
		printf("File loaded\n");
	return true;
}

void WavPlayer::play()
{

}

bool WavPlayer::readHeader()
{
	FRESULT res;
	UINT readed = 0;
	res = f_read (&m_fil, &m_header, sizeof(m_header), &readed);
	if (readed != sizeof(m_header))
	{
		if (m_verbose) printf("Incomplete wav file.\n");
		return false;
	}

	//if (m_verbose) printInfo();

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

void WavPlayer::fragmentDoneCallback(SoundSample* oldBuffer)
{

}

