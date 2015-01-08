/*
 * console.hpp
 *
 *  Created on: 24 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_DEV_CONSOLE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DEV_CONSOLE_HPP_

#include "hal/usart.hpp"
#include <functional>
#include <list>

#define CONSOLE_INPUT_BUFFER_SIZE	100

using ConsoleCallback = std::function<void(const char*)>;

class Console
{
public:

    void init(uint8_t port);
    void registerCommand(const char* command, const char* help, ConsoleCallback callback);
    void prompt();
    void interrogate();

    static Console& instance();
private:

    struct CommandDescr {
        CommandDescr();
        CommandDescr(const ConsoleCallback& _callback, const char* _command, const char* _help) :
        	callback(_callback),
			command(_command),
			help(_help)
        {}

        ConsoleCallback callback;
        const char* command;
        const char* help;
    };

    struct StaticDeinitializer
    { // Yes, this is for Chuck Norris
    public:
    	~StaticDeinitializer();
    };

    Console();

    void printHelp(const char* buffer);
    int receiveStringCb(char* buffer, int bytesCount);
    void clearBuffer();
    void fixBackspaces();
    bool checkEmpty();

    std::list<CommandDescr> m_commands;
    char m_buffer[CONSOLE_INPUT_BUFFER_SIZE];
    char* m_argument = nullptr;
    IUSARTManager* m_pUsart;
    unsigned int m_commandsCount;

    ConsoleCallback m_toCall = nullptr;

    static Console* m_console;
    static StaticDeinitializer m_deinitializer;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_CONSOLE_HPP_ */
