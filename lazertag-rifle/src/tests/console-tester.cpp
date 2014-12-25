/*
 * console-tester.cpp
 *
 *  Created on: 26 дек. 2014 г.
 *      Author: alexey
 */

#include "tests/console-tester.hpp"
#include "hal/fire-emitter.hpp"
#include "dev/console.hpp"
#include <stdio.h>

ConsoleTester::ConsoleTester()
{
	Console::instance().registerCommand(
		"tpulse",
		"infinite test fire pulse modulated by 1000us packs",
		std::bind(&ConsoleTester::firePulseTest, this, std::placeholders::_1)
	);
}

void ConsoleTester::firePulseTest(const char*)
{
	printf("Test pulse start\n");
	fireEmittersPool->getFireEmitter(0)->init();
	fireEmittersPool->getFireEmitter(0)->setPower(24);
	fireEmittersPool->getFireEmitter(0)->setCallback(std::bind(&ConsoleTester::firePulseTestCallback, this, std::placeholders::_1));
	fireEmittersPool->getFireEmitter(0)->startImpulsePack(true, 600);
}

void ConsoleTester::firePulseTestCallback(bool state)
{
	fireEmittersPool->getFireEmitter(0)->startImpulsePack(!state, 600);
}
