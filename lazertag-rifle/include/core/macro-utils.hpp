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

#endif /* LAZERTAG_RIFLE_INCLUDE_CORE_MACRO_UTILS_HPP_ */
