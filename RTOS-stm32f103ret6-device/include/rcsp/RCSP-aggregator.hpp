/*
 * configuration.hpp
 *
 *  Created on: 10 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LOGIC_CONFIGURATION_HPP_
#define LOGIC_CONFIGURATION_HPP_

#include "rcsp/RCSP-base-types.hpp"
#include "rcsp/RCSP-codes-manipulation.hpp"
#include "utils/macro.hpp"
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

#define PAR_ST(restorable, NameSpace, name)   NameSpace::name##Type name; \
                                              StandartStrParseParameterAccessor<NameSpace::name##Type> name##Accessor \
                                                  {NameSpace::name, STRINGIFICATE(name), &name, restorable}

/** The same as PAR_ST, but string parsing using convertFromString function
 *  CL = CLass
 */
#define PAR_CL(restorable, NameSpace, name)   NameSpace::name##Type name; \
                                              CustomStrParseParameterAccessor<NameSpace::name##Type> name##Accessor \
                                                  {NameSpace::name, STRINGIFICATE(name), &name, restorable}

/** The same as PAR_CL, but SS means 'Self serializing'
 *  This macro should be used with classes that has methods
 *  'serialize' and 'deseriaize'
 *
 *  Also no str parsing avaliable for typed this macro applied to
 */
#define PAR_CL_SS(restorable, NameSpace, name) NameSpace::name##Type name; \
                                               SelfSerializingParameterAccessor<NameSpace::name##Type> name##Accessor \
                                                   {NameSpace::name, STRINGIFICATE(name), &name, restorable}


/*
/// Create variable in class and connect in to configs aggregator with custom test name
#define PARAMETER_S(NameSpace, Type, name, textName)   Type name; \
                                                       DefaultParameterAccessor<Type> name##Accessor {NameSpace::name, textName, &name}
*/
/// Create function in class with 1 parameter and connect it to configs aggregator
#define FUNCION_1P(NameSpace, ClassName, functionName)    void functionName(NameSpace::functionName##Arg1Type argument); \
                                                          DefaultFunctionAccessor<NameSpace::functionName##Arg1Type> \
                                                              functionName##Accessor {NameSpace::functionName, STRINGIFICATE(functionName), \
                                                                  std::bind(&ClassName::functionName, this, std::placeholders::_1)}

