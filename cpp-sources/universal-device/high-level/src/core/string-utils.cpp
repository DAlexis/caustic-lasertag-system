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


#include "core/string-utils.hpp"
#include "fatfs.h"
#include <string.h>

void printHex(const uint8_t* buffer, size_t size)
{
	for (size_t i=0; i<size; i++)
	{
		printf("%x ", buffer[i]);
	}
	printf("\n");
}

void formatTime(char* buffer, uint16_t min, uint16_t sec)
{
	unsigned int i=0;
	if (min == 0)
	{
		buffer[i++] = '0';
	} else {
		for (; min != 0; min /= 10)
		{
			buffer[i++] = min % 10 + '0';
		}

		for (unsigned int j=0; j<i/2; j++)
		{
			std::swap(buffer[j], buffer[i-1-j]);
		}
	}

	buffer[i++] = ':';
	buffer[i++] = sec / 10 + '0';
	buffer[i++] = sec % 10 + '0';
	buffer[i] = '\0';
}

void floatToString(char* buffer, float value, int bufferSize, int fracDigits)
{
	doubleToString(buffer, value, bufferSize, fracDigits);
}

void doubleToString(char* buffer, double value, int bufferSize, int fracDigits)
{
	unsigned int digits = value * 100.0;
	int pointPosition = 0;

	for (int p = bufferSize-1; p>=0; p--)
	{
		if (digits == 0 && pointPosition-p > 1)
			buffer[p] = ' ';
		else
		{
			buffer[p] = digits % 10 + '0';
			digits /= 10;
		}

		if (p == bufferSize-1 - 1)
		{
			buffer[--p] = '.';
			pointPosition = p;
		}
	}
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

bool checkPrefix(const char* where, const char* what)
{
	int whereLen = strlen(where);
	int whatLen = strlen(what);
	if (whereLen < whatLen)
		return false;

	for (int i=0; i<whatLen; i++)
		if (what[i] != where[i])
			return false;
	return true;
}


void IniParser::setCallback(AcceptKeyValueCallback callback)
{
	m_acceptKeyValueCallback = callback;
}

Result IniParser::parseFile(const char* filename)
{
	if (!m_acceptKeyValueCallback)
		return Result("Callback not set");

	group[0] = '\0';

	FIL* m_file = new FIL;

	FRESULT fres = f_open(m_file, filename, FA_OPEN_EXISTING | FA_READ);
	if (fres != FR_OK)
	{
		return Result("Cannot open file\n");
	}

	Result result;
	ParserState state = PS_WAITING_KEY;
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
							sprintf(m_errorMessage, "Expected \'=\' symbol at line %d\n", line);
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
							sprintf(m_errorMessage, "Expected value at line %d\n", line);
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
							m_acceptKeyValueCallback(group, key, value);
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
