/*
 * rifle-MFRC522-reader.cpp
 *
 *  Created on: 6 апр. 2016 г.
 *      Author: alexey
 */

#include "rifle/MFRC522-wrapper.hpp"
#include "core/logging.hpp"
#include "core/string-utils.hpp"

#include <algorithm>

RC552Wrapper::RC552Wrapper()
{
	// Prepare the key (used both as key A and as key B)
	// using FFFFFFFFFFFFh which is the default at chip delivery from the factory
	for (byte i = 0; i < 6; i++) {
		m_key.keyByte[i] = 0xFF;
	}
}

void RC552Wrapper::init()
{
	MFRC522::RC522IO io;
	io.spi = SPIs->getSPI(3);
	io.chipSelect = IOPins->getIOPin(0, 15);
	io.resetPowerDown = IOPins->getIOPin(1, 11);
	m_mfrc.PCD_Init(io);
	m_mfrc.PCD_SetAntennaGain(0x07<<4);
}

void RC552Wrapper::interrogate()
{
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

void RC552Wrapper::readBlock(DataReadingDoneCallback callback, byte size)
{
	m_readCallback = callback;
	m_inputBufferSize = size;
	m_currentLoop = [this] { readBlockLoop(); };
}

void RC552Wrapper::writeBlock(uint8_t* data, uint16_t size)
{
	m_dataToWrite = data;
	m_dataToWriteSize = size;
	m_currentLoop = [this] { writeBlockLoop(); };
}

void RC552Wrapper::readBlockLoop()
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
		m_readCallback(m_inputBuffer, m_inputBufferSize);
}

void RC552Wrapper::writeBlockLoop()
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
}

bool RC552Wrapper::cardReadyToOperate()
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
