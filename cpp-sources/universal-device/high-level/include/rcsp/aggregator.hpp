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


#ifndef LOGIC_CONFIGURATION_HPP_
#define LOGIC_CONFIGURATION_HPP_


#include "rcsp/operation-codes-manipulation.hpp"
#include "rcsp/RCSP-base-types.hpp"
#include "utils/macro.hpp"
#include "core/result-code.hpp"
#include "core/string-utils.hpp"
#include "core/buffer.hpp"
#include <vector>
#include <map>
#include <functional>
#include <list>
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
 *   00 - function call request
 *   01 - set parameter
 *   10 - request parameter
 *   11 - reserved
 */

#define RESTORABLE      true
#define NOT_RESTORABLE  false

/**
 * Get type for any parameter to avoid type mismatch
 */
#define TYPE_OF(NameSpace, parameter)        NameSpace::parameter##Type

/** Create variable (in any class) and connect it to configs aggregator.
 *  ST = Simple Types, that can be parsed from string using StringParser<Type>::parse(str)
 */

#define PAR_ST(restorable, NameSpace, name, aggregator) \
    NameSpace::name##Type name; \
    StandartStrParseParameterAccessor<NameSpace::name##Type> name##Accessor \
      {NameSpace::name, STRINGIFICATE(name), &name, restorable, aggregator}

/** The same as PAR_ST, but string parsing using convertFromString function
 *  CL = CLass
 */
#define PAR_CL(restorable, NameSpace, name, aggregator) \
    NameSpace::name##Type name; \
    CustomStrParseParameterAccessor<NameSpace::name##Type> name##Accessor \
        {NameSpace::name, STRINGIFICATE(name), &name, restorable, aggregator}

/** The same as PAR_CL, but SS means 'Self serializing'
 *  This macro should be used with classes that has methods
 *  'serialize' and 'deseriaize'
 *
 *  Also no str parsing avaliable for typed this macro applied to
 */
#define PAR_CL_SS(restorable, NameSpace, name, aggregator) \
    NameSpace::name##Type name; \
    SelfSerializingParameterAccessor<NameSpace::name##Type> name##Accessor \
        {NameSpace::name, STRINGIFICATE(name), &name, restorable, aggregator}

/// Create function in class with 1 parameter and connect it to configs aggregator
#define FUNCTION_1P(NameSpace, ClassName, functionName, aggregator) \
    void functionName(NameSpace::functionName##Arg1Type argument); \
    DefaultFunctionAccessor<NameSpace::functionName##Arg1Type> \
        functionName##Accessor {NameSpace::functionName, STRINGIFICATE(functionName), \
            std::bind(&ClassName::functionName, this, std::placeholders::_1), aggregator}

/// Create function in class with no parameters and connect it to configs aggregator
#define FUNCTION_NP(NameSpace, ClassName, functionName, aggregator) \
    void functionName(); \
    DefaultFunctionAccessor<> functionName##Accessor {NameSpace::functionName, STRINGIFICATE(functionName), \
        std::bind(&ClassName::functionName, this), aggregator}

/// Any kind of object that can read data from stream and write data to stream
class IOperationAccessor
{
public:
	virtual ~IOperationAccessor() {}

	/// Get data from stream and set parameter, call function etc.
	virtual void deserialize(const void* source, OperationSize size) = 0;

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

template<typename T>
inline void serializeAndInc(uint8_t*& cursor, const T& data)
{
	memcpy(cursor, &data, sizeof(T));
	cursor += sizeof(T);
}

template<typename T>
inline void deserializeAndInc(const void*& cursor, T& target)
{
	memcpy(&target, cursor, sizeof(T));
	cursor += sizeof(T);
}

template<typename T>
inline void deserializeAndInc(const uint8_t*& cursor, T& target)
{
	memcpy(&target, cursor, sizeof(T));
	cursor += sizeof(T);
}

template<typename T>
inline void deserializeAndInc(void*& cursor, T& target)
{
	memcpy(&target, cursor, sizeof(T));
	reinterpret_cast<uint8_t*&>(cursor) += sizeof(T);
}

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
		NOT_READABLE,
		AGGREGATOR_IS_NULLPTR
	};

#pragma pack(push, 1)
	struct ChunkHeader
	{
		OperationSize size = 0;
		OperationCode code = 0;
	};
