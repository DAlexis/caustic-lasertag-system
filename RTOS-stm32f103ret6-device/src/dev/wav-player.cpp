/*
 * wav-player.cpp
 *
 *  Created on: 09 янв. 2015 г.
 *      Author: alexey
 */

#include "dev/wav-player.hpp"
#include "dev/random.hpp"
#include "core/logging.hpp"
#include "core/string-utils.hpp"
#include "utils/fatfs-utils.hpp"
#include <stdio.h>
#include <string.h>

SINGLETON_IN_CPP(WavPlayer)

WavPlayer::WavPlayer()
{
	ScopedTag tag("wav-player-init");
	debug << "Creating audio buffers";
	m_buffer1 = new SoundSample[audioBufferSize];
	m_buffer2 = new SoundSample[audioBufferSize];
	m_tempBuffer = new SoundSample[audioBufferSize];
	trace << "Testing audio buffers";
	for (unsigned int i=0; i<audioBufferSize; i++)
	{
		m_buffer1[i] = 0xFFFF;
		m_buffer2[i] = 0xFFFF;
		m_tempBuffer[i] = 0xFFFF;
	}
	debug << "Done";
	m_currentBuffer = m_buffer1;
	m_nextBuffer = m_buffer2;

	m_loadingTask.setStackSize(256);
	m_loadingTask.setTask(std::bind(&WavPlayer::loader, this));

}

WavPlayer::~WavPlayer()
{
	delete[] m_buffer1;
	delete[] m_buffer2;
	delete[] m_tempBuffer;
}


void WavPlayer::init()
{
	fragmentPlayer->init();
	fragmentPlayer->setFragmentDoneCallback(std::bind(&WavPlayer::fragmentDoneCallback, this, std::placeholders::_1));
	m_loadingTask.run(0);

	// Prepairing buffers
	silenceToBuffer(m_currentBuffer);
	silenceToBuffer(m_nextBuffer);
	// Starting infinite playback
	fragmentPlayer->setFragmentSize(audioBufferSize);
}

void WavPlayer::loader()
{
	LoadingTask task;

	for(;;)
	{
		m_loadQueue.popFront(task);
		//info << "Queue has task: " << task.task;

		switch (task.task)
		{
		case LoadingTask::clearBuffer:
			silenceToBuffer(task.buffer);
			break;
		case LoadingTask::loadNewFile:
			openFile(m_contexts[task.channel].filename, task.channel);
			break;
		case LoadingTask::loadNextFragment:
			loadFragment(task.buffer, task.channel);
			break;
		default:
			error << "Sound loader: unknown task " << task.task;
		}
	}
}

void WavPlayer::silenceToBuffer(SoundSample* buffer)
{
	for (unsigned int i=0; i<audioBufferSize; i++)
		buffer[i] = INT16_MAX >> 4;
	//buffer[0] = buffer[1] = INT16_MAX >> 4 + 1000;
}

bool WavPlayer::openFile(const char* fileName, uint8_t channel)
{
	// If we already playing on this channel
	if (m_contexts[channel].fileIsOpened)
	{
		f_close(&m_contexts[channel].file);
		m_contexts[channel].fileIsOpened = false;
	}
	FRESULT res;
	debug << "Opening file...";
	res = f_open(&m_contexts[channel].file, m_contexts[channel].filename, FA_OPEN_EXISTING | FA_READ);
	if (res)
	{
		error << "f_open returned error " << parseFRESULT(res);
		return false;
	}

	//debug << "ok";
	m_contexts[channel].fileIsOpened = true;

	if (!m_contexts[channel].readHeader())
	{
		f_close(&m_contexts[channel].file);
		error << "Invalid file header";
		return false;
	}

	//trace << "File header is ok";
	m_contexts[channel].totalReaded = 0;

	// Adding first fragment from file to next buffer
	return loadFragment(m_nextBuffer, channel);
}

