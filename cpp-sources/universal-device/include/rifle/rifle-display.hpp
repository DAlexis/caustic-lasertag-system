/*
 * rifle-display.hpp
 *
 *  Created on: 4 апр. 2016 г.
 *      Author: alexey
 */

#ifndef INCLUDE_RIFLE_RIFLE_DISPLAY_HPP_
#define INCLUDE_RIFLE_RIFLE_DISPLAY_HPP_

#include "rifle-config-and-state.hpp"
#include "dev/lcd5110.hpp"

class RifleLCD5110Display
{
public:
	RifleLCD5110Display(const RifleOwnerConfiguration *owner, const RifleState *state, const PlayerPartialState *playerState);
	void init();
	void update();

private:
	LCD5110Controller m_lcd;
	const RifleOwnerConfiguration *m_owner;
	const RifleState *m_state;
	const PlayerPartialState *m_playerState;
};





#endif /* INCLUDE_RIFLE_RIFLE_DISPLAY_HPP_ */
