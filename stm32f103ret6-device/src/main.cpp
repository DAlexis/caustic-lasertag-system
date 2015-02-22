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

	// Setting priorities group where 4 bit for priority and 0 bit for subpriority
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	trace_printf("\n\nStarting system...\n");
	trace_printf("System clock: %uHz\n", SystemCoreClock);

	Console::instance().init(0);

#ifdef DEBUG
	// Creating console tester
	ConsoleTester tester;
#endif

	// Waiting for some modules initialization
	systemClock->wait_us(100000);

#if defined(DEVICE_RIFLE)
	Rifle *rifle = nullptr;
	rifle = new Rifle;
	rifle->configure();
#elif defined(DEVICE_HEAD_SENSOR)
	HeadSensor *headSensor = nullptr;
	headSensor = new HeadSensor;
	headSensor->configure();
#else
	#error "Device type not selected!"
#endif

	// Turning on console
	Scheduler::instance().addTask(std::bind(&Console::interrogate, &Console::instance()), false, 500000);

	// Turning on memory checker
	Scheduler::instance().addTask(checkMemory, false, 500000);

	// Printing console prompt
	Console::instance().prompt();

	// Running main loop of co-op scheduler
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
