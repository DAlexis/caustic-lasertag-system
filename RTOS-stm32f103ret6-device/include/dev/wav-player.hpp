/*
 * wav-player.hpp
 *
 *  Created on: 09 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_DEV_WAV_PLAYER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DEV_WAV_PLAYER_HPP_

#include "hal/fragment-player.hpp"
#include "core/os-wrappers.hpp"
#include "core/result-code.hpp"
#include "fatfs.h"
#include "utils/macro.hpp"
#include <vector>
#include <string>

class WavPlayer
{
public:
	constexpr static uint32_t audioBufferSize = 500;
	constexpr static uint8_t channelsCount = 2;
	WavPlayer();
	~WavPlayer();
	void init();

	Result play(const char* fileName, uint8_t channel);

	SIGLETON_IN_CLASS(WavPlayer)
private:
	struct WavHeader {
		char riff[4];
		uint32_t chunk_size;
		char format[4];
		char subchunk1_id[4];
		uint32_t subchunk1_size;
		uint16_t audio_format;
		uint16_t num_channels;
		uint32_t sample_rate;
		uint32_t byte_rate;
		uint16_t block_align;
		uint16_t bits_per_sample;

		char subchunk2_id[4];
		uint32_t subchunk2_size;
	};

	struct ChannelContext
	{
		bool isPlaying = false;
		bool fileIsOpened = false;
		const char* filename = nullptr;
		FIL file;
		Mutex fileMutex;
		WavHeader header;
		uint32_t totalReaded = 0;

		bool readHeader();
	};

	bool openFile(const char* fileName, uint8_t channel);
	void fragmentDoneCallback(SoundSample* oldBuffer);

	bool loadFragment(SoundSample* buffer, uint8_t channel);

	/** This function is a queue-listener. It perform sound fragments loading,
	 *  buffer cleaning and any other job related to audio buffer prepairing
	 */
	void loader();
	void silenceToBuffer(SoundSample* buffer);

	SoundSample *m_buffer1 = nullptr, *m_buffer2 = nullptr;
	SoundSample *m_currentBuffer, *m_nextBuffer;
	SoundSample *m_tempBuffer = nullptr;
	ChannelContext m_contexts[channelsCount];

	bool m_fragmentPlayerReady = false;

	struct LoadingTask
	{
		constexpr static uint8_t loadNextFragment = 0;
		constexpr static uint8_t clearBuffer      = 1;
		constexpr static uint8_t loadNewFile      = 2;

		LoadingTask(SoundSample* _buffer = nullptr, uint8_t _channel = 0, uint8_t _task = loadNextFragment) :
			buffer(_buffer), channel(_channel), task(_task)
		{}
		SoundSample* buffer;
		uint8_t channel = 0;
		uint8_t task = 0;
	};

	Queue<LoadingTask> m_loadQueue{7};
	TaskOnce m_loadingTask;
};

class SoundPlayer
{
public:
	void readVariants(const char* filenamePrexix, const char* filenameSuffix);
	void addVariant(const char* filename);
	void addVariant(std::string& filename);
	void addVariant(std::string&& filename);
	void play();

private:
	std::vector<std::string> m_variants;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_WAV_PLAYER_HPP_ */
