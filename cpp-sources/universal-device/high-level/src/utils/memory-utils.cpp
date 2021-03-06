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

#include "utils/memory.hpp"

AnyBuffer::AnyBuffer(uint16_t size, const void *data) :
    size(size)
{
    this->data = nullptr;
    this->data = new uint8_t[size];
    if (this->data != nullptr && data != nullptr)
        memcpy(this->data, data, this->size);
}

AnyBuffer::~AnyBuffer()
{
    if (data)
        delete[] data;
}

uint32_t hashLyC(uint32_t hash, const uint8_t * buf, uint32_t size)
{
	for(uint32_t i=0; i<size; i++)
		hash = (hash * 1664525) + buf[i] + 1013904223;

	return hash;
}
