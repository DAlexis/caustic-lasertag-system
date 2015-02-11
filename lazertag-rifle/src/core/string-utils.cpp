/*
 * string-utils.cpp
 *
 *  Created on: 07 февр. 2015 г.
 *      Author: alexey
 */

#include "core/string-utils.hpp"
#include "hal/ff/ff.h"

#include <stdio.h>

void printBar(int barLength, int filled)
{
	printf("[");
	for (int i=0; i < filled-1; i++)
		printf("=");
	printf(">");
	for (int i=0; i < barLength-1-filled; i++)
		printf(" ");
	printf("]\n");
}

void IniParcer::setCallback(AcceptKeyValueCallback callback)
{
	m_acceptKeyValueCallback = callback;
}

Result IniParcer::parseFile(const char* filename)
{
	if (!m_acceptKeyValueCallback)
		return Result("Callback not set");

	FIL file;
	FRESULT fres = f_open(&file, filename, FA_OPEN_EXISTING | FA_READ);
	if (fres != FR_OK)
	{
		return Result("Cannot open file\n");
	}

	Result result;
	ParcerState state = PS_WAITING_KEY;
	unsigned int keyCursor = 0, valueCursor = 0;
	unsigned int line = 1;
	bool error = false;

	while (!f_eof(&file) && !error)
	{
		UINT readed = 0;
		unsigned int cursor = 0;
		f_read(&file, buffer, blockSize, &readed);
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
	f_close(&file);
	return result;
}
