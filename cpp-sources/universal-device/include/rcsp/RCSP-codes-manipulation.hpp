/*
 * RCSP-codes-manipulation.hpp
 *
 *  Created on: 8 сент. 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_RCSP_RCSP_CODES_MANIPULATION_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_RCSP_RCSP_CODES_MANIPULATION_HPP_

#include "utils/macro.hpp"
#include "rcsp/RCSP-base-types.hpp"

#define PAR_CODE(type, parameterName, value)    constexpr OperationCode parameterName = RCSPCodeManipulator::makeSetObject(value); \
                                                constexpr const char parameterName##Text[] = STRINGIFICATE(variable); \
                                                using parameterName##Type = type;


#define FUNC_CODE_NP(function, value)           constexpr OperationCode function = RCSPCodeManipulator::makeCallRequest(value);

#define FUNC_CODE_1P(function, argType, value)  constexpr OperationCode function = RCSPCodeManipulator::makeCallRequest(value); \
                                                using function##Arg1Type = argType;

/**
 * Compile-time class used to manipulate with operation codes
 */
class RCSPCodeManipulator
{
public:
	constexpr static OperationCode clearOptionBits(OperationCode operationCode) { return operationCode & OperationCodeMask; }
	constexpr static OperationCode makeCallRequest(OperationCode operationCode) { return operationCode & OperationCodeMask; }
	constexpr static OperationCode makeSetObject(OperationCode operationCode)
			{ return (operationCode & OperationCodeMask) | (1 << 14); }
	constexpr static OperationCode makeObjectRequestOC(OperationCode operationCode)
			{ return (operationCode & OperationCodeMask) | (1 << 15); }

	constexpr static inline bool __attribute__((always_inline)) isSetObject(uint16_t operationCode)
				{ return !getBit(operationCode, 15) && getBit(operationCode, 14); }

	constexpr static inline bool __attribute__((always_inline)) isCallRequest(uint16_t operationCode)
			{ return !getBit(operationCode, 15) && !getBit(operationCode, 14); }

	constexpr static inline bool __attribute__((always_inline)) isObjectRequest(uint16_t operationCode)
			{ return getBit(operationCode, 15) && !getBit(operationCode, 14); }

private:

	constexpr static inline bool __attribute__((always_inline)) getBit(OperationCode code, uint8_t bit)
			{ return (code & (1 << bit)) ? true : false; }


	constexpr static OperationCode OperationCodeMask =  (OperationCode) ~( (1<<15) | (1<<14) ); ///< All bits =1 except two upper bits

};







#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_RCSP_RCSP_CODES_MANIPULATION_HPP_ */
