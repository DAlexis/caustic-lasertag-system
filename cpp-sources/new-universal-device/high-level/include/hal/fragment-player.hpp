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

#ifndef LAZERTAG_RIFLE_INCLUDE_HAL_FRAGMENT_PLAYER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HAL_FRAGMENT_PLAYER_HPP_

#include <functional>

using SoundSample = uint16_t;
using FragmentDoneCallback = std::function<void(SoundSample* /*lastPlayedBuffer*/)>;

class IFragmentPlayer
{
public:
	virtual ~IFragmentPlayer() {}
	virtual void setFragmentSize(uint32_t size) = 0;
	virtual void init() = 0;
	virtual void setFragmentDoneCallback(FragmentDoneCallback callback) = 0;
	virtual void playFragment(SoundSample* buffer) = 0;
	virtual void stop() = 0;
};

class FragmentPlayerBase : public IFragmentPlayer
{
public:
	void setFragmentSize(uint32_t size) { m_size = size; }
	void setFragmentDoneCallback(FragmentDoneCallback callback) { m_callback = callback; }

protected:
	uint32_t m_size;
	SoundSample* m_buffer = nullptr;
	FragmentDoneCallback m_callback;
};

class IFragmentPlayerCreator
{
public:
	virtual ~IFragmentPlayerCreator() {}
	virtual IFragmentPlayer* get() = 0;
};

extern IFragmentPlayerCreator* fragmentPlayerCreator;


#endif /* LAZERTAG_RIFLE_INCLUDE_HAL_FRAGMENT_PLAYER_HPP_ */