Result WavPlayer::play(const char* fileName, uint8_t channel)
{
	// To start playing we should add task to queue
	if (!m_fragmentPlayerReady)
	{
		m_fragmentPlayerReady = true;
		fragmentPlayer->playFragment(m_currentBuffer);
	}
	m_contexts[channel].filename = fileName;
	m_loadQueue.pushBack(LoadingTask(nullptr, channel, LoadingTask::loadNewFile));
	return Result();
}

void WavPlayer::fragmentDoneCallback(SoundSample* oldBuffer)
{
	UNUSED_ARG(oldBuffer);
	std::swap(m_currentBuffer, m_nextBuffer);
	fragmentPlayer->playFragment(m_currentBuffer);

	// Telling to clear buffer
	m_loadQueue.pushBackFromISR(LoadingTask(m_nextBuffer, 0, LoadingTask::clearBuffer));

	// Telling to add sound from all channels to buffers
	for (int i=0; i<channelsCount; i++)
	{
		m_loadQueue.pushBackFromISR(LoadingTask(m_nextBuffer, i, LoadingTask::loadNextFragment));
	}
}

bool WavPlayer::ChannelContext::readHeader()
{
	FRESULT res;
	UINT readed = 0;
	res = f_read (&file, &header, sizeof(header), &readed);
	if (res != FR_OK)
	{
		error << "Cannot read header from file";
		return false;
	}

	if (readed != sizeof(header))
	{
		error << "Incomplete wav file";
		return false;
	}

	//if (m_verbose) printInfo();

	// Validating m_header
	if (strncmp(header.riff, "RIFF", 4) != 0)
	{
		error << "Invalid format: not RIFF";
		return false;
	}
	if (header.audio_format != 1)
	{
		error << "Invalid audio format: " << header.audio_format << " - Only PCM audio supported";
		return false;
	}
	if (header.num_channels != 1)
	{
		error << "Invalid channels count: " << header.num_channels << " - Only mono sound supported";
		return false;
	}
	if (header.bits_per_sample != 16)
	{
		error << "Invalid bits per sample rate: " << header.bits_per_sample << " - Only 16 supported";
		return false;
	}
	return true;
}

bool WavPlayer::loadFragment(SoundSample* buffer, uint8_t channel)
{
	if (!m_contexts[channel].fileIsOpened)
	{
		//error << "Attempt to load fragment from closed file";
		return false;
	}

	FRESULT res;
	UINT readed = 0;
	int16_t *tmpPrt = reinterpret_cast<int16_t *>(m_tempBuffer);

	// For some unknown reason, f_read does not read more about 1k and crashes
	/// @todo increase blockSize to maximum

	uint16_t sizeToRead = audioBufferSize*sizeof(int16_t);

	if (sizeToRead > (m_contexts[channel].header.chunk_size - m_contexts[channel].totalReaded))
	{
		sizeToRead = m_contexts[channel].header.chunk_size - m_contexts[channel].totalReaded;
		// For testing:
		/// @todo Remove this next line!11
		sizeToRead = 0;
	}

	res = f_read_huge(&m_contexts[channel].file, tmpPrt, sizeToRead, &readed);
	if (res != FR_OK)
	{
		error << "Cannot read fragment from file: " << parseFRESULT(res);
		f_close(&m_contexts[channel].file);
		return false;
	}

	if (readed < sizeToRead)
	{
		// We readed till file's end, so we can close it
		trace << "file's end";
		f_close(&m_contexts[channel].file);
		m_contexts[channel].fileIsOpened = false;
	}
	readed /= sizeof(int16_t);
	for (unsigned int i=readed; i<audioBufferSize; i++)
	{
		tmpPrt[i] = 0;
	}


	// Now we have array of int16 samples so we need to add it to buffer
	for (unsigned int i=0; i<audioBufferSize; i++)
	{
		buffer[i] += tmpPrt[i] >> 4;
	}

	return true;
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
	debug << filenamePrefix << "X" << filenameSuffix << " - " << number-1 << " found";
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
	WavPlayer::instance().play(m_variants[rnd].c_str(), 0);
}

