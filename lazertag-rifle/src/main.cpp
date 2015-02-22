//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"

#include "logic/rifle.hpp"
#include "logic/head-sensor.hpp"
#include "hal/usart.hpp"
#include "hw/sdcard.h"
#include "dev/console.hpp"
#include "dev/alive-indicator.hpp"
#include "dev/buttons.hpp"
#include "dev/wav-player.hpp"
#include "hal/system-clock.hpp"
#include "hal/leds.hpp"
#include "core/scheduler.hpp"
#include "core/memory-utils.h"

#include "tests/console-tester.hpp"
#include <functional>
#include <vector>

//#undef DEVICE_RIFLE

//#ifndef DEVICE_RIFLE
//#define DEVICE_HEAD_SENSOR
//#endif

// ----------------------------------------------------------------------------
//
// The external clock frequency is specified as a preprocessor definition
// passed to the compiler via a command line option (see the 'C/C++ General' ->
// 'Paths and Symbols' -> the 'Symbols' tab, if you want to change it).
// The value selected during project creation was HSE_VALUE=8000000.
//

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

char buff[100];

void test(const char* b, int size)
{
	printf("Registered\n");
}

void fireCallbackTest(bool state)
{
	printf("keypressedm state=%d\n", state ? 1 : 0);
}

Rifle *rifle = nullptr;
HeadSensor *headSensor = nullptr;

void checkMemory()
{
	if (isMemoryCorrupted) {
		printf("Memory corrupted!!\n");
	}
}


int main(int argc, char* argv[])
{
	// Send a greeting to the trace device (skipped on Release).
	//trace_puts("Hello ARM World!");

	// The standard output and the standard error should be forwarded to
	// the trace device. For this to work, a redirection in _write.c is
	// required.
	//puts("Standard output message.");
	//fprintf(stderr, "Standard error message.\n");

	// At this stage the system clock should have already been configured
	// at high speed.
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	trace_printf("\n\nStarting system...\n", SystemCoreClock);
	trace_printf("System clock: %uHz\n", SystemCoreClock);

	// Perform all necessary initialisations for the LED.

	Console::instance().init(0);

	ConsoleTester tester;

	systemClock->wait_us(100000);

#if defined(DEVICE_RIFLE)
	rifle = new Rifle;
	rifle->configure();
#elif defined(DEVICE_HEAD_SENSOR)
	headSensor = new HeadSensor;
	headSensor->configure();
#else
	#error "Device type not selected!"
#endif

	Scheduler::instance().addTask(std::bind(&Console::interrogate, &Console::instance()), false, 500000);
	Scheduler::instance().addTask(checkMemory, false, 500000);
	Console::instance().prompt();
	WavPlayer::instance();

	Scheduler::instance().mainLoop();

	// Why? - Why not?
#if defined(DEVICE_RIFLE)
	delete rifle;
#elif defined(DEVICE_HEAD_SENSOR)
	delete headSensor;
#endif
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
