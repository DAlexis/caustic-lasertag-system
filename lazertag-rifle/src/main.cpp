//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"

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

#define DEVICE_RIFLE

#ifndef DEVICE_RIFLE
#define DEVICE_HEAD_SENSOR
#endif

// ----------------------------------------------------------------------------
//
// The external clock frequency is specified as a preprocessor definition
// passed to the compiler via a command line option (see the 'C/C++ General' ->
// 'Paths and Symbols' -> the 'Symbols' tab, if you want to change it).
// The value selected during project creation was HSE_VALUE=8000000.
//

// Definitions visible only within this translation unit.
namespace
{
  // ----- Timing definitions -------------------------------------------------

  // Keep the LED on for 2/3 of a second.
  constexpr Timer::ticks_t BLINK_ON_TICKS = Timer::FREQUENCY_HZ * 2 / 3;
  constexpr Timer::ticks_t BLINK_OFF_TICKS = Timer::FREQUENCY_HZ
      - BLINK_ON_TICKS;
}

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

	BlinkLed blinkLed;

	// Perform all necessary initialisations for the LED.
	blinkLed.powerUp();

	Console::instance().init(0);

	ConsoleTester tester;

	systemClock->wait_us(100000);

#ifdef DEVICE_RIFLE
	rifle = new Rifle;
	rifle->configure();
#endif

#ifdef DEVICE_HEAD_SENSOR
	headSensor = new HeadSensor;
	headSensor->configure();
#endif

	Scheduler::instance().addTask(std::bind(&Console::interrogate, &Console::instance()), false, 500000);
	Scheduler::instance().addTask(checkMemory, false, 500000);
	Console::instance().prompt();
	WavPlayer::instance();

	Scheduler::instance().mainLoop();

	// Why? - Why not?
#ifdef DEVICE_RIFLE
	delete rifle;
#endif
#ifdef DEVICE_HEAD_SENSOR
	delete headSensor;
#endif
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
