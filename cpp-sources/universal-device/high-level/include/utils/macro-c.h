/*
 * macro-c.h
 *
 *  Created on: 2 янв. 2016 г.
 *      Author: alexey
 */

#ifndef INCLUDE_UTILS_MACRO_C_H_
#define INCLUDE_UTILS_MACRO_C_H_

#define DECL_CLEAN(type, name)         type name; \
                                       memset(&name, 0, sizeof(type));


#endif /* INCLUDE_UTILS_MACRO_C_H_ */
