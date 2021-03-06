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


#include "rcsp/state-saver.hpp"
#include "rcsp/stream.hpp"
#include "core/os-wrappers.hpp"
#include "core/logging.hpp"
#include "core/device-initializer.hpp"

#include <string.h>
#include <sys/stat.h>

MainStateSaver::MainStateSaver(RCSPAggregator* aggregator) :
    m_aggregator(aggregator)
{
	m_savers.push_back(this);
	m_savingTask.setTask(std::bind(&MainStateSaver::saveAll, this));
	m_savingTask.setStackSize(512);
	m_savingTask.setName("StatSav");
}

void MainStateSaver::setFilename(const std::string& filename)
{
#ifdef TWO_FILES_STATE_SAVER
	m_file[0] = filename + "_1.bin";
	m_file[1] = filename + "_2.bin";
	m_fileLock[0] = filename + "_1.lock";
	m_fileLock[1] = filename + "_2.lock";
	m_fileCurrent[0] = filename + "_1.current";
	m_fileCurrent[1] = filename + "_2.current";
#else
	m_filename = filename;
#endif
}

void MainStateSaver::saveState()
{
	info << "Saving state";
	if (DeviceInitializer::instance().timeSinceLastSDIO() < sdioTimeout)
	{
		info << "State saving aborted: sdio timeout is not done";
		return;
	}
#ifdef TWO_FILES_STATE_SAVER
	trace << "Variant = " << m_current;
	// Creating lock file
    m_fil = fopen(m_fileLock[m_current].c_str(), "w");
    if (m_fil == nullptr)
	{
        error << "Cannot create lock file: " << m_fileLock[m_current];
		return;
	}

    fclose(m_fil);

	// Deleting old state file
    remove(m_file[m_current].c_str());
	// Creating new state file
    m_fil = fopen(m_file[m_current].c_str(), "w");
    if (m_fil == nullptr)
	{
        error << "Cannot create state file: " << m_file[m_current];
		return;
	}
	// Putting data to state file
    RCSPStream stream(m_aggregator);
	for (OperationCode code : m_aggregator->getRestorableOperationCodes())
		stream.addPush(code);
    stream.writeToFile(m_fil);
    fclose(m_fil);

	// Removing lock file
    remove(m_fileLock[m_current].c_str());
	// Deleting old .current file
    remove(m_fileCurrent[m_next].c_str());
	// Creating new .current file
    m_fil = fopen(m_fileCurrent[m_current].c_str(), "w");
    if (m_fil == nullptr)
	{
        error << "Cannot create .current file: " << m_fileCurrent[m_current];
		return;
	}
    fclose(m_fil);

	std::swap(m_current, m_next);
#else
	FRESULT res = FR_OK;
	// Creating lock file
	res = f_open(&m_fil, m_filename.c_str(), FA_CREATE_ALWAYS | FA_WRITE);
	if (res != FR_OK)
	{
		error << "Cannot create lock file: " << parseFRESULT(res);
		return;
	}
	RCSPMultiStream stream;
	for (OperationCode code : m_codes)
		stream.addValue(code);
	stream.writeToFile(&m_fil);
	f_close(&m_fil);
#endif
}

bool MainStateSaver::tryRestore(uint8_t variant)
{
#ifdef TWO_FILES_STATE_SAVER
	// Check if we have not deleted lock file
    if (stat(m_fileLock[variant].c_str(), nullptr) == 0)
	{
		warning << "Lock file detected, clearing\n";
        remove(m_fileLock[m_current].c_str());
        remove(m_file[m_current].c_str());
		return false;
	}
	// Opening file
    if (stat(m_file[variant].c_str(), nullptr) != 0)
	{
		warning << m_file[variant].c_str() << " does not exists\n";
		return false;
	}


    m_fil = fopen(m_file[variant].c_str(), "r");
    if (m_fil == nullptr)
	{
		error << "Cannot open state file!\n";
		return false;
	}

    Buffer buf;
    const int chunkSize = 100;

    int readed = 0;
    int totalReaded = 0;

	do {
		// resize for next chunk
		buf.resize(buf.size() + chunkSize, 0);
        readed = fread(buf.data()+totalReaded, 1, chunkSize, m_fil);
        if (ferror(m_fil) != 0)
		{
            error << "Error while reading state file: " << m_file[variant];
            fclose(m_fil);
			return false;
		}
        // Reading is good
        totalReaded += readed;
	} while (readed == chunkSize);

    fclose(m_fil);
    m_aggregator->dispatchStream(buf.data(), totalReaded);
	return true;

#else
	return false;
#endif
}

bool MainStateSaver::tryRestore()
{
#ifdef TWO_FILES_STATE_SAVER
    if (stat(m_fileCurrent[0].c_str(), nullptr) == 0)
	{
		// If .current file points to 0 we first try to restore 0, than 1
		return tryRestore(0) ? true : tryRestore(1);
	}
	// If .current file points to 1 we first try to restore 1, than 0
	return tryRestore(1) ? true : tryRestore(0);
#else
	FRESULT res = FR_OK;
	res = f_open(&m_fil, m_filename.c_str(), FA_READ);
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
#endif
}

void MainStateSaver::resetSaves()
{
#ifdef TWO_FILES_STATE_SAVER
    remove(m_fileLock[0].c_str());
    remove(m_file[0].c_str());
    remove(m_fileLock[1].c_str());
    remove(m_file[1].c_str());
#else
    remove(m_filename);
#endif
}

void MainStateSaver::runSaver(uint32_t period)
{
	info << "State saver run with period " << period;
	m_savingTask.run(period, period, period, 0);
}

void MainStateSaver::saveAll()
{
	for (auto it = m_savers.begin(); it != m_savers.end(); it++)
		(*it)->saveState();
}

void MainStateSaver::registerStateSaver(IAnyStateSaver* saver)
{
	m_savers.push_back(saver);
}

void MainStateSaver::stopSaver()
{
	/*
	if (m_savingTask)
		Scheduler::instance().stopTask(m_savingTask);
		*/
}
