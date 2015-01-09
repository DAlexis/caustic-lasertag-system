/*
 * console-tester.hpp
 *
 *  Created on: 26 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_TESTS_CONSOLE_TESTER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_TESTS_CONSOLE_TESTER_HPP_

#include "hal/fragment-player.hpp"

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
};


#endif /* LAZERTAG_RIFLE_INCLUDE_TESTS_CONSOLE_TESTER_HPP_ */
