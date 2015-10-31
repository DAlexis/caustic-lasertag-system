/*
 * string-utils.cpp
 *
 *  Created on: 25 июня 2015 г.
 *      Author: alexey
 */

#include "core/string-utils.hpp"
#include "fatfs.h"
#include <string.h>

void printHex(uint8_t* buffer, size_t size)
{
	for (size_t i=0; i<size; i++)
	{
		printf("%x ", buffer[i]);
	}
	printf("\n");
}

const char* parseFRESULT(int res)
{
	switch (res)
	{
	case FR_OK:			return "(0) Succeeded";
	case FR_DISK_ERR:	return "(1) A hard error occurred in the low level disk I/O layer ";
	case FR_INT_ERR:	return "(2) Assertion failed ";
	case FR_NOT_READY:	return "(3) The physical drive cannot work ";
	case FR_NO_FILE:	return "(4) Could not find the file ";
	case FR_NO_PATH:	return "(5) Could not find the path ";
	case FR_INVALID_NAME:		return "(6) The path name format is invalid ";
	case FR_DENIED:				return "(7) Access denied due to prohibited access or directory full ";
	case FR_EXIST:				return "(8) Access denied due to prohibited access ";
	case FR_INVALID_OBJECT:		return "(9) The file/directory object is invalid ";
	case FR_WRITE_PROTECTED:	return "(10) The physical drive is write protected ";
	case FR_INVALID_DRIVE:		return "(11) The logical drive number is invalid ";
	case FR_NOT_ENABLED:		return "(12) The volume has no work area ";
	case FR_NO_FILESYSTEM:		return "(13) There is no valid FAT volume ";
	case FR_MKFS_ABORTED:		return "(14) The f_mkfs() aborted due to any parameter error ";
	case FR_TIMEOUT:			return "(15) Could not get a grant to access the volume within defined period ";
	case FR_LOCKED:				return "(16) The operation is rejected according to the file sharing policy ";
	case FR_NOT_ENOUGH_CORE:	return "(17) LFN working buffer could not be allocated ";
	case FR_TOO_MANY_OPEN_FILES:	return "(18) Number of open files > _FS_SHARE ";
	case FR_INVALID_PARAMETER:	return "(19) Given parameter is invalid ";
	default:
		return "Invalid FRESULT value!";
	}
}


void printBar(int barLength, int filled)
{
	printf("[");
	for (int i=0; i < filled-1; i++)
		printf("=");
	printf(">");
	for (int i=0; i < barLength-1-filled; i++)
		printf(" ");
	printf("]");
}

bool checkSuffix(const char* where, const char* what)
{
	int whereLen = strlen(where);
	int whatLen = strlen(what);
	if (whereLen < whatLen)
		return false;

	for (int i=0; i<whatLen; i++)
		if (what[i] != where[whereLen-whatLen+i])
			return false;

	return true;
}


void IniParcer::setCallback(AcceptKeyValueCallback callback)
{
	m_acceptKeyValueCallback = callback;
}

Result IniParcer::parseFile(const char* filename)
{
	if (!m_acceptKeyValueCallback)
		return Result("Callback not set");

	FIL* m_file = new FIL;

	FRESULT fres = f_open(m_file, filename, FA_OPEN_EXISTING | FA_READ);
	if (fres != FR_OK)
	{
		return Result("Cannot open file\n");
	}

	Result result;
	ParcerState state = PS_WAITING_KEY;
	unsigned int keyCursor = 0, valueCursor = 0;
	unsigned int line = 1;
	bool error = false;

	while (!f_eof(m_file) && !error)
	{
		UINT readed = 0;
		unsigned int cursor = 0;
		fres = f_read(m_file, buffer, blockSize, &readed);
		if (fres != FR_OK) {
			return Result(parseFRESULT(fres));
		}
		// Skipping spaces
		while (cursor < readed && !error)
		{
			switch (state)
			{
				case PS_COMMENT:
					while (cursor < readed && buffer[cursor] != '\n')
						cursor++;

					if (cursor != readed) {
						state = PS_WAITING_KEY;
						cursor++;
						line++;
					}
					break;


				case PS_WAITING_KEY:
					for (; cursor < readed; cursor++)
					{
						if (buffer[cursor] == '#')
						{
							state = PS_COMMENT;
							cursor++;
							break;
						}

						if (buffer[cursor] == '\n')
						{
							line++;
							continue;
						}

						if (isCharacter(buffer[cursor]))
						{
							state = PS_READING_KEY;
							break;
						}
						if (!isSpace(buffer[cursor]))
						{
							sprintf(m_errorMessage, "Expected key at line %d\n", line);
							result.fail(m_errorMessage);
							error = true;
							break;
						}
					}
					break;

				case PS_READING_KEY:
					for (; cursor < readed; cursor++)
					{
						if (buffer[cursor] == '=' || isSpace(buffer[cursor]))
						{
							state = PS_WAITING_EQ;
							key[keyCursor] = '\0';
							break;
						}

						if (buffer[cursor] == '\n')
						{
							sprintf(m_errorMessage, "Expected \'= value\' at line %d\n", line);
							result.fail(m_errorMessage);
							error = true;
							break;
						}

						if (!isCharacter(buffer[cursor]) && !isNumber(buffer[cursor]))
						{
							sprintf(m_errorMessage, "Unexpected character in key name at line %d\n", line);
							result.fail(m_errorMessage);
							error = true;
							break;
						}
						key[keyCursor++] = buffer[cursor];
						if (keyCursor == keyValMaxLength-2)
						{
							sprintf(m_errorMessage, "Key is too long at line %d\n", line);
							result.fail(m_errorMessage);
							error = true;
							break;
						}
					}
					break;


				case PS_WAITING_EQ:
					for (; cursor < readed; cursor++)
					{
						if (buffer[cursor] == '=')
						{
							state = PS_WAITING_VALUE;
							cursor++;
							break;
						}

						if (!isSpace(buffer[cursor]))
						{
							sprintf(m_errorMessage, "Expexted \'=\' symbol at line %d\n", line);
							result.fail(m_errorMessage);
							error = true;
							break;
						}
					}
					break;

				case PS_WAITING_VALUE:
					for (; cursor < readed; cursor++)
					{
						if (isCharacter(buffer[cursor]) || isNumber(buffer[cursor]))
						{
							state = PS_READING_VALUE;
							break;
						}
						if (!isSpace(buffer[cursor]))
						{
							sprintf(m_errorMessage, "Expexted value at line %d\n", line);
							result.fail(m_errorMessage);
							error = true;
							break;
						}
					}
					break;

				case PS_READING_VALUE:
					for (; cursor < readed; cursor++)
					{
						if (buffer[cursor] == '\n')
						{
							state = PS_WAITING_KEY;
							value[valueCursor] = '\0';
							cursor++;
							line++;
							m_acceptKeyValueCallback(key, value);
							valueCursor = 0;
							keyCursor = 0;
							break;
						}

						value[valueCursor++] = buffer[cursor];
						if (valueCursor == keyValMaxLength-2)
						{
							sprintf(m_errorMessage, "Value is too long at line %d\n", line);
							result.fail(m_errorMessage);
							error = true;
							break;
						}
					}
					break;
			}
		}
	}
	f_close(m_file);
	delete m_file;
	return result;
}