#pragma pack(pop)

	struct Operation
	{
		ChunkHeader header;
		const uint8_t* argument = nullptr;
	};

	constexpr static unsigned int minimalStreamSize = sizeof(OperationSize) + sizeof(OperationCode);

	using ResultType = DetailedResult<AddingResult>;

	RCSPAggregator() {}

	void registerAccessor(OperationCode code, const char* textName, IOperationAccessor* accessor, bool restorable = false);

	/**
	 * Dispatch binary stream with encoded operations
	 * @param stream Binary stream
	 * @param size Stream size in bytes
	 * @param answerStream stream to put answer for object requests
	 * @return Count of unsupported operation found on stream
	 */
	uint32_t dispatchStreamNew(const uint8_t* stream, uint32_t size, Buffer* answerStream = nullptr);
	uint32_t dispatchStreamNew(const Buffer& stream, Buffer* answerStream = nullptr);

	//uint32_t dispatchStream(uint8_t* stream, uint32_t size, RCSPMultiStream* answerStream = nullptr);

	const uint8_t* extractNextOperation(
			const uint8_t* stream,
			Operation& commad,
			uint16_t streamSize,
			bool& success
		);

	bool isStreamConsistent(const uint8_t* stream, uint32_t size);

	Result readIni(const char* filename);

	Result parseSring(const char* key, const char* value);

	/**
	 * Serialize variable or other object with accessor and put it to stream
	 * @param code         Code of variable/accessible object
	 * @param target       Array where to put
	 * @param pCustomValue Pointer to custom value of parameter that should be put into stream. Using
	 * this parameter means that serialization is trivial, so data of size = actual parameter size will be
	 * copied to stream from pCustomValue
	 * @return result of operation
	 *
	 * @todo: Make a template for custom value
	 */
	ResultType serializePush(
		OperationCode code,
		Buffer& target,
		const uint8_t* pCustomValue = nullptr
	);

	/**
	 * Put to stream request for variable
	 * @param code Code of variable
	 * @param target Array where to put
	 * @return result of operation
	 */
	static ResultType serializePull(
		OperationCode code,
		Buffer& target
	);

	/**
	 * Put to stream request for function call without parameters
	 * @param code Code of variable
	 * @param target Array where to put
	 * @return result of operation
	 */
	static ResultType serializeCall(
		OperationCode code,
		Buffer& target
	);

	/**
	 * Put to stream request for function call with parameter
	 * @param code Code of variable
	 * @param target Array where to put
	 * @param parameter Parameter value
	 * @return result of operation
	 */
	template<typename Type>
	static ResultType serializeCall(
		OperationCode code,
		Buffer& target,
		const Type& parameter
	)
	{
		ChunkHeader header;
		header.size = sizeof(parameter);
		header.code = RCSPCodeManipulator::makeCall(code);
		uint8_t *cursor = resizeAndWriteHeader(header, target);
		serializeAndInc(cursor, parameter);
		return DetailedResult<AddingResult>(OK);
	}

	bool doOperation(OperationCode code)
	{
		ChunkHeader h;
		h.code = code;
		return dispatchOperation(&h, nullptr, nullptr);
	}

	template <typename Type>
	bool doOperation(OperationCode code, Type& arg)
	{
		ChunkHeader h;
		h.code = code;
		h.size = sizeof(arg);
		return dispatchOperation(&h, reinterpret_cast<uint8_t*>(&arg), nullptr);
	}

	template <typename T>
	DetailedResult<T> getLocalObject(OperationCode code)
	{
		T result;
		OperationCode objReq = RCSPCodeManipulator::makePush(code);
		auto it = m_accessorsByOpCode.find(code);
		if (it == m_accessorsByOpCode.end())
			return DetailedResult<T>(result, "Opcode not found");

		if (!it->second->isReadable())
			return DetailedResult<T>(result, "Object not readable");

		if (it->second->getSize() != sizeof(result))
			return DetailedResult<T>(result, "Object type size != required");

		it->second->serialize(&result);
		return DetailedResult<T>(result);
	}

	using SplitBufferCallback = std::function<void(const uint8_t* begin, uint16_t size)>;

	/**
	 * Split buffer to blocks of size < maxSize. It is guaranteed that RCSP chunks will not be broken
	 * @param buf      Buffer object
	 * @param maxSize  Max chunk size
	 * @param callback Will be called for every block
	 * @return true if all is OK and false if stream is inconsistent
	 */
	static bool splitBuffer(const Buffer& buf, uint16_t maxSize, SplitBufferCallback callback);

	/**
	 * Verify if block contains correct RCSP commands stream. It does not check opcodes existance,
	 * only chunk and stream size validation
	 * @param buf      Buffer object
	 * @return true if all is OK and false if stream is inconsistent
	 */
	static bool verifyBuffer(const Buffer& buf);

	const std::list<OperationCode>& getRestorableOperationCodes() { return m_restorable; }

	RCSPAggregator(const RCSPAggregator&) = delete;
private:
	constexpr static OperationCode OperationCodeMask =  (OperationCode) ~( (1<<15) | (1<<14) ); ///< All bits =1 except two upper bits

	bool dispatchOperation(const ChunkHeader* header, const uint8_t* arg, Buffer* answerStream);
	void printWarningUnknownCode(OperationCode code);
	/**
	 * Resize buffer to contain package with given header and put header to buffer
	 * @return pointer to chunk payload if header.size != 0 and pointer to probably unallocated space otherwise
	 */
	static uint8_t* resizeAndWriteHeader(const ChunkHeader& header, Buffer& buffer);

	std::map<OperationCode, IOperationAccessor*> m_accessorsByOpCode;
	std::list<OperationCode> m_restorable;
	std::map<std::string, IOperationAccessor*> m_accessorsByOpText;
};

