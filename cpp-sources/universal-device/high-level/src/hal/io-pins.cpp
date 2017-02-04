#include "hal/io-pins.hpp"

#include <string.h>

uint8_t IIOPin::parsePort(const char* port)
{

	if (strcmp(port, "PORTA") == 0 || strcmp(port, "GPIOA") == 0 )
		return PORTA;

	if (strcmp(port, "PORTB")  == 0 || strcmp(port, "GPIOB") == 0 )
		return PORTB;

	if (strcmp(port, "PORTC")  == 0 || strcmp(port, "GPIOC") == 0 )
		return PORTC;

	if (strcmp(port, "PORTD")  == 0 || strcmp(port, "GPIOD") == 0 )
		return PORTD;

	return notAPort;
}
