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

#ifndef LAZERTAG_RIFLE_INCLUDE_HW_FRAGMENT_PLAYER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HW_FRAGMENT_PLAYER_HPP_

#include "hal/fragment-player.hpp"
#include "stm32f10x.h"

class FragmentPlayer : public FragmentPlayerBase
{
public:
	FragmentPlayer();
	void playFragment(SoundSample* buffer);
	void init();
	void DMAInterruptionHandler();
	void stop();

private:
	void stopFragment();
	DMA_InitTypeDef m_DMA_InitStructure;
};

class FragmentPlayerCreator : public IFragmentPlayerCreator
{
public:
	IFragmentPlayer* get() override;

private:
	FragmentPlayer* m_fragmentPlayer = nullptr;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_HW_FRAGMENT_PLAYER_HPP_ */
