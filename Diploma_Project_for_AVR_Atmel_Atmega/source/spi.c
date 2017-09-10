#include <avr/io.h>
#include "spi.h"

void spi_init(u8 mode)
{
//atmega128-specific
	DDRB|=(1<<0)|(1<<1)|(1<<2);
	DDRB&=~(1<<3); //3
	PORTB|=(1<<0); //SS

	SPCR=(1<<SPE)|(1<<MSTR)|((mode&0x03)<<CPHA)|3; // f/128
	SPSR=0;
}

void spi_assert(u8 pin)
{
	if(!pin) PORTB&=~(1<<0); //SS
}

void spi_deassert(u8 pin)
{
	if(!pin) PORTB|=(1<<0); //SS
}

u8 spi_exchange(u8 byte)
{
	SPDR=byte;
	while (!(SPSR&(1<<SPIF)));
	return SPDR;
}