/// Realization of IOperationAccessor for function
template<typename... Type>
class DefaultFunctionAccessor;

/// Realization of IOperationAccessor for function without arguments
template<>
class DefaultFunctionAccessor<> : public IOperationAccessor
{
public:
	using FunctionAccessorCallback = std::function<void(void)>;

	DefaultFunctionAccessor(
	        OperationCode code,
	        const char* textName,
	        FunctionAccessorCallback callback,
			RCSPAggregator& aggregator) :
		callback(callback)
	{
		aggregator.registerAccessor(code, textName, this);
	}

	void deserialize(const void*, OperationSize size)
	{
		if (size != 0)
			printf("Warning: argument provided for function without args\n");
		callback();
	}

	bool isReadable() { return true; }
	bool isWritable() { return false; }
	void serialize(void*) { }
	void parseString(const char*) { }
	inline uint32_t getSize() { return 0; }

private:
	FunctionAccessorCallback callback;
};

/// Realization of IOperationAccessor for function with argument
template<typename ArgType>
class DefaultFunctionAccessor<ArgType> : public IOperationAccessor
{
public:
	using FunctionAccessorCallback = std::function<void(ArgType)>;

	DefaultFunctionAccessor(
	        OperationCode code,
	        const char* textName,
	        FunctionAccessorCallback callback,
			RCSPAggregator& aggregator) :
		callback(callback)
	{
		aggregator.registerAccessor(code, textName, this);
	}

	void deserialize(const void* source, OperationSize size)
	{
		if (sizeof(ArgType) != size)
		{
			printf("Error: Invalid function argument size\n");
			return;
		}
		ArgType argument;
		memcpy(&argument, source, sizeof(argument));
		callback(argument);
	}

	bool isReadable() { return true; }
	bool isWritable() { return false; }
	void serialize(void*) { }
	void parseString(const char*) { }
	inline uint32_t getSize() { return 0; }

private:
	FunctionAccessorCallback callback;
};

template<class Type>
class SelfSerializingParameterAccessor : public IOperationAccessor
{
public:
	SelfSerializingParameterAccessor(
	        OperationCode code,
	        const char* textName,
	        Type* _parameter,
	        bool restorable,
			RCSPAggregator& aggregator) :
		parameter(_parameter)
	{
		aggregator.registerAccessor(code, textName, this, restorable);
	}

	void deserialize(const void* source, OperationSize size)
	{
		parameter->deserialize(source, size);
	}

	void serialize(void* destination)
	{
		parameter->serialize(destination);
	}

	bool isReadable() { return true; }

	bool isWritable() { return true; }

	void parseString(const char*) { }

	inline uint32_t getSize() { return parameter->serializedSize(); }

protected:
	Type* parameter;
};

/// Realization of IOperationAccessor for any parameters
template<class Type>
class ParameterAccessorBase : public IOperationAccessor
{
public:
	ParameterAccessorBase(
	        OperationCode code,
	        const char* textName,
	        Type* _parameter,
	        bool restorable,
			RCSPAggregator& aggregator) :
		parameter(_parameter)
	{
		aggregator.registerAccessor(code, textName, this, restorable);
	}

	void deserialize(const void* source, OperationSize size)
	{
		if (size != sizeof(Type))
		{
			printf("Error: invalid size for parameter!\n");
			return;
		}
		memcpy(parameter, source, sizeof(Type));
	}

	void serialize(void* destination)
	{
		memcpy(destination, parameter, sizeof(Type));
	}

	bool isReadable() { return true; }

	bool isWritable() { return true; }

	inline uint32_t getSize() { return sizeof(Type); }

protected:
	Type* parameter;
};

/// Realization of IOperationAccessor for simple typed parameters
template<class Type>
class StandartStrParseParameterAccessor : public ParameterAccessorBase<Type>
{
public:
	StandartStrParseParameterAccessor(
	        OperationCode code,
	        const char* textName,
	        Type* _parameter,
	        bool restorable,
			RCSPAggregator& aggregator):
		ParameterAccessorBase<Type>(code, textName, _parameter, restorable, aggregator)
	{
	}

	void parseString(const char* str)
	{
		if (StringParser<Type>::isSupported())
		{
			*(ParameterAccessorBase<Type>::parameter) = StringParser<Type>::parse(str);
		} else {
			printf("Parsing of type not supported\n");
		}
	}
};

/// Realization of IOperationAccessor for parameters that have convertFromString(...) method
template<class Type>
class CustomStrParseParameterAccessor : public ParameterAccessorBase<Type>
{
public:
	CustomStrParseParameterAccessor(
	        OperationCode code,
	        const char* textName,
	        Type* _parameter,
	        bool restorable,
			RCSPAggregator& aggregator):
		ParameterAccessorBase<Type>(code, textName, _parameter, restorable, aggregator)
	{
	}

	void parseString(const char* str)
	{
		ParameterAccessorBase<Type>::parameter->convertFromString(str);
	}
};

#endif /* LOGIC_CONFIGURATION_HPP_ */
