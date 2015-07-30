/*
 * RCSP-state-saver.cpp
 *
 *  Created on: 08 апр. 2015 г.
 *      Author: alexey
 */

#include "rcsp/RCSP-state-saver.hpp"
#include "rcsp/RCSP-stream.hpp"
#include "core/os-wrappers.hpp"
#include "core/logging.hpp"
//#include "dev/sdcard-fs.hpp"
#include "fatfs.h"

#include <string.h>

SINGLETON_IN_CPP(StateSaver)

StateSaver::StateSaver()
{
	m_savingTask.setTask(std::bind(&StateSaver::saveState, this));
	m_savingTask.setStackSize(1824);
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
	taskENTER_CRITICAL();
	info << "Saving state";
	FRESULT res = FR_OK;
	info << "Fake state saving";
	res = f_open(&m_fil, "test.txt", /*FA_CREATE_NEW | */FA_WRITE);
	if (res)
	{
		error << "Error f_open: " << parseFRESULT(res);
		return;
	}
	UINT bw = 0;
	res = f_write(&m_fil, "Test!11", 7, &bw);
	if (res)
	{
		error << "Error f_write: " << parseFRESULT(res);
		return;
	}
	f_close(&m_fil);
	taskEXIT_CRITICAL();
	// Creating lock file
	/*
	res = f_open(&m_fil, m_fileLock[m_current].c_str(), FA_CREATE_NEW);
	if (res != FR_OK)
	{
		error << "Cannot create lock file: " << parseFRESULT(res);
		return;
	}

	f_close(&m_fil);*/
/*
	// Deleting old state file
	f_unlink(m_file[m_current].c_str());
	// Creating new state file
	res = f_open(&m_fil, m_file[m_current].c_str(), FA_CREATE_NEW | FA_WRITE);
	if (res != FR_OK)
	{
		error << "Cannot create state file: " << parseFRESULT(res);
		return;
	}
	// Putting data to state file
	RCSPMultiStream stream;
	for (OperationCode code : m_codes)
		stream.addValue(code);
	stream.writeToFile(&m_fil);
	f_close(&m_fil);

	// Removing lock file
	f_unlink(m_fileLock[m_current].c_str());
	// Deleting old .current file
	f_unlink(m_fileCurrent[m_next].c_str());
	// Creating new .current file
	res = f_open(&m_fil, m_fileCurrent[m_current].c_str(), FA_CREATE_NEW);
	if (res != FR_OK)
	{
		error << "Cannot create .current file: " << parseFRESULT(res);
		return;
	}
	f_close(&m_fil);

	std::swap(m_current, m_next);*/

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
		res = f_open(&m_fil, m_file[m_current].c_str(), FA_READ);
		if (res != FR_OK)
		{
			error << "Cannot open state file!\n";
			return false;
		}
		uint8_t* buffer = new uint8_t[RCSPStream::defaultLength];
		memset(buffer, 0, RCSPStream::defaultLength);
		UINT readed = 0;

		for(;;) {
			res = f_read(&m_fil, buffer, RCSPStream::defaultLength, &readed);
			if (res == FR_OK && readed != 0)
			{
				//trace << "Dispatching restored parameters chunk...\n";
				RCSPAggregator::instance().dispatchStream(buffer, readed);
			}
			else
				break;
		}

		f_close(&m_fil);
		return true;
	}
	warning << m_file[variant].c_str() << " does not exists\n";
	return false;
}

bool StateSaver::tryRestore()
{
	if (f_stat(m_fileCurrent[0].c_str(), nullptr) == FR_OK)
	{
		return tryRestore(0) ? true : tryRestore(1);
	}
	return tryRestore(1) ? true : tryRestore(0);
}

void StateSaver::resetSaves()
{
	f_unlink(m_fileLock[0].c_str());
	f_unlink(m_file[0].c_str());
	f_unlink(m_fileLock[1].c_str());
	f_unlink(m_file[1].c_str());
}

void StateSaver::runSaver(uint32_t period)
{
	m_savingTask.run(period, period, period, 0);
}

void StateSaver::stopSaver()
{
	/*
	if (m_savingTask)
		Scheduler::instance().stopTask(m_savingTask);
		*/
}
