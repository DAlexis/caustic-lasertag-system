/*
 * RCSP-state-saver.cpp
 *
 *  Created on: 08 апр. 2015 г.
 *      Author: alexey
 */

#include "rcsp/RCSP-state-saver.hpp"
#include "rcsp/RCSP-stream.hpp"
#include "core/scheduler.hpp"
#include "core/logging.hpp"
#include "dev/sdcard-fs.hpp"
#include "hal/ff/ff.h"

#include <string.h>

StateSaver* StateSaver::m_saver = nullptr;
STATIC_DEINITIALIZER_IN_CPP_FILE(StateSaver, m_saver)

StateSaver& StateSaver::instance()
{
	if (!m_saver)
		m_saver = new StateSaver;
	return *m_saver;
}


void StateSaver::addValue(OperationCode code)
{
	m_codes.push_back(code);
}

void StateSaver::setFilename(const std::string& filename)
{
	m_file[0] = filename + "_1.bin";
	m_file[1] = filename + "_2.bin";
	m_fileLock[0] = filename + "_1.lock";
	m_fileLock[1] = filename + "_1.lock";
	m_fileCurrent[0] = filename + "_1.current";
	m_fileCurrent[1] = filename + "_1.current";

}

void StateSaver::saveState()
{
	info << "Saving state\n";
	if (SDCardFS::instance().isLocked())
	{
		printf("State saver: sd-card is locked\n");
		return;
	}
	SDCardFS::ScopedLocker sdLocker;
	FRESULT res = FR_OK;
	FIL file;
	// Creating lock file
	res = f_open(&file, m_fileLock[m_current].c_str(), FA_CREATE_NEW);
	if (res != FR_OK)
	{
		printf("Cannot create lock file: %d!\n", (int)res);
		return;
	}

	f_close(&file);

	// Deleting old state file
	f_unlink(m_file[m_current].c_str());
	// Creating new state file
	res = f_open(&file, m_file[m_current].c_str(), FA_CREATE_NEW | FA_WRITE);
	if (res != FR_OK)
	{
		printf("Cannot create state file: %d!\n", (int)res);
		return;
	}
	// Putting data to state file
	RCSPMultiStream stream;
	for (OperationCode code : m_codes)
		stream.addValue(code);
	stream.writeToFile(&file);
	f_close(&file);

	// Removing lock file
	f_unlink(m_fileLock[m_current].c_str());
	// Deleting old .current file
	f_unlink(m_fileCurrent[m_next].c_str());
	// Creating new .current file
	res = f_open(&file, m_fileCurrent[m_current].c_str(), FA_CREATE_NEW);
	if (res != FR_OK)
	{
		printf("Cannot create .current file: %d!\n", (int)res);
		return;
	}
	f_close(&file);

	std::swap(m_current, m_next);
}

bool StateSaver::tryRestore(uint8_t variant)
{
	// Check if we have not deleted lock file
	if (f_stat(m_fileLock[variant].c_str(), nullptr) == FR_OK)
	{
		warning << "Lock file detected, clearing\n";
		f_unlink(m_fileLock[m_current].c_str());
		f_unlink(m_file[m_current].c_str());
		return false;
	}
	// Opening file
	if (f_stat(m_file[variant].c_str(), nullptr) == FR_OK)
	{
		FRESULT res = FR_OK;
		FIL file;
		res = f_open(&file, m_file[m_current].c_str(), FA_READ);
		if (res != FR_OK)
		{
			error << "Cannot open state file!\n";
			return false;
		}
		uint8_t* buffer = new uint8_t[RCSPStream::defaultLength];
		memset(buffer, 0, RCSPStream::defaultLength);
		UINT readed = 0;

		for(;;) {
			res = f_read(&file, buffer, RCSPStream::defaultLength, &readed);
			if (res == FR_OK && readed != 0)
			{
				//trace << "Dispatching restored parameters chunk...\n";
				RCSPAggregator::instance().dispatchStream(buffer, readed);
			}
			else
				break;
		}

		f_close(&file);
		return true;
	}
	warning << m_file[variant].c_str() << " does not exists\n";
	return false;
}

bool StateSaver::tryRestore()
{
	if (SDCardFS::instance().isLocked())
	{
		printf("State saver: sd-card is locked\n");
		return false;
	}
	SDCardFS::ScopedLocker sdLocker;
	if (f_stat(m_fileCurrent[0].c_str(), nullptr) == FR_OK)
	{
		return tryRestore(0) ? true : tryRestore(1);
	}
	return tryRestore(1) ? true : tryRestore(0);
}

void StateSaver::resetSaves()
{
	if (SDCardFS::instance().isLocked())
	{
		printf("State saver: sd-card is locked\n");
		Scheduler::instance().addTask(std::bind(&StateSaver::resetSaves, this), true, 0, 0, 100000);
		return;
	}
	SDCardFS::ScopedLocker sdLocker;
	f_unlink(m_fileLock[0].c_str());
	f_unlink(m_file[0].c_str());
	f_unlink(m_fileLock[1].c_str());
	f_unlink(m_file[1].c_str());
}

void StateSaver::runSaver(uint32_t period)
{
	stopSaver();
	m_savingTask = Scheduler::instance().addTask(std::bind(&StateSaver::saveState, this), false, period, 0, period);
}

void StateSaver::stopSaver()
{
	if (m_savingTask)
		Scheduler::instance().stopTask(m_savingTask);
}
