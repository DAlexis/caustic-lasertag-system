/*
 * console.hpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_CONSOLE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_CONSOLE_HPP_

#include "usart-manager.hpp"
#include <functional>

#define BUFFER_SIZE     100

typedef void (*Cmdline_command_cb)(const char* buffer);
//typedef std::function<void (char*)> Cmdline_command_cb;

class Console
{
public:
    static void InitConsole(USARTManager& usart);
    static Console& getInstance();
    void registerCommand(const char* command, const char* help, Cmdline_command_cb callback);
    void prompt();

private:
    struct CommandDescr {
        CommandDescr();
        CommandDescr* pnext;
        Cmdline_command_cb callback;
        const char* command;
        const char* help;
    };

    Console(USARTManager& usart);
    static void printHelp(const char* buffer);
    static int receiveStringCb(char* buffer, int bytesCount);
    void clearBuffer();
    void fixBackspaces();
    bool checkEmpty();


    CommandDescr *m_pFisrtCommand;
    char m_buffer[BUFFER_SIZE];
    USARTManager& m_usart;
    static Console* m_self;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_CONSOLE_HPP_ */
