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
#include "fatfs.h"
#include "utils/macro.hpp"
#include <vector>
#include <string>

class WavPlayer
{
public:
	constexpr static uint32_t audioBufferSize = 2000;
	WavPlayer();
	~WavPlayer();
	void init();

	void loadAndPlay(const char* fileName);
	void stop();
	inline bool isPlaying() { return m_isPlaying; }
	void setVerbose(bool verbose = true);

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
		FIL file;
		Mutex fileMutex;
		WavHeader header;
	};

	bool loadFile(const char* fileName);
	void play();
	void fragmentDoneCallback(SoundSample* oldBuffer);
	bool readHeader();
	bool loadFragment(SoundSample* m_buffer);
	void closeFile();

	void loaderTask();

	FIL m_fil;
	Mutex m_fileMutex;
	uint32_t m_lastBufferSize;
	WavHeader m_header;
	uint32_t m_totalReaded;
	bool m_verbose = true;
	bool m_isPlaying = false;
	bool m_fileIsOpened = false;
	SoundSample *m_buffer1 = nullptr, *m_buffer2 = nullptr;

	TaskDeferredFromISR m_deferredLoadFragment;

	struct LoadingTask
	{
		LoadingTask(SoundSample* _buffer = nullptr, uint8_t _channel = 0) :
			buffer(_buffer), channel(_channel)
		{}
		SoundSample* buffer;
		uint8_t channel = 0;
	};

	Queue<LoadingTask> m_loadQueue{4};
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
