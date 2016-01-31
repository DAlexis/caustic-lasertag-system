/*
 * RCSP-state-saver.hpp
 *
 *  Created on: 08 апр. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_RCSP_RCSP_STATE_SAVER_HPP_
#define INCLUDE_RCSP_RCSP_STATE_SAVER_HPP_

#include "rcsp/RCSP-aggregator.hpp"
#include "rcsp/state-saver-interface.hpp"
#include "core/os-wrappers.hpp"
#include "utils/macro.hpp"
#include "fatfs.h"

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
	MainStateSaver();
    void addValue(OperationCode code);
    void setFilename(const std::string& filename);

    void runSaver(uint32_t period);
    void stopSaver();

    void saveState();

    bool tryRestore();
    void resetSaves();

    void registerStateSaver(IAnyStateSaver* saver);

    SIGLETON_IN_CLASS(MainStateSaver)

private:
    void saveAll();
    bool tryRestore(uint8_t variant);
    FIL m_fil;

#ifdef TWO_FILES_STATE_SAVER
    std::string m_file[2];
    std::string m_fileLock[2];
    std::string m_fileCurrent[2];
    uint8_t m_current = 0, m_next = 1;
#else
    std::string m_filename;
#endif

    std::list<OperationCode> m_codes;
    std::list<IAnyStateSaver*> m_savers;
    TaskCycled m_savingTask;
};

#endif /* INCLUDE_RCSP_RCSP_STATE_SAVER_HPP_ */
