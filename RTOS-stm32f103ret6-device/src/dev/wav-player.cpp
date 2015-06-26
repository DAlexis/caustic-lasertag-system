/*
 * wav-player.cpp
 *
 *  Created on: 09 янв. 2015 г.
 *      Author: alexey
 */

#include "dev/wav-player.hpp"
#include "dev/random.hpp"
#include "core/logging.hpp"
#include <stdio.h>
#include <string.h>

SINGLETON_IN_CPP(WavPlayer)

WavPlayer::WavPlayer()
{
	ScopedTag tag("wav-player-init");
	debug << "Creating audio buffers";
	m_buffer1 = new SoundSample[audioBufferSize];
	m_buffer2 = new SoundSample[audioBufferSize];
	trace << "Testing audio buffers";
	for (int i=0; i<audioBufferSize; i++)
	{
		m_buffer1[i] = 0xFFFF;
		m_buffer2[i] = 0xFFFF;
	}
	debug << "Done";
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

void WavPlayer::setVerbose(bool verbose)
{
	m_verbose = verbose;
}

bool WavPlayer::loadFile(const char* fileName)
{
	ScopedTag tag("wav-loading");
	fragmentPlayer->stop();
	FRESULT res;
	debug << "Opening file...";
	m_totalReaded = 0;
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
		debug << "File loaded";

	if (!loadFragment(m_buffer1))
	{
		error << "Cannot load first fragment from audio file";
		f_close(&m_fil);
		return false;
	}

	if (!loadFragment(m_buffer2))
	{
		error << "Cannot load second fragment from audio file";
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
		error << "Cannot play, file wasn't opened";
		return;
	}
	fragmentPlayer->stop();
	fragmentPlayer->setFragmentSize(audioBufferSize);
	fragmentPlayer->playFragment(m_buffer1);
	m_isPlaying = true;
}

void WavPlayer::stop()
{
	//SDCardFS::instance().unlock();
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
	ScopedTag tag("wav-header-reading");
	FRESULT res;
	UINT readed = 0;
	res = f_read (&m_fil, &m_header, sizeof(m_header), &readed);
	if (res != FR_OK)
	{
		error << "Cannot read header from file";
		return false;
	}

	if (readed != sizeof(m_header))
	{
		error << "Incomplete wav file";
		return false;
	}

	//if (m_verbose) printInfo();

	// Validating m_header
	if (strncmp(m_header.riff, "RIFF", 4) != 0)
	{
		error << "Invalid format: not RIFF";
		return false;
	}
	if (m_header.audio_format != 1)
	{
		error << "Invalid audio format: " << m_header.audio_format << " Only PCM audio supported";
		return false;
	}
	if (m_header.num_channels != 1)
	{
		error << "Invalid channels count: " << m_header.num_channels << " Only mono sound supported";
		return false;
	}
	if (m_header.bits_per_sample != 16)
	{
		error << "Invalid bits per sample rate: " << m_header.bits_per_sample << "Only 16 supported";
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
	bool needStop = false;

	// For some unknown reason, f_read does not read more about 1k and crashes
	/// @todo increase blockSize to maximum
	uint8_t* curs = (uint8_t*) ptr;
	uint16_t sizeToRead = audioBufferSize*sizeof(int16_t);

	if (sizeToRead > (m_header.chunk_size - m_totalReaded))
	{
		sizeToRead = m_header.chunk_size - m_totalReaded;
		sizeToRead = 0;
	}

	constexpr uint16_t blockSize = 400;
	uint16_t count = sizeToRead / blockSize;
	uint16_t tail = sizeToRead % blockSize;

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
		if (blockSize != readedNow)
			break;
	}

	if (tail != 0 && blockSize == readedNow) {
		res = f_read(&m_fil, curs, tail, &readedNow);
		if (res != FR_OK)
		{
			m_lastBufferSize = 0;
			closeFile();
			return false;
		}
		// If file has metainformation or any other chunk
		if (readedNow > tail)
			readedNow = tail;
		readed += readedNow;
	}

	m_totalReaded += readed;
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
		//SDCardFS::instance().unlock();
	}
}

void WavPlayer::loadAndPlay(const char* fileName)
{
	/*
	// If SD-card is busy
	if (SDCardFS::instance().isLocked() && !m_isPlaying)
		return;
		*/
	/// @todo Add here deferred call

	if (m_isPlaying)
	{
		stop();
	}

	//SDCardFS::instance().lock();
	if (loadFile(fileName))
		play();
	//else
	//	SDCardFS::instance().unlock();
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
	ScopedTag tag("sp-scan-sounds");
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
	debug << filenamePrefix << "X" << filenameSuffix << " - " << number-1 << "found";
}

void SoundPlayer::play()
{
	ScopedTag tag("play-sound");
	if (m_variants.empty())
	{
		warning << "No sound set";
		return;
	}
	unsigned int rnd = Random::random(m_variants.size()-1);
	debug << "Playing variant " << rnd;
	WavPlayer::instance().loadAndPlay(m_variants[rnd].c_str());
}

