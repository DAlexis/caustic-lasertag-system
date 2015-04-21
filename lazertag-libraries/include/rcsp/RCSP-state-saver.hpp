/*
 * RCSP-state-saver.hpp
 *
 *  Created on: 08 апр. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_RCSP_RCSP_STATE_SAVER_HPP_
#define INCLUDE_RCSP_RCSP_STATE_SAVER_HPP_

#include "rcsp/RCSP-aggregator.hpp"
#include "core/scheduler.hpp"
#include "core/singleton-macro.hpp"

#include <list>
#include <string>

/**
 * @brief Class provides saving state of device that is devices parameters.
 *        This class has singleton interface, but can be instantiated directly
 *        if user need it anywhere.
  */
class StateSaver
{
public:
	static StateSaver& instance();
    void addValue(OperationCode code);
    void setFilename(const std::string& filename);

    void runSaver(uint32_t period);
    void stopSaver();

    void saveState();

    bool tryRestore();

private:
    bool tryRestore(uint8_t variant);

    std::string m_file[2];
    std::string m_fileLock[2];
    std::string m_fileCurrent[2];
    uint8_t m_current = 0, m_next = 1;

    TaskId m_savingTask = 0;

    std::list<OperationCode> m_codes;
    static StateSaver* m_saver;
    STATIC_DEINITIALIZER_IN_CLASS_DECLARATION;
};

#endif /* INCLUDE_RCSP_RCSP_STATE_SAVER_HPP_ */
