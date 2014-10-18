/*
 * sound.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_SOUND_HPP_
#define LAZERTAG_RIFLE_INCLUDE_SOUND_HPP_

#define SOUND_BUFFER_SIZE   512

#include <stdint.h>
#include "ff.h"

class SoundPlayer
{
public:
    SoundPlayer();
    void init();
    void playWav(const char* filename);
    void stop();
    void setVerbose(bool flag);

    void fragmentTimerIRQ();
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

    struct AudioBuffer {
        uint16_t buffer[SOUND_BUFFER_SIZE];
        uint16_t size;
    };

    void initPWMTimer();
    void initSamplerTimer();
    void initFragmentTimer();
    void initDMA(AudioBuffer* source);

    bool readHeader();
    void printInfo();
    void printParameters();
    bool loadFragment(AudioBuffer* buffer);
    void swapBuffers();
    void normalizeBuffer(AudioBuffer* buffer);

    void playCurrentBuffer();

    void fragmentTimerEnable();
    void PWMTimerEnable();
    void fragmentTimerDisable();
    void PWMTimerDisable();

    void stopTimers();

    WavHeader m_header;
    const char* m_filename;

    FATFS FatFs;
    FIL m_fil;

    bool m_verbose;
    bool m_needStop;
    AudioBuffer m_buffer1, m_buffer2;
    int16_t m_tmpBuffer[SOUND_BUFFER_SIZE];
    AudioBuffer *m_pCurrentBuffer, *m_pNextBuffer;
};

extern SoundPlayer sound;

#endif /* LAZERTAG_RIFLE_INCLUDE_SOUND_HPP_ */
