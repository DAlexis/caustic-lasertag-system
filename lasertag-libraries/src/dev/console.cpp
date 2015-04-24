/*
 * console.cpp
 *
 *  Created on: 24 дек. 2014 г.
 *      Author: alexey
 */

#include "dev/console.hpp"
#include <stdio.h>
#include <string.h>

Console *Console::m_console = nullptr;

STATIC_DEINITIALIZER_IN_CPP_FILE(Console, m_console)

Console::Console() :
    m_commandsCount(0)
{
}

void Console::init(uint8_t port)
{
    m_pUsart = USARTS->getUSART(port);
    clearBuffer();
    registerCommand("help", "Print this help message", std::bind(&Console::printHelp, this, std::placeholders::_1));
}

void Console::registerCommand(const char* command, const char* help, ConsoleCallback callback)
{
    m_commands.push_back(CommandDescr(callback, command, help));
}

Console::CommandDescr::CommandDescr() :
    callback(nullptr),
    command(nullptr),
    help(nullptr)
{

}

void Console::printHelp(const char*)
{
    printf("Commands list:\n");
    for (auto it = m_commands.begin(); it != m_commands.end(); it++)
        printf("    %s - %s\n", it->command, it->help);
}

void Console::clearBuffer()
{
    for (uint8_t i=0; i<CONSOLE_INPUT_BUFFER_SIZE; i++)
        m_buffer[i] = '\0';
}

void Console::fixBackspaces()
{
    uint8_t iw=0, ir=0;
    while (m_buffer[ir] != '\0' && ir < CONSOLE_INPUT_BUFFER_SIZE) {
        m_buffer[iw] = m_buffer[ir];
        if (m_buffer[ir++] != 0x08) iw++;
        else if (iw != 0) iw--;
    }
    m_buffer[iw] = '\0';
}

bool Console::checkEmpty()
{
    if (m_buffer[0] == '\0')
        return true;
    else
        return false;
}

void Console::prompt()
{
    m_pUsart->syncWrite("> ", 2);
    m_pUsart->asyncRead(m_buffer, '\r', 100, std::bind(&Console::receiveStringCb, this, std::placeholders::_1, std::placeholders::_2), 1);
}

int Console::receiveStringCb(char* buffer, int)
{
    buffer[CONSOLE_INPUT_BUFFER_SIZE-1] = '\0';
    fixBackspaces();
    if (checkEmpty()) {
        prompt();
        return 0;
    }
    printf("[%s]\n", buffer);

    char *first_space = buffer;
    while (*first_space != ' ' && *first_space != '\0')
        first_space++;

    m_argument = "";
    if (*first_space == ' ') {
        *first_space = '\0';
        m_argument = first_space+1;
    }

    auto it = m_commands.begin();
    for (; it != m_commands.end(); it++)
    {
        if (strcmp(buffer, it->command) == 0) {
            m_toCall = it->callback;
            break;
        }
    }

    if (it == m_commands.end())
    {
        printf("Unknown command\n");
        clearBuffer();
        prompt();
    }

    return 0;
}

void Console::interrogate()
{
    if (m_toCall)
    {
        m_toCall (m_argument);
        m_toCall = nullptr;
        clearBuffer();
        prompt();
    }
}

Console& Console::instance()
{
    if (!m_console)
        m_console = new Console;
    return *m_console;
}


