/*
 * dynamic-memory.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_DYNAMIC_MEMORY_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DYNAMIC_MEMORY_HPP_

#include <stdlib.h>
#include <new>

template <typename T, typename... ConstructorArgumentTypes>
void createInstance(T* &where, ConstructorArgumentTypes... args)
{
    where = (T*) malloc(sizeof(T));
    new (where) T(args...);
}

template <typename T>
void deleteInstance(T* &where)
{
    where->~T();
    free(where);
    where = nullptr;
}


#endif /* LAZERTAG_RIFLE_INCLUDE_DYNAMIC_MEMORY_HPP_ */
