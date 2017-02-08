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


#include <dev/RC522-wrapper.hpp>
#include "core/logging.hpp"
#include "core/string-utils.hpp"

#include <algorithm>

RC552Frontend::RC552Frontend()
{
	// Prepare the key (used both as key A and as key B)
	// using FFFFFFFFFFFFh which is the default at chip delivery from the factory
	for (byte i = 0; i < 6; i++) {
		m_key.keyByte[i] = 0xFF;
	}
}

void RC552Frontend::init()
{
	MFRC522::RC522IO io;
	io.spi = SPIs->getSPI(3);
	io.chipSelect = IOPins->getIOPin(0, 15);
	io.resetPowerDown = IOPins->getIOPin(1, 6);
	m_initResult = m_mfrc.PCD_Init(io);
	if (m_initResult == false)
		return;
	m_mfrc.PCD_SetAntennaGain(0x07<<4);
}

void RC552Frontend::interrogate()
{
	if (m_initResult == false)
		return;

	Time nowtime = systemClock->getTime();
	if (nowtime - m_lastReinitTime > reinitPeriod)
	{
		info << "RFID reader reinit to prevent broken state";
		init();
		m_lastReinitTime = nowtime;
	}

	if (m_currentLoop)
	{
		m_mfrc.configureSPI();
		m_currentLoop();
	}
}

void RC552Frontend::readBlock(RWDoneCallback callback, byte size)
{
	m_readCallback = callback;
	m_inputBufferSize = size;
	m_currentLoop = [this] { readBlockLoop(); };
}

void RC552Frontend::writeBlock(uint8_t* data, uint16_t size, RWDoneCallback callback)
{
	m_dataToWrite = data;
	m_dataToWriteSize = size;
	m_writeDoneCallback = callback;
	m_currentLoop = [this] { writeBlockLoop(); };
}

void RC552Frontend::readBlockLoop()
{
	if (!cardReadyToOperate())
		return;

	MFRC522::StatusCode status;

	// Authenticate using key A
	debug << "Authenticating using key A...";
	status = (MFRC522::StatusCode) m_mfrc.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &m_key, &(m_mfrc.uid));
	if (status != MFRC522::STATUS_OK) {
		error << "PCD_Authenticate() failed: " << m_mfrc.GetStatusCodeName(status);
		return;
	}

	debug << "Reading data from block " << blockAddr << " with size " << m_inputBufferSize;
	status = (MFRC522::StatusCode) m_mfrc.MIFARE_Read(blockAddr, m_inputBuffer, &m_inputBufferSize);
	if (status != MFRC522::STATUS_OK) {
		error << "MIFARE_Read() failed: " << m_mfrc.GetStatusCodeName(status);
	}

	// Halt PICC
	m_mfrc.PICC_HaltA();
	// Stop encryption on PCD
	m_mfrc.PCD_StopCrypto1();

	debug << "Card reading done";

	if (m_readCallback)
	{
		m_readCallback(m_inputBuffer, m_inputBufferSize);
	}
}

void RC552Frontend::writeBlockLoop()
{
	if (!cardReadyToOperate())
		return;

	MFRC522::StatusCode status;

	// Authenticate using key B
	status = (MFRC522::StatusCode) m_mfrc.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &m_key, &(m_mfrc.uid));
	if (status != MFRC522::STATUS_OK) {
		error << "PCD_Authenticate() failed: " << m_mfrc.GetStatusCodeName(status);
		return;
	}

	// Write data to the block
	status = (MFRC522::StatusCode) m_mfrc.MIFARE_Write(blockAddr, m_dataToWrite, m_dataToWriteSize);
	if (status != MFRC522::STATUS_OK) {
		error << "MIFARE_Write() failed: " << m_mfrc.GetStatusCodeName(status);
	}
	//Serial.println();

	byte readedSize = std::max(18, (int) m_dataToWriteSize);
	byte readBufferSize = readedSize;
	// Verification
	byte tmp[readedSize];

	// Read data from the block (again, should now be what we have written)
	//Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
	//Serial.println(F(" ..."));
	status = (MFRC522::StatusCode) m_mfrc.MIFARE_Read(blockAddr, tmp, &readedSize);
	if (status != MFRC522::STATUS_OK) {
		error << "MIFARE_Read() failed: " << m_mfrc.GetStatusCodeName(status);
	}
	if (readBufferSize < readedSize)
	{
		error << "MIFARE_Read() failed: readed " << readedSize << " instead of " << readBufferSize;
	} else {
		for (int i=0; i<m_dataToWriteSize; i++)
			if (tmp[i] != m_dataToWrite[i])
			{
				error << "Data verification failed";
				break;
			}
	}

	// Halt PICC
	m_mfrc.PICC_HaltA();
	// Stop encryption on PCD
	m_mfrc.PCD_StopCrypto1();

	debug << "Card writing done";
	if (m_writeDoneCallback)
	{
		m_writeDoneCallback(m_dataToWrite, m_dataToWriteSize);
		m_writeDoneCallback = nullptr;
	}
}

bool RC552Frontend::cardReadyToOperate()
{
	// Look for new cards
	if ( ! m_mfrc.PICC_IsNewCardPresent())
		return false;

	// Select one of the cards
	if ( ! m_mfrc.PICC_ReadCardSerial())
		return false;

	// Show some details of the PICC (that is: the tag/card)
	//Serial.print(F("Card UID:"));
	//dump_byte_array(m_mfrc.uid.uidByte, m_mfrc.uid.size);
	//Serial.println();
	//Serial.print(F("PICC type: "));
	MFRC522::PICC_Type piccType = m_mfrc.PICC_GetType(m_mfrc.uid.sak);
	debug << "PICC type: " << m_mfrc.PICC_GetTypeName(piccType);
	//Serial.println(m_mfrc.PICC_GetTypeName(piccType));

	// Check for compatibility
	if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
		&&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
		&&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
		error << "Reading only works with MIFARE Classic cards.";
		return false;
	}
	return true;
}
