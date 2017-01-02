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


#ifndef INCLUDE_RIFLE_RIFLE_DISPLAY_HPP_
#define INCLUDE_RIFLE_RIFLE_DISPLAY_HPP_

#include "rifle-config-and-state.hpp"

class RifleDisplayBase
{
public:
    virtual ~RifleDisplayBase() {}
    virtual bool init() = 0;
    virtual void update() = 0;

    void setData(const RifleOwnerConfiguration *owner, const RifleState *state, const PlayerPartialState *playerState);

protected:
    const RifleOwnerConfiguration *m_owner;
    const RifleState *m_state;
    const PlayerPartialState *m_playerState;
};




#endif /* INCLUDE_RIFLE_RIFLE_DISPLAY_HPP_ */
