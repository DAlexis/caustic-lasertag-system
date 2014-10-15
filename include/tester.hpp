/*
 * tester.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_TESTER_H_
#define LAZERTAG_RIFLE_INCLUDE_TESTER_H_


class Tester
{
public:
    void testSDCard();
    void testSoundWav(const char* filename);
};

void registerTests();

#endif /* LAZERTAG_RIFLE_INCLUDE_TESTER_H_ */
