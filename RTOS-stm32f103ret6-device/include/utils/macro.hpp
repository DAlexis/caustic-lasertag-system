/*
 * macro-utils.hpp
 *
 *  Created on: 11 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_CORE_MACRO_UTILS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_CORE_MACRO_UTILS_HPP_

#define STRINGIFICATE(a)    _STRINGIFICATE(a)
#define _STRINGIFICATE(a)   #a

#define ARRAY_SIZE(a)       (sizeof(a) / sizeof(a[0]))

#define SINGLETON_IN_CPP(ClassName)		ClassName& ClassName::instance() \
										{ \
												static ClassName singletonInstance; \
												return singletonInstance; \
										}


#define SIGLETON_IN_CLASS(ClassName)	static ClassName& instance(); \
										ClassName(const ClassName&) = delete;

#define ADD_BITS(where, what)		(where) |= (what)
#define REMOVE_BITS(where, what)	(where) &= ~(what)

#define UNUSED_ARG(x)		( (void) x )

#endif /* LAZERTAG_RIFLE_INCLUDE_CORE_MACRO_UTILS_HPP_ */
