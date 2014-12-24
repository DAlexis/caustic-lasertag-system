/*
 * usart-write-impl.cpp
 *
 *  Created on: 24 дек. 2014 г.
 *      Author: alexey
 */

#include "hal/usart-write-impl.h"
#include "hal/usart.hpp"

IUSARTManager *usart1 = nullptr;

void usartWriteImpl(const char* ptr, int len)
{
	if (!usart1)
	{
		usart1 = USARTS->getUSART(0);
		usart1->init(921600);
	}
	usart1->syncWrite(ptr, len);
}
