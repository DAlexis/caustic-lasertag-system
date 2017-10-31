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

#ifndef INCLUDE_RCSP_RCSP_STATE_SAVER_HPP_
#define INCLUDE_RCSP_RCSP_STATE_SAVER_HPP_

#include "rcsp/aggregator.hpp"
#include "rcsp/state-saver-interface.hpp"
#include "core/os-wrappers.hpp"
#include "utils/macro.hpp"

#include <list>
#include <string>

#define TWO_FILES_STATE_SAVER

/**
 * @brief Class provides saving state of device that is devices parameters.
 *        This class has singleton interface, but can be instantiated directly
 *        if user need it anywhere.
  */
class MainStateSaver : public IAnyStateSaver
{
public:
	MainStateSaver(RCSPAggregator* aggregator);
    void setFilename(const std::string& filename);

    void runSaver(uint32_t period);
    void stopSaver();

    void saveState();

    bool tryRestore();
    void resetSaves();

    void registerStateSaver(IAnyStateSaver* saver);

private:
    constexpr static uint32_t sdioTimeout = 100000;
    void saveAll();
    bool tryRestore(uint8_t variant);
    FILE* m_fil;

#ifdef TWO_FILES_STATE_SAVER
    std::string m_file[2];
    std::string m_fileLock[2];
    std::string m_fileCurrent[2];
    uint8_t m_current = 0, m_next = 1;
#else
    std::string m_filename;
#endif

    std::list<IAnyStateSaver*> m_savers;
    TaskCycled m_savingTask;
    RCSPAggregator* m_aggregator;
};

#endif /* INCLUDE_RCSP_RCSP_STATE_SAVER_HPP_ */
