/*
 * console-tester.hpp
 *
 *  Created on: 26 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_TESTS_CONSOLE_TESTER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_TESTS_CONSOLE_TESTER_HPP_

#include "hal/fragment-player.hpp"
#include "dev/nrf24l01.hpp"

class ConsoleTester
{
public:
	ConsoleTester();

private:
	void firePulseTest(const char*);
	void firePulseTestCallback(bool state);
	void SDReadingTest(const char*);
	void readSDMBRTest(const char*);
	void fragmentPlayerTest(const char*);

	void loadNextFragment(SoundSample* old);

	void playSoundFile(const char* filename);
	void stopPlaying(const char*);

	void radioInit(const char*);
	void interrogateRadio(const char*);
	void radioRXCallback(uint8_t channel, uint8_t* data);

	NRF24L01Manager *nrf = nullptr;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_TESTS_CONSOLE_TESTER_HPP_ */
