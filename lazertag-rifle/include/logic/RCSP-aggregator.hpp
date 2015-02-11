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
#include "core/string-utils.hpp"
#include "core/result-code.hpp"

#include <vector>
#include <map>
#include <functional>
#include <string>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

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
 *   10 - request parameter
 *   11 - reserved
 */

/// Create variable in class and connect in to configs aggregator
#define PARAMETER(NameSpace, Type, name)               Type name; \
                                                       DefaultParameterAccessor<Type> name##Accessor {NameSpace::name, STRINGIFICATE(name), &name}

/// Create variable in class and connect in to configs aggregator with custom test name
#define PARAMETER_S(NameSpace, Type, name, textName)   Type name; \
                                                       DefaultParameterAccessor<Type> name##Accessor {NameSpace::name, textName, &name}

/// Create function in class and connect in to configs aggregator
#define FUNCION(NameSpace, ClassName, functionName)    void functionName(void* arguments, uint16_t size); \
                                                       DefaultFunctionAccessor functionName##Accessor {NameSpace::functionName, STRINGIFICATE(functionName), \
                                                           std::bind(&ClassName::functionName, this, std::placeholders::_1, std::placeholders::_2)}

using OperationSize = uint8_t;
using OperationCode = uint16_t;

constexpr OperationCode OperationCodeMask =  (OperationCode) ~( (1<<15) | (1<<14) ); /// All bits =1 except two upper bits

constexpr OperationCode SetCommandOC(OperationCode operationCode) { return operationCode & OperationCodeMask; }
constexpr OperationCode SetParameterOC(OperationCode operationCode)
		{ return (operationCode & OperationCodeMask) | (1 << 14); }
constexpr OperationCode SetParameterRequest(OperationCode operationCode)
		{ return (operationCode & OperationCodeMask) | (1 << 15); }

inline bool __attribute__((always_inline)) getBit(OperationCode code, uint8_t bit)
		{ return (code & (1 << bit)) ? true : false; }

inline bool __attribute__((always_inline)) isSetParameterOC(uint16_t operationCode)
		{ return !getBit(operationCode, 15) && getBit(operationCode, 14); }

inline bool __attribute__((always_inline)) isCommand(uint16_t operationCode)
		{ return !getBit(operationCode, 15) && !getBit(operationCode, 14); }

inline bool __attribute__((always_inline)) isParameterRequest(uint16_t operationCode)
		{ return getBit(operationCode, 15) && !getBit(operationCode, 14); }

/// Any kind of object that can read data from stream and write data to stream
class IOperationAccessor
{
public:
	virtual ~IOperationAccessor() {}
	/// Get data from stream and set parameter, call function etc.
	virtual void deserialize(void* source, OperationSize size) = 0;
	/// Put parameter, function return value etc. to stream
	virtual void serialize(void* destination) = 0;
	/// Get data from string
	virtual void parseString(const char* str) = 0;
	/// Check if serialize is callable method
	virtual bool isReadable() = 0;
	/// Check if deserialize is callable method
	virtual bool isWritable() = 0;
	/// Get data size in stream
	virtual uint32_t getSize() = 0;
};

class RCSPStream;
class RCSPMultiStream;

/**
 * Remote Control and Synchronization Protocol Aggregator
 */
class RCSPAggregator
{
public:
	enum AddingResult
	{
		OK = 0,
		INVALID_OPCODE,
		NOT_ENOUGH_SPACE,
		NOT_READABLE
	};

	using ResultType = DetailedResult<AddingResult>;

	static RCSPAggregator& instance();

	void registerAccessor(OperationCode code, const char* textName, IOperationAccessor* accessor);

	/**
	 * Dispatch binary stream with encoded operations
	 * @param stream Binary stream
	 * @param size Stream size in bytes
	 * @return Count of unsupported operation found on stream
	 */
	uint32_t dispatchStream(uint8_t* stream, uint32_t size, RCSPMultiStream* answerStream = nullptr);

	Result readIni(const char* filename);

	Result parseSring(const char* key, const char* value);

	/**
	 * Serialize variable or other object with accessor and put it to stream
	 * @param stream Array where to put
	 * @param code Code of variable/accessible object
	 * @param freeSpace Max available space in stream. Will be decremented after successful adding
	 * @return result of operation
	 */
	ResultType serializeObject(uint8_t* stream, OperationCode code, uint16_t freeSpace, uint16_t& actualSize);

