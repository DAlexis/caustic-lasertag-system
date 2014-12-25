/*
 * console-tester.hpp
 *
 *  Created on: 26 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_TESTS_CONSOLE_TESTER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_TESTS_CONSOLE_TESTER_HPP_

class ConsoleTester
{
public:
	ConsoleTester();

private:
	void firePulseTest(const char*);
	void firePulseTestCallback(bool state);
};


#endif /* LAZERTAG_RIFLE_INCLUDE_TESTS_CONSOLE_TESTER_HPP_ */
