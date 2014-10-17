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
    static void testSDCard(const char*);
    static void testFreeMem(const char*);
    static void testSoundWav(const char* filename);
    static void registerCommands();
};

extern Tester tester;

#endif /* LAZERTAG_RIFLE_INCLUDE_TESTER_H_ */
