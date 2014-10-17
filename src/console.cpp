/*
 * console.cpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#include "console.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <new>
#include <string.h>

Console console;

Console::Console() :
    m_commandsCount(0)
{
}

void Console::init(USARTManager& usart)
{
    m_pUsart = &usart;
    // We suppose that usart is already initialized
    clearBuffer();
    registerCommand("help", "Print this help message", &Console::printHelp);
}

void Console::registerCommand(const char* command, const char* help, Cmdline_command_cb callback)
{
    m_commands[m_commandsCount].command = command;
    m_commands[m_commandsCount].callback = callback;
    m_commands[m_commandsCount].help = help;
    ++m_commandsCount;
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
    for (unsigned int i=0; i<console.m_commandsCount; i++)
        printf("    %s - %s\n", console.m_commands[i].command, console.m_commands[i].help);

}

void Console::clearBuffer()
{
    for (uint8_t i=0; i<BUFFER_SIZE; i++)
        m_buffer[i] = '\0';
}

void Console::fixBackspaces()
{
    uint8_t iw=0, ir=0;
    while (m_buffer[ir] != '\0' && ir < BUFFER_SIZE) {
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
    m_pUsart->write("> ", 2);
    m_pUsart->async_read(m_buffer, '\r', 100, &Console::receiveStringCb, 1);
}

int Console::receiveStringCb(char* buffer, int)
{
    buffer[BUFFER_SIZE-1] = '\0';
    console.fixBackspaces();
    if (console.checkEmpty()) {
        console.prompt();
        return 0;
    }
    printf(">>> %s\n", buffer);

    char *first_space = buffer;
    while (*first_space != ' ' && *first_space != '\0')
        first_space++;

    const char *argument = "";
    if (*first_space == ' ') {
        *first_space = '\0';
        argument = first_space+1;
    }
    unsigned int i=0;
    for (; i<console.m_commandsCount; i++)
    {
        if (strcmp(buffer, console.m_commands[i].command) == 0) {
            (* console.m_commands[i].callback) (argument);
            break;
        }
    }

    if (i == console.m_commandsCount)
        printf("Unknown command\n");

    console.clearBuffer();
    console.prompt();
    return 0;
}
