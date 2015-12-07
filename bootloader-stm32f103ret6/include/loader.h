/*
 * loader.h
 *
 *  Created on: 7 дек. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_LOADER_H_
#define INCLUDE_LOADER_H_

#ifndef FUNC_PREFIX
	#define FUNC_PREFIX(a)    bootloader_##a
#endif


#ifdef __cplusplus
extern "C" {
#endif

int FUNC_PREFIX(testFuncLib) ();

unsigned char* FUNC_PREFIX(testFuncArray) ();
void FUNC_PREFIX(loaderMain) ();

const char* FUNC_PREFIX(getBootloaderVersion) ();

#ifdef __cplusplus
}
#endif



#endif /* INCLUDE_LOADER_H_ */
