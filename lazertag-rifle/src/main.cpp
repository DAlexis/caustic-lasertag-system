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
#include "hal/usart.hpp"
#include "hw/usart.hpp"
#include "dev/console.hpp"
#include "dev/alive-indicator.hpp"
#include "dev/buttons.hpp"
#include "hal/system-clock.hpp"
#include "hal/leds.hpp"
#include "core/scheduler.hpp"

#include "tests/console-tester.hpp"
#include <functional>
#include <vector>

// ----------------------------------------------------------------------------
//
// STM32F1 led blink sample (trace via ITM).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// To demonstrate POSIX retargetting, reroute the STDOUT and STDERR to the
// trace device and display messages on both of them.
//
// Then demonstrates how to blink a led with 1Hz, using a
// continuous loop and SysTick delays.
//
// On DEBUG, the uptime in seconds is also displayed on the trace device.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the ITM output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//
// The external clock frequency is specified as a preprocessor definition
// passed to the compiler via a command line option (see the 'C/C++ General' ->
// 'Paths and Symbols' -> the 'Symbols' tab, if you want to change it).
// The value selected during project creation was HSE_VALUE=8000000.
//
// Note: The default clock settings take the user defined HSE_VALUE and try
// to reach the maximum possible system clock. For the default 8MHz input
// the result is guaranteed, but for other values it might not be possible,
// so please adjust the PLL settings in system/src/cmsis/system_stm32f10x.c
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

Timer *timer = nullptr;

// ----- main() ---------------------------------------------------------------



// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

void testfunc(int q)
{
	timer->sleep(BLINK_ON_TICKS);
}

char buff[100];

void test(const char* b, int size)
{
	printf("Registered\n");
}

void fireCallbackTest(bool state)
{
	printf("keypressedm state=%d\n", state ? 1 : 0);
}

int main(int argc, char* argv[])
{
	// By customising __initialize_args() it is possible to pass arguments,
	// for example when running tests with semihosting you can pass various
	// options to the test.
	// trace_dump_args(argc, argv);

	// Send a greeting to the trace device (skipped on Release).
	trace_puts("Hello ARM World!");

	// The standard output and the standard error should be forwarded to
	// the trace device. For this to work, a redirection in _write.c is
	// required.
	puts("Standard output message.");
	fprintf(stderr, "Standard error message.\n");

	// At this stage the system clock should have already been configured
	// at high speed.
	trace_printf("System clock: %uHz\n", SystemCoreClock);

	timer = new Timer;
	timer->start();

	BlinkLed blinkLed;

	// Perform all necessary initialisations for the LED.
	blinkLed.powerUp();

	Console::instance().init(0);

	ConsoleTester tester;

	Console::instance().prompt();

	Rifle rifle;
	rifle.configure();
	rifle.run();
	// Infinite loop
	/*
	Scheduler sheduler;
	ILedManager *alive = ledsPool->getLed(1);
	AliveIndicator aliveIndicator;
	aliveIndicator.init(alive);

	printf("new task: %u\n", sheduler.addTask(std::bind(&AliveIndicator::blink, aliveIndicator), false, 500000));

	ButtonManager* bmgr = ButtonsPool::instance().getButtonManager(0,0);
	ButtonsPool::instance().setExti(0, 0, true);

	bmgr->setAutoRepeat(true);
	bmgr->setCallback(fireCallbackTest);

	printf("new task: %u\n", sheduler.addTask(std::bind(&ButtonManager::interrogate, bmgr), false, 5000));



	sheduler.mainLoop();*/
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