	/**
	 * Put to stream request for variable
	 * @param stream Array where to put
	 * @param code Code of variable
	 * @param freeSpace Max available space in stream. Will be decremented after successful adding
	 * @return result of operation
	 */
	ResultType requestVariable(uint8_t* stream, OperationCode variableCode, uint16_t freeSpace, uint16_t& actualSize);

	/**
	 * Put to stream request for function call without parameters
	 * @param stream Array where to put
	 * @param variableCode Code of variable
	 * @param freeSpace Max available space in stream. Will be decremented after successful adding
	 * @return result of operation
	 */
	ResultType functionCall(uint8_t* stream, OperationCode functionCode, uint16_t freeSpace, uint16_t& actualSize);

	/**
	 * Put to stream request for function call with parameter
	 * @param stream Array where to put
	 * @param variableCode Code of variable
	 * @param freeSpace Max available space in stream. Will be decremented after successful adding
	 * @param parameter Parameter value
	 * @return result of operation
	 */
	template<typename Type>
	ResultType functionCall(uint8_t* stream, OperationCode functionCode, uint16_t freeSpace, uint16_t& actualSize, const Type& parameter)
	{
		/// @todo [refactor] Remove code duplication with functionCallwithout parameters
		actualSize = 0;
		uint16_t packageSize = sizeof(OperationSize) + sizeof(OperationCode)+sizeof(parameter);
		if (packageSize > freeSpace)
			return DetailedResult<AddingResult>(NOT_ENOUGH_SPACE, "Not enough space in stream");

		functionCode = SetCommandOC(functionCode);
		OperationSize size = sizeof(parameter);
		memcpy(stream, &size, sizeof(OperationSize));
		stream += sizeof(OperationSize);
		memcpy(stream, &functionCode, sizeof(OperationCode));
		stream += sizeof(OperationCode);
		memcpy(stream, &parameter, sizeof(parameter));
		actualSize = packageSize;
		return DetailedResult<AddingResult>(OK);
	}

private:
	std::map<OperationCode, IOperationAccessor*> m_accessorsByOpCode;
	std::map<std::string, IOperationAccessor*> m_accessorsByOpText;
	static RCSPAggregator* m_RCSPAggregator;
	STATIC_DEINITIALIZER_IN_CLASS_DECLARATION;

};

using FunctionAccessorCallback = std::function<void(void* /*arguments*/, uint16_t /*size*/)>;

/// Realization of IOperationAccessor for function
class DefaultFunctionAccessor : public IOperationAccessor
{
public:
	DefaultFunctionAccessor(OperationCode code, const char* textName, FunctionAccessorCallback _callback) :
		callback(_callback)
	{
		RCSPAggregator::instance().registerAccessor(code, textName, this);
	}

	void deserialize(void* source, OperationSize size)
	{
		callback(source, size);
	}

	bool isReadable()
	{
		return true;
	}

	bool isWritable()
	{
		return false;
	}

	void serialize(void*)
	{
	}

	void parseString(const char*)
	{
	}

	inline uint32_t getSize() { return 0; }


private:
	FunctionAccessorCallback callback;
};

/// Realization of IOperationAccessor for simple typed parameters
template<class Type>
class DefaultParameterAccessor : public IOperationAccessor
{
public:
	DefaultParameterAccessor(OperationCode code, const char* textName, Type* _parameter) :
		parameter(_parameter)
	{
		RCSPAggregator::instance().registerAccessor(code, textName, this);
	}

	void deserialize(void* source, OperationSize)
	{
		memcpy(parameter, source, sizeof(Type));
	}

	void serialize(void* destination)
	{
		memcpy(destination, parameter, sizeof(Type));
	}

	bool isReadable()
	{
		return true;
	}

	bool isWritable()
	{
		return true;
	}

	void parseString(const char* str)
	{
		if (StringParser<Type>().isSupported())
		{
			*parameter = StringParser<Type>().parse(str);
		} else {
			/// @todo printf("Parsing of type not supported\n");
			printf("Parsing of type not supported\n");
		}
	}

	inline uint32_t getSize() { return sizeof(Type); }

	Type* parameter;
};

#endif /* LOGIC_CONFIGURATION_HPP_ */
