/*
 * console.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_CONSOLE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_CONSOLE_HPP_

#include "usart.hpp"
#include <functional>

#define BUFFER_SIZE     100
#define COMMAND_DESCR_COUNT_MAX     20

typedef void (*Cmdline_command_cb)(const char* buffer);

class Console
{
public:
    Console();
    void init(USARTManager& usart);
    void registerCommand(const char* command, const char* help, Cmdline_command_cb callback);
    void prompt();


private:
    struct CommandDescr {
        CommandDescr();
        Cmdline_command_cb callback;
        const char* command;
        const char* help;
    };


    static void printHelp(const char* buffer);
    static int receiveStringCb(char* buffer, int bytesCount);
    void clearBuffer();
    void fixBackspaces();
    bool checkEmpty();


    CommandDescr m_commands[COMMAND_DESCR_COUNT_MAX];
    char m_buffer[BUFFER_SIZE];
    USARTManager* m_pUsart;
    unsigned int m_commandsCount;
};

extern Console console;
#endif /* LAZERTAG_RIFLE_INCLUDE_CONSOLE_HPP_ */
