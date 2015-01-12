/*
 * static-deinit.h
 *
 *  Created on: 09 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_CORE_STATIC_DEINIT_H_
#define LAZERTAG_RIFLE_INCLUDE_CORE_STATIC_DEINIT_H_


#define STATIC_DEINITIALIZER_IN_CLASS_DECLARATION   struct StaticDeinitializer \
													{ \
													public: \
														~StaticDeinitializer(); \
													}; \
													static StaticDeinitializer m_deinitializer

#define STATIC_DEINITIALIZER_IN_CPP_FILE(ClassName, classInstancePtr)   ClassName::StaticDeinitializer ClassName::m_deinitializer;\
																		ClassName::StaticDeinitializer::~StaticDeinitializer() \
																		{ \
																			if (ClassName::classInstancePtr) \
																				delete ClassName::classInstancePtr; \
																		}


#endif /* LAZERTAG_RIFLE_INCLUDE_CORE_STATIC_DEINIT_H_ */
