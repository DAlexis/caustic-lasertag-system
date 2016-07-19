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
