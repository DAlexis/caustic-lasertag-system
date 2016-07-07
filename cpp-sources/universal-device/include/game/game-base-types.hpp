/*
 * game-base-types.hpp
 *
 *  Created on: 28 янв. 2016 г.
 *      Author: alexey
 */

#ifndef INCLUDE_GAME_GAME_BASE_TYPES_HPP_
#define INCLUDE_GAME_GAME_BASE_TYPES_HPP_

#include "ir/MT2-base-types.hpp"

namespace GameLog
{
#pragma pack(push, 1)
	/**
	 * Description of players damage by one concrete other player
	 */
	struct PvPDamageResults
	{
		PlayerMT2Id player = 0;
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
