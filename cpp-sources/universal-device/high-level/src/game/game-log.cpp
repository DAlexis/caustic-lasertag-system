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

#include "rcsp/stream.hpp"
#include "game/game-log.hpp"
#include "rcsp/operation-codes.hpp"
#include "core/logging.hpp"
#include "string.h"

using namespace GameLog;

const char* BaseStatsCounter::filename = "base-stat.bin";

BaseStatsCounter::BaseStatsCounter(const PlayerGameId& hostPlayerId, INetworkClientSender& networkClientSender) :
		m_hostPlayerId(hostPlayerId),
		m_networkClientSender(networkClientSender)
{

}

void BaseStatsCounter::checkAndCreate(PlayerGameId enemy)
{
	auto it = m_results.find(enemy);
	if (it == m_results.end())
	{
		m_results[enemy] = PvPDamageResults();
		m_results[enemy].enemyId = enemy;
		m_results[enemy].victimId = m_hostPlayerId;
		m_iteratorCorrupted = true;
	}
}

void BaseStatsCounter::registerDamage(PlayerGameId player, uint16_t damage)
{
	checkAndCreate(player);
	m_results[player].totalDamage += damage;
}

void BaseStatsCounter::registerHit(PlayerGameId player)
{
	checkAndCreate(player);
	m_results[player].hitsCount++;
}

void BaseStatsCounter::registerKill(PlayerGameId player)
{
	checkAndCreate(player);
	m_results[player].killsCount++;
}

void BaseStatsCounter::clear()
{
	m_results.clear();
	saveState();
}

void BaseStatsCounter::saveState()
{
	FRESULT res = FR_OK;
	info << "<Save stats> Saving stats";
	res = f_open(&m_file, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if (res != FR_OK)
	{
		error << "<Save stats> Cannot open file to write base stats: " << res;
		return;
	}

	for (auto it = m_results.begin(); it != m_results.end(); it++)
	{
		UINT written = 0;
		res = f_write(&m_file, & (it->second), sizeof(PvPDamageResults), &written);
		if (res != FR_OK)
		{
			error << "<Save stats> Cannot write base stats to file: " << res;
			break;
		}
		if (written != sizeof(PvPDamageResults))
		{
			error << "<Save stats> Wrong size written to file: " << written << " instead of " << sizeof(PvPDamageResults);
			break;
		}
	}
	f_close(&m_file);
}

void BaseStatsCounter::restoreFromFile()
{
	FRESULT res = FR_OK;
	res = f_open(&m_file, filename, FA_READ);
	if (res != FR_OK)
	{
		warning << "<Restore stats> Cannot open file to read base stats: " << res;
		return;
	}

	while (!f_eof(&m_file))
	{
		UINT readed = 0;
		PvPDamageResults tmp;
		res = f_read(&m_file, &tmp, sizeof(PvPDamageResults), &readed);
		if (res != FR_OK)
		{
			error << "<Restore stats> Cannot write base stats from file: " << res;
			break;
		}
		if (readed != sizeof(PvPDamageResults))
		{
			error << "<Restore stats> Wrong size readed from file: " << readed << " instead of " << sizeof(PvPDamageResults) << " or 0";
			break;
		}
		m_results[tmp.enemyId] = tmp;
	}

	f_close(&m_file);
}

void BaseStatsCounter::interrogate()
{
	switch(m_sendingState)
	{
	case S_READY_TO_TRANSMIT:
		sendNextPackage();
		break;
	case S_WAIT_DELAY_AFTER_CHUNK:
		waitDelay();
		break;
	case S_NOTHING:
	default:
		break;
	}
}

void BaseStatsCounter::sendStats()
{
	m_statsReceiver = broadcast.any;
	prepareTransmission();
}


void BaseStatsCounter::sendNextPackage()
{
	ScopedLock<CritialSection> lock(m_iteratorCheck);
		if (m_iteratorCorrupted)
		{
			prepareTransmission();
			return;
		}
		// Iterator is fine here
		if (m_sendingIterator == m_results.end())
		{
			// All data transmitted
			m_sendingState = S_NOTHING;
			return;
		}

		PvPDamageResults target = m_sendingIterator->second;
		m_sendingIterator++;
		// Now we have valid object and we can send it
	lock.unlock();
	stateToDelayAfterChunk();
	RCSPStream::call(
        &m_networkClientSender,
		m_statsReceiver,
		ConfigCodes::Base::Functions::getPvPResults,
		target,
		false,
		nullptr,
		PackageTimings(false, 2000000)
	);

}

void BaseStatsCounter::prepareTransmission()
{
	m_sendingIterator = m_results.begin();
	m_iteratorCorrupted = false;
	m_sendingState = S_READY_TO_TRANSMIT;
}

void BaseStatsCounter::waitDelay()
{
	if (systemClock->getTime() - m_waitingBeginned >= delayAfterChunk)
	{
		m_sendingState = S_READY_TO_TRANSMIT;
	}
}

void BaseStatsCounter::stateToDelayAfterChunk()
{
	m_waitingBeginned = systemClock->getTime();
	m_sendingState = S_WAIT_DELAY_AFTER_CHUNK;
}

Event::Event()
{
	memset(eventParameter, 0, eventParameterSize);
}
