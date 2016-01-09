/*
 * memory.hpp
 *
 *  Created on: 21 июня 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_UTILS_MEMORY_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_UTILS_MEMORY_HPP_

#include <string.h>

template <typename T>
void zerify(T& object)
{
	memset(&object, 0, sizeof(T));
}

template <typename T>
void copyToBuffer(void* buffer, const T& object)
{
	memcpy(buffer, (void*) &object, sizeof(T));
}

template <typename T>
T& interpretBuffer(void* buffer)
{
	return *reinterpret_cast<T*>(buffer);
}

template <typename T>
class ChangeWatcher
{
public:
	ChangeWatcher(const T& original) { init(original); }
	void init(const T& original) { m_original = &original; m_stored = *m_original; }
	bool isChanged() { return m_stored == *m_original; }
	void update() { m_stored = *m_original; }
	T& get() { return m_stored; }
private:
	const T* m_original;
	T m_stored;
};


#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_UTILS_MEMORY_HPP_ */
