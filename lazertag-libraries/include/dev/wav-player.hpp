/*
 * wav-player.hpp
 *
 *  Created on: 09 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_DEV_WAV_PLAYER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DEV_WAV_PLAYER_HPP_

#include "hal/fragment-player.hpp"
#include "hal/ff/ff.h"
#include "core/singleton-macro.hpp"

#define AUDIO_BUFFER_SIZE         1000

class WavPlayer
{
public:
	WavPlayer();
	~WavPlayer();
	static WavPlayer& instance();
	void init();
	bool loadFile(const char* fileName);
	void play();
	void stop();
	void setVerbose(bool verbose = true);

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

	void fragmentDoneCallback(SoundSample* oldBuffer);
	bool readHeader();
	bool loadFragment(SoundSample* m_buffer);

	FIL m_fil;
	uint32_t m_lastBufferSize;
	WavHeader m_header;
	bool m_verbose = true;
	bool m_isPlaying = false;
	SoundSample *m_buffer1 = nullptr, *m_buffer2 = nullptr;

	static WavPlayer* m_wavPlayer;
	STATIC_DEINITIALIZER_IN_CLASS_DECLARATION;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_WAV_PLAYER_HPP_ */