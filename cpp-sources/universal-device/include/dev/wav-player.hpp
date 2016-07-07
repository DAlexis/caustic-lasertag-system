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
	constexpr static uint32_t audioBufferSize = 1000;
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

	bool openFile(uint8_t channel);
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

	Worker m_loader{10};
};

class SoundPlayer
{
public:
	void readVariants(const char* filenamePrexix, const char* filenameSuffix, uint8_t channel = 0);
	void addVariant(const char* filename);
	void addVariant(std::string& filename);
	void addVariant(std::string&& filename);
	void play();

private:
	std::vector<std::string> m_variants;
	uint8_t m_channel;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_WAV_PLAYER_HPP_ */
