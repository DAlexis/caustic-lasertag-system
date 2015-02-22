/*
 * fragment-player.hpp
 *
 *  Created on: 09 янв. 2015 г.
 *      Author: alexey
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
	void DMAInterruptionHandler();
	void stop();

private:
	void stopFragment();
	DMA_InitTypeDef m_DMA_InitStructure;
};



#endif /* LAZERTAG_RIFLE_INCLUDE_HW_FRAGMENT_PLAYER_HPP_ */
