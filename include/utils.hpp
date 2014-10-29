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

#define SYSTICK_FREQUENCY_HZ       1000

class SysTicTimer
{
public:
    SysTicTimer();
    void delay(__IO uint32_t time);

    template<typename FunctionType, typename... Args>
    void delay_async(__IO uint32_t time, FunctionType func, Args... args)
    {
        func(args...);
    }
    //void waitAndRun

    void decrement();
private:
    __IO uint32_t m_counter;

};

extern SysTicTimer systemTimer;

#endif /* LAZERTAG_RIFLE_INCLUDE_UTILS_HPP_ */
