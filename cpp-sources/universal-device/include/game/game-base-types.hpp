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


#ifndef INCLUDE_GAME_GAME_BASE_TYPES_HPP_
#define INCLUDE_GAME_GAME_BASE_TYPES_HPP_

#include "ir/MT2-base-types.hpp"

/// Player identifier for IR protocol
using PlayerGameId = uint8_t;
/// Team identifier. May be 1,2,3,4
using TeamGameId = uint8_t;
constexpr TeamGameId teamGameIdNotATeam = 5;

struct ShotMessage {
	ShotMessage(TeamGameId _teamId = 0, PlayerGameId _playerId = 0, UintParameter _damage = 0) :
		teamId(_teamId), playerId(_playerId), damage(_damage)
	{}
	TeamGameId teamId;
	PlayerGameId playerId;
	UintParameter damage;
};

namespace GameLog
{
#pragma pack(push, 1)
	/**
	 * Description of players damage by one concrete other player
	 */
	struct PvPDamageResults
	{
		PlayerGameId player = 0;
		uint16_t killsCount = 0; //< Players bullet killed player
		uint16_t hitsCount = 0; //< hits except kills
		uint32_t totalDamage = 0;
	};

	struct ShotsCounter
	{
		uint16_t count = 0;
	};
#pragma pack(pop)
}



#endif /* INCLUDE_GAME_GAME_BASE_TYPES_HPP_ */
