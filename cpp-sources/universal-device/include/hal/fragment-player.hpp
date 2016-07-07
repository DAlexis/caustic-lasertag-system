/*
 * fragment-player.hpp
 *
 *  Created on: 08 янв. 2015 г.
 *      Author: alexey
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

extern IFragmentPlayer* fragmentPlayer;

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





#endif /* LAZERTAG_RIFLE_INCLUDE_HAL_FRAGMENT_PLAYER_HPP_ */
