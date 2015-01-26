/*
 * configuration.hpp
 *
 *  Created on: 10 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LOGIC_CONFIGURATION_HPP_
#define LOGIC_CONFIGURATION_HPP_

#include "core/singleton-macro.hpp"
#include "core/macro-utils.hpp"
#include "core/string-conversions.hpp"

#include <vector>
#include <map>
#include <functional>
#include <string>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>


/**
 *   Format of any serialized operation:
 *   ---------------------------------------------------------
 *   | data length |   Operation code   |        data        |
 *   ---------------------------------------------------------
 *   |   1 byte    |       2 bytes      | data length bytes  |
 *   ---------------------------------------------------------
 *
 *
 *   Operation code:
 *   ttxxxxxxxxxxxxxx
 *
 *   tt - operation type:
 *
 *   00 - command (means action like restart or other)
 *   01 - set parameter
 *   10 - reserved
 *   11 - reserved
 */

#define PARAMETER(NameSpace, Type, name)    Type name; \
                                            DefaultParameterAccessor<Type> name##Accessor {NameSpace::name, STRINGIFICATE(name), &name}


using OperationSize = uint8_t;
using OperationCode = uint16_t;

constexpr OperationCode OperationCodeMask =  (OperationCode) ~( (1<<15) | (1<<14) ); /// All bits =1 except two upper bits

constexpr OperationCode SetCommandOC(OperationCode operationCode) { return operationCode & OperationCodeMask; }
constexpr OperationCode SetParameterOC(OperationCode operationCode)
		{ return (operationCode & OperationCodeMask) | (1 << 14); }

inline bool __attribute__((always_inline)) getBit(OperationCode code, uint8_t bit)
		{ return (code & (1 << bit)) ? true : false; }

inline bool __attribute__((always_inline)) isSetParameterOC(uint16_t operationCode)
		{ return !getBit(operationCode, 15) && getBit(operationCode, 14); }

inline bool __attribute__((always_inline)) isCommand(uint16_t operationCode)
		{ return !getBit(operationCode, 15) && !getBit(operationCode, 14); }

/// Any kind of object that can read data from stream and write data to stream
class IOperationAccessor
{
public:
	virtual ~IOperationAccessor() {}
	virtual void deserialize(void* source, OperationSize size) = 0;
	virtual void serialize(void* destination) = 0;
	virtual void parseString(const char* str) = 0;
	virtual uint32_t getSize() = 0;
};

class ConfigsAggregator
{
public:
	static ConfigsAggregator& instance();

	void registerAccessor(OperationCode code, const char* textName, IOperationAccessor* accessor);

	/**
	 * Dispatch binary stream with encoded operations
	 * @param stream Binary stream
	 * @param size Stream size in bytes
	 * @return Count of unsupported operation found on stream
	 */
	uint32_t dispatchStream(uint8_t* stream, uint32_t size);

	void readFromFile(const char* filename);

private:
	std::map<OperationCode, IOperationAccessor*> m_accessorsByOpCode;
	std::map<std::string, IOperationAccessor*> m_accessorsByOpText;
	static ConfigsAggregator* m_configsAggregator;
	STATIC_DEINITIALIZER_IN_CLASS_DECLARATION;

};

/// Realization of IOperationAccessor for simple typed parameters
template<class Type>
class DefaultParameterAccessor : public IOperationAccessor
{
public:
	DefaultParameterAccessor(OperationCode code, const char* textName, Type* _parameter) :
		parameter(_parameter)
	{
		ConfigsAggregator::instance().registerAccessor(code, textName, this);
	}

	void deserialize(void* source, OperationSize)
	{
		memcpy(parameter, source, sizeof(Type));
	}

	void serialize(void* destination)
	{
		memcpy(destination, parameter, sizeof(Type));
	}

	void parseString(const char* str)
	{
		if (StringParser<Type>().isSupported())
			*parameter = StringParser<Type>().parse(str);
		else {
			/// @todo printf("Parsing of type not supported\n");
		}
	}

	inline uint32_t getSize() { return sizeof(Type); }

	Type* parameter;
};

using OperationFunction = std::function<void(void*, OperationSize)>;

class DefaultFunctionAccessor : public IOperationAccessor
{
public:
	DefaultFunctionAccessor(OperationCode code, const char* textName, OperationFunction func) :
		m_func(func)
	{
		ConfigsAggregator::instance().registerAccessor(code, textName, this);
	}

	void deserialize(void* source, OperationSize size)
	{
		m_func(source, size);
	}

	void serialize(void* destination)
	{
	}
	void parseString(const char* str)
	{
	}

	uint32_t getSize() { return 0; }

private:
	OperationFunction m_func;
};

/*
/// General case - string parsing not defined
template<class Type>
class DefaultParameterAccessor : public DefaultParameterAccessorBase<Type>
{
public:
	DefaultParameterAccessor(OperationCode code, const char* textName, Type* _parameter) :
		DefaultParameterAccessorBase<Type>(code, textName, _parameter)
	{}

	void parseString(const char* str)
	{}
};

template<>
class DefaultParameterAccessor<uint32_t> : public DefaultParameterAccessorBase<uint32_t>
{
public:
	DefaultParameterAccessor(OperationCode code, const char* textName, uint32_t* _parameter) :
		DefaultParameterAccessorBase<uint32_t>(code, textName, _parameter)
	{}

};*/

#endif /* LOGIC_CONFIGURATION_HPP_ */
