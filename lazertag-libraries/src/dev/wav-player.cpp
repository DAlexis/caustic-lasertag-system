/*
 * wav-player.cpp
 *
 *  Created on: 09 янв. 2015 г.
 *      Author: alexey
 */

#include "dev/wav-player.hpp"
#include "dev/random.hpp"
#include <stdio.h>
#include <string.h>

WavPlayer* WavPlayer::m_wavPlayer = nullptr;
STATIC_DEINITIALIZER_IN_CPP_FILE(WavPlayer, m_wavPlayer);

WavPlayer::WavPlayer()
{
	printf("Creating audio buffers\n");
	m_buffer1 = new SoundSample[audioBufferSize];
	m_buffer2 = new SoundSample[audioBufferSize];
	printf("Testing audio buffers\n");
	for (int i=0; i<audioBufferSize; i++)
	{
		m_buffer1[i] = 0xFFFF;
		m_buffer2[i] = 0xFFFF;
	}
	printf("Done\n");
}

WavPlayer::~WavPlayer()
{
	delete[] m_buffer1;
	delete[] m_buffer2;
}


void WavPlayer::init()
{
	fragmentPlayer->setFragmentDoneCallback(std::bind(&WavPlayer::fragmentDoneCallback, this, std::placeholders::_1));
}

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
	fragmentPlayer->stop();
	FRESULT res;
	printf("Opening file...\n");
	m_fileIsOpened = false;
	res = f_open(&m_fil, fileName, FA_OPEN_EXISTING | FA_READ);
	if (res)
	{
		return false;
	}

	if (!readHeader())
	{
		f_close(&m_fil);
		return false;
	}

	if (m_verbose)
		printf("File loaded\n");

	if (!loadFragment(m_buffer1))
	{
		if (m_verbose)
			printf("Cannot load first fragment from audio file\n");
		f_close(&m_fil);
		return false;
	}

	if (!loadFragment(m_buffer2))
	{
		if (m_verbose)
			printf("Cannot load second fragment from audio file\n");
		f_close(&m_fil);
		return false;
	}

	m_fileIsOpened = true;
	return true;
}

void WavPlayer::play()
{
	if (!m_fileIsOpened)
	{
		printf("Cannot play, file wasn't opened\n");
	}
	fragmentPlayer->stop();
	fragmentPlayer->setFragmentSize(audioBufferSize);
	fragmentPlayer->playFragment(m_buffer1);
	m_isPlaying = true;
}

void WavPlayer::stop()
{
	fragmentPlayer->stop();
	m_isPlaying = false;
	closeFile();
}

void WavPlayer::fragmentDoneCallback(SoundSample* oldBuffer)
{
	if (m_lastBufferSize == 0)
	{
		m_isPlaying = false;
		closeFile();
		return;
	}
	fragmentPlayer->setFragmentSize(m_lastBufferSize);
	if (oldBuffer == m_buffer1) {
		fragmentPlayer->playFragment(m_buffer2);
		loadFragment(m_buffer1);
	} else {
		fragmentPlayer->playFragment(m_buffer1);
		loadFragment(m_buffer2);
	}
}

bool WavPlayer::readHeader()
{
	FRESULT res;
	UINT readed = 0;
	res = f_read (&m_fil, &m_header, sizeof(m_header), &readed);
	if (res != FR_OK)
	{
		if (m_verbose) printf("Cannot read header from file.\n");
		return false;
	}

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

bool WavPlayer::loadFragment(SoundSample* m_buffer)
{
	FRESULT res;
	UINT readed = 0;
	UINT readedNow = 0;
	void* ptr = m_buffer;
	int16_t *tmpPrt = (int16_t *)ptr;

	// For some unknown reason, f_read does not read more about 1k and crashes
	/// @todo increase blockSize to maximum
	uint8_t* curs = (uint8_t*) ptr;
	constexpr uint16_t blockSize = 400;
	constexpr uint16_t count = audioBufferSize*sizeof(int16_t) / blockSize;
	constexpr uint16_t tail = audioBufferSize*sizeof(int16_t) % blockSize;
	for (uint16_t i=0; i<count; i++)
	{
		res = f_read(&m_fil, curs, blockSize, &readedNow);
		if (res != FR_OK)
		{
			m_lastBufferSize = 0;
			closeFile();
			return false;
		}
		curs += readedNow;
		readed += readedNow;
	}

	if (tail != 0) {
		res = f_read(&m_fil, curs, tail, &readedNow);
		if (res != FR_OK)
		{
			m_lastBufferSize = 0;
			closeFile();
			return false;
		}
		readed += readedNow;
	}

	//res = f_read(&m_fil, tmpPrt, AUDIO_BUFFER_SIZE*sizeof(int16_t), &readed);


	m_lastBufferSize = readed / sizeof(int16_t);

	// Decoding int16_t 16 bit -> uint16_t 12 bit
	for (unsigned int i=0; i<m_lastBufferSize; i++)
		m_buffer[i] = (tmpPrt[i] + INT16_MAX) >> 4;

	return true;
}


void WavPlayer::closeFile()
{
	if (m_fileIsOpened)
	{
		f_close(&m_fil);
		m_fileIsOpened = false;
	}
}

void WavPlayer::loadAndPlay(const char* fileName)
{
	if (loadFile(fileName))
		play();
}

/////////////////////////
// SoundPlayer
void SoundPlayer::addVariant(const char* filename)
{
	m_variants.push_back(filename);
}

void SoundPlayer::addVariant(std::string& filename)
{
	m_variants.push_back(filename);
}

void SoundPlayer::addVariant(std::string&& filename)
{
	m_variants.push_back(filename);
}

void SoundPlayer::readVariants(const char* filenamePrefix, const char* filenameSuffix)
{
	unsigned int number = 0;
	char buff[10];
	FRESULT res = FR_OK;
	do {
		sprintf(buff, "%u", ++number);
		std::string nextFilename = std::string(filenamePrefix) + buff + filenameSuffix;
		res = f_stat(nextFilename.c_str(), nullptr);
		if (res == FR_OK)
		{
			addVariant(nextFilename);
		}
	} while (res == FR_OK);
	printf("%sN%s - %u found\n", filenamePrefix, filenameSuffix, number-1);
}

void SoundPlayer::play()
{
	if (m_variants.empty())
	{
		printf("No sound set\n");
		return;
	}
	unsigned int rnd = Random::random(m_variants.size()-1);
	printf("Random sound %u\n", rnd);
	WavPlayer::instance().loadAndPlay(m_variants[rnd].c_str());
}

