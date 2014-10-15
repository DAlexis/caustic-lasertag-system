/*
 * console.cpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#include "console.hpp"
#include "dynamic-memory.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <new>
#include <string.h>

Console* Console::m_self = nullptr;

void Console::InitConsole(USARTManager& usart)
{
    if (m_self == nullptr) {
        createInstance<Console, USARTManager&>(m_self, usart);
    }
}

Console& Console::getInstance()
{

    return *m_self;
}

Console::Console(USARTManager& usart) :
    m_pFisrtCommand(nullptr),
    m_usart(usart)
{
    // We suppose that usart is already initialized
    clearBuffer();
    registerCommand("help", "Print this help message", &Console::printHelp);
}

void Console::registerCommand(const char* command, const char* help, Cmdline_command_cb callback)
{
    CommandDescr** pplast_command = &m_pFisrtCommand;
    while (*pplast_command != 0)
        pplast_command = &( (*pplast_command)->pnext);
    createInstance((*pplast_command));
    (*pplast_command)->command = command;
    (*pplast_command)->callback = callback;
    (*pplast_command)->help = help;
}

Console::CommandDescr::CommandDescr() :
    pnext(nullptr),
    callback(nullptr),
    command(nullptr),
    help(nullptr)
{

}

void Console::printHelp(const char*)
{
    printf("Commands list:\n");
    CommandDescr** pplast_command = &(m_self->m_pFisrtCommand);
    while (*pplast_command != 0) {
        printf("    %s - %s\n", (*pplast_command)->command, (*pplast_command)->help);
        pplast_command = &( (*pplast_command)->pnext);
    }
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
    m_usart.write("> ", 2);
    m_usart.async_read(m_buffer, '\r', 100, &Console::receiveStringCb, 1);
}

int Console::receiveStringCb(char* buffer, int)
{
    buffer[BUFFER_SIZE-1] = '\0';
    m_self->fixBackspaces();
    if (m_self->checkEmpty()) {
        m_self->prompt();
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
    CommandDescr** pplast_command = &(m_self->m_pFisrtCommand);
    while (*pplast_command != 0) {
        if (strcmp(buffer, (*pplast_command)->command) == 0) {
            (* (*pplast_command)->callback) (argument);
            break;
        }
        pplast_command = &( (*pplast_command)->pnext);
    }
    if (*pplast_command == 0)
        printf("Unknown command\n");

    m_self->clearBuffer();
    m_self->prompt();
    return 0;
}
