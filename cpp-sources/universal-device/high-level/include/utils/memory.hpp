/*
*    Copyright (C) 2016-2017 by Aleksey Bulatov
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

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_UTILS_MEMORY_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_UTILS_MEMORY_HPP_

#include <string.h>
#include <stdint.h>

struct AnyBuffer
{
    AnyBuffer(uint16_t size, const void *data = nullptr);
    ~AnyBuffer();
    uint8_t* data;
    const uint16_t size;
};


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
