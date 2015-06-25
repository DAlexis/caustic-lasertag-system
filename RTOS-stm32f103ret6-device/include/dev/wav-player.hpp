/*
 * wav-player.hpp
 *
 *  Created on: 09 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_DEV_WAV_PLAYER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DEV_WAV_PLAYER_HPP_

#include "hal/fragment-player.hpp"
/*
#include "hal/ff/ff.h"
#include "core/singleton-macro.hpp"
#include <vector>
#include <string>

class WavPlayer
{
public:
	constexpr static uint32_t audioBufferSize = 1000;
	WavPlayer();
	~WavPlayer();
	static WavPlayer& instance();
	void init();

	void loadAndPlay(const char* fileName);
	void stop();
	inline bool isPlaying() { return m_isPlaying; }
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

	bool loadFile(const char* fileName);
	void play();
	void fragmentDoneCallback(SoundSample* oldBuffer);
	bool readHeader();
	bool loadFragment(SoundSample* m_buffer);
	void closeFile();

	FIL m_fil;
	uint32_t m_lastBufferSize;
	WavHeader m_header;
	uint32_t m_totalReaded;
	bool m_verbose = true;
	bool m_isPlaying = false;
	bool m_fileIsOpened = false;
	SoundSample *m_buffer1 = nullptr, *m_buffer2 = nullptr;

	static WavPlayer* m_wavPlayer;
	STATIC_DEINITIALIZER_IN_CLASS_DECLARATION;
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

*/
#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_WAV_PLAYER_HPP_ */
