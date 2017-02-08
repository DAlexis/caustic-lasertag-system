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

#ifndef INCLUDE_RIFLE_RIFLE_MFRC522_READER_HPP_
#define INCLUDE_RIFLE_RIFLE_MFRC522_READER_HPP_

#include "dev/RC522.hpp"
#include "core/os-wrappers.hpp"
#include <functional>

/**
 * This class is to simplify IO with RC552 card. It supports simply writing and reading data
 */
class RC552Frontend : public IInterrogatable
{
public:
	RC552Frontend();
	using RWDoneCallback = std::function<void(uint8_t*, uint16_t)>;
	void init();

	void interrogate() override;
	void readBlock(RWDoneCallback callback, byte size);
	void writeBlock(uint8_t* data, uint16_t size, RWDoneCallback callback = nullptr);

private:
	constexpr static Time reinitPeriod = 10000000;
	using LoopFunc = std::function<void()>;

	constexpr static uint16_t inputBufferMaxSize = 32;

	constexpr static byte trailerBlock   = 7;
	constexpr static byte sector         = 1;
	constexpr static byte blockAddr      = 4;

	bool cardReadyToOperate();
	void readBlockLoop();
	void writeBlockLoop();

	bool m_initResult = false;

	RWDoneCallback m_readCallback = nullptr;
	RWDoneCallback m_writeDoneCallback = nullptr;
	uint8_t* m_dataToWrite = nullptr;
	uint16_t m_dataToWriteSize = 0;

	uint8_t m_inputBuffer[inputBufferMaxSize];
	byte m_inputBufferSize = 0;

	// Last operated card UID to prevent double operating
	byte m_nuidPICC[3];
	MFRC522 m_mfrc;
	LoopFunc m_currentLoop = nullptr;

	MFRC522::MIFARE_Key m_key;

	Time m_lastReinitTime = 0;
};




#endif /* INCLUDE_RIFLE_RIFLE_MFRC522_READER_HPP_ */