/// Create function in class with no parameters and connect it to configs aggregator
#define FUNCION_NP(NameSpace, ClassName, functionName)     void functionName(); \
                                                           DefaultFunctionAccessor<> functionName##Accessor {NameSpace::functionName, STRINGIFICATE(functionName), \
                                                           std::bind(&ClassName::functionName, this)}




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

	struct Operation
	{
		OperationCode code;
		OperationSize argumentSize;
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
	uint32_t dispatchStream(uint8_t* stream, uint32_t size, RCSPMultiStream* answerStream = nullptr);

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
	ResultType serializeObjectRequest(uint8_t* stream, OperationCode variableCode, uint16_t freeSpace, uint16_t& actualSize);

	/**
	 * Put to stream request for function call without parameters
	 * @param stream Array where to put
	 * @param variableCode Code of variable
	 * @param freeSpace Max available space in stream. Will be decremented after successful adding
	 * @return result of operation
	 */
	ResultType serializeCallRequest(uint8_t* stream, OperationCode functionCode, uint16_t freeSpace, uint16_t& actualSize);

	/**
	 * Put to stream request for function call with parameter
	 * @param stream Array where to put
	 * @param variableCode Code of variable
	 * @param freeSpace Max available space in stream. Will be decremented after successful adding
	 * @param parameter Parameter value
	 * @return result of operation
	 */
	template<typename Type>
	ResultType serializeCallRequest(uint8_t* stream, OperationCode functionCode, uint16_t freeSpace, uint16_t& actualSize, const Type& parameter)
	{
		/// @todo [refactor] Remove code duplication with functionCallwithout parameters
		actualSize = 0;
		uint16_t packageSize = sizeof(OperationSize) + sizeof(OperationCode)+sizeof(parameter);
		if (packageSize > freeSpace)
			return DetailedResult<AddingResult>(NOT_ENOUGH_SPACE, "Not enough space in stream");

		functionCode = RCSPCodeManipulator::makeCallRequest(functionCode);
		OperationSize size = sizeof(parameter);
		memcpy(stream, &size, sizeof(OperationSize));
		stream += sizeof(OperationSize);
		memcpy(stream, &functionCode, sizeof(OperationCode));
		stream += sizeof(OperationCode);
		memcpy(stream, &parameter, sizeof(parameter));
		actualSize = packageSize;
		return DetailedResult<AddingResult>(OK);
	}

	bool doOperation(OperationCode code)
	{
		OperationSize size = 0;
		return dispatchOperation(&size, &code, nullptr);
	}

	template <typename Type>
	bool doOperation(OperationCode code, Type& arg)
	{
		OperationSize size = sizeof(arg);
		return dispatchOperation(&size, &code, reinterpret_cast<uint8_t*>(&arg));
	}

	template <typename T>
	DetailedResult<T> getLocalObject(OperationCode code)
	{
		T result;
		OperationCode objReq = RCSPCodeManipulator::makeSetObject(code);
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

	SIGLETON_IN_CLASS(RCSPAggregator)
private:
	constexpr static OperationCode OperationCodeMask =  (OperationCode) ~( (1<<15) | (1<<14) ); ///< All bits =1 except two upper bits

	bool dispatchOperation(OperationSize* size, OperationCode* code, uint8_t* arg, RCSPMultiStream* answerStream = nullptr);
	void printWarningUnknownCode(OperationCode code);

	std::map<OperationCode, IOperationAccessor*> m_accessorsByOpCode;
	std::map<std::string, IOperationAccessor*> m_accessorsByOpText;
	static RCSPAggregator* m_RCSPAggregator;


};

template<typename T>
inline void serializeAndInc(void*& cursor, const T& data)
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
	cursor += sizeof(T);
}

/// Realization of IOperationAccessor for function
template<typename... Type>
class DefaultFunctionAccessor;

/// Realization of IOperationAccessor for function without arguments
template<>
class DefaultFunctionAccessor<> : public IOperationAccessor
{
public:
	using FunctionAccessorCallback = std::function<void(void)>;

	DefaultFunctionAccessor(OperationCode code, const char* textName, FunctionAccessorCallback _callback) :
		callback(_callback)
	{
		RCSPAggregator::instance().registerAccessor(code, textName, this);
	}

	void deserialize(void*, OperationSize size)
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

	DefaultFunctionAccessor(OperationCode code, const char* textName, FunctionAccessorCallback _callback) :
		callback(_callback)
	{
		RCSPAggregator::instance().registerAccessor(code, textName, this);
	}

	void deserialize(void* source, OperationSize size)
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
	SelfSerializingParameterAccessor(OperationCode code, const char* textName, Type* _parameter, bool restorable = false) :
		parameter(_parameter)
	{
		RCSPAggregator::instance().registerAccessor(code, textName, this, restorable);
	}

	void deserialize(void* source, OperationSize size)
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
	ParameterAccessorBase(OperationCode code, const char* textName, Type* _parameter, bool restorable = false) :
		parameter(_parameter)
	{
		RCSPAggregator::instance().registerAccessor(code, textName, this, restorable);
	}

	void deserialize(void* source, OperationSize size)
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
	StandartStrParseParameterAccessor(OperationCode code, const char* textName, Type* _parameter, bool restorable = false):
		ParameterAccessorBase<Type>(code, textName, _parameter, restorable)
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
	CustomStrParseParameterAccessor(OperationCode code, const char* textName, Type* _parameter, bool restorable = false):
		ParameterAccessorBase<Type>(code, textName, _parameter, restorable)
	{
	}

	void parseString(const char* str)
	{
		ParameterAccessorBase<Type>::parameter->convertFromString(str);
	}
};

#endif /* LOGIC_CONFIGURATION_HPP_ */
