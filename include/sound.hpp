/*
 * sound.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_SOUND_HPP_
#define LAZERTAG_RIFLE_INCLUDE_SOUND_HPP_

#define SOUND_BUFFER_SIZE   128

#include <stdint.h>
#include "ff.h"

class SoundPlayer
{
public:
    static SoundPlayer& getInstance();
    void init();
    void playWav(const char* filename);
    void stop();
    void setVerbose(bool flag);

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

    bool readHeader();
    void printInfo();

    WavHeader m_header;
    const char* m_filename;

    FATFS FatFs;
    FIL fil;

    bool m_verbose;
    static SoundPlayer* m_self;
};



#endif /* LAZERTAG_RIFLE_INCLUDE_SOUND_HPP_ */
