/*
 * utils.hpp
 *
 *  Created on: 25 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_UTILS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_UTILS_HPP_

#include "stm32f10x.h"
#include <stdint.h>

//#define SYSTICK_FREQUENCY_HZ       100000


class SysTicTimer
{
public:
    SysTicTimer();
    void delay(__IO uint32_t timeToWait);

/*
    template<typename FunctionType, typename... Args>
    void delay_async(__IO uint32_t time, FunctionType func, Args... args)
    {
        func(args...);
    }
    */
    //void waitAndRun

    void interrupt();

    // Returns time in microseconds from MCU start
    uint32_t getTime();
    uint32_t getTime2();
    uint32_t getMs();
    uint32_t getSlave();

private:
    __IO uint32_t time;


};

extern SysTicTimer systemTimer;

#endif /* LAZERTAG_RIFLE_INCLUDE_UTILS_HPP_ */
