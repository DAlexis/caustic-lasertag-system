/*
 * write-adaptor.h
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_WRITE_ADAPTOR_H_
#define LAZERTAG_RIFLE_INCLUDE_WRITE_ADAPTOR_H_


#ifdef __cplusplus
 extern "C" {
#endif

void usart1Write(const char* ptr, int len);

#ifdef __cplusplus
 }
#endif

#endif /* LAZERTAG_RIFLE_INCLUDE_WRITE_ADAPTOR_H_ */
