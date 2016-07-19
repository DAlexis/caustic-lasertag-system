/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/

#include "head-sensor/head-sensor.hpp"
#include "core/logging.hpp"
#include "core/os-wrappers.hpp"
#include "core/device-initializer.hpp"
#include "core/diagnostic.hpp"
#include "dev/wav-player.hpp"
#include "hal/adc.hpp"
#include "hal/rtc.hpp"
#include "core/power-monitor.hpp"

#include <functional>
#include <stdio.h>

DeviceInitializer deviceInitializer;

IADC* adc = nullptr;

TaskOnce sound([](){
	info << "Sound here";

	//WavPlayer::instance().play("piknik.wav", 1);
	//WavPlayer::instance().play("sine.wav", 0);
});

int main(void)
{
	deviceInitializer.initEnvironment();
	// Wait for voltages stabilization
	printf("Hal delay\n");

	//HAL_Delay(100);
	printf("delay done\n");
#ifdef DEBUG
	debug.enable();
	radio.enable();
	//trace.enable();
#endif
	info << "=============== Device initialization ===============";

	sound.setStackSize(128);
	//sound.run(2000);

	IAnyDevice* device = deviceInitializer.initDevice("device.ini");

	SystemMonitor::instance().run();

	//HAL_Delay(10);

	Kernel::instance().run();


	// We should never get here as control is now taken by the scheduler
	while (1)
	{
		printf("That's fail\n");
	}
}

char buf[10];


#ifdef USE_FULL_ASSERT

extern "C" void assert_failed(uint8_t* file, uint32_t line)
{
	//error << "Called assert_failed at " << (char*)file << ":" << (int)line;
}

#endif

