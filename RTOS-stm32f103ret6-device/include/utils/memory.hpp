/*
 * memory.hpp
 *
 *  Created on: 21 июня 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_UTILS_MEMORY_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_UTILS_MEMORY_HPP_

#include <string.h>
#include <stdint.h>

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

uint32_t hashLyC(uint32_t hash, const uint8_t * buf, uint32_t size);

template<typename T1>
uint32_t hashLy(const T1& a1, uint32_t hash = 0)
{
	return hashLyC(hash, static_cast<const uint8_t *>(static_cast<const void*>(&a1)), sizeof(a1));
}

template<typename T1, typename T2>
uint32_t hashLy(const T1& a1, const T2& a2)
{
	uint32_t hash = hashLyC(0, static_cast<const uint8_t *>(static_cast<const void*>(&a1)), sizeof(a1));
	hash = hashLyC(hash, static_cast<const uint8_t *>(static_cast<const void*>(&a2)), sizeof(a2));
	return hash;
}

template<typename T1, typename T2, typename T3>
uint32_t hashLy(const T1& a1, const T2& a2, const T3& a3)
{
	uint32_t hash = hashLyC(0, static_cast<const uint8_t *>(static_cast<const void*>(&a1)), sizeof(T1));
	hash = hashLyC(hash, static_cast<const uint8_t *>(static_cast<const void*>(&a2)), sizeof(T2));
	hash = hashLyC(hash, static_cast<const uint8_t *>(static_cast<const void*>(&a3)), sizeof(T3));
	return hash;
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
