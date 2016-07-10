/*
 * rifle-MFRC522-reader.hpp
 *
 *  Created on: 6 апр. 2016 г.
 *      Author: alexey
 */

#ifndef INCLUDE_RIFLE_RIFLE_MFRC522_READER_HPP_
#define INCLUDE_RIFLE_RIFLE_MFRC522_READER_HPP_

#include "dev/RC522.hpp"
#include "core/os-wrappers.hpp"
#include <functional>

class RC552Wrapper : public IInterrogatable
{
public:
	RC552Wrapper();
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
