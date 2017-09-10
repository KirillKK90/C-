#include <stdio.h>
#include "serial.h"
#include "spi.h"

#define UART_USED 0
// second uart doesn't work ! (

static int uart_putchar(char c, FILE *stream)
{
	if (c == '\n') uart_putchar('\r', stream);
	while (serial_putc(UART_USED,c)==-1);
	return 0;
}

static int uart_getchar(FILE *stream)
{
	int r;
	do
		r=serial_getc(UART_USED);
	while ((r==-1)||(r=='\r'));
	return r;
}

FILE *console_init(void)
{
	serial_init(UART_USED, SERIALCLEN_8|SERIALPAR_N|SERIALSTOP_1, 19200, 65000);
	return fdevopen(uart_putchar, uart_getchar);
}

void getPrepared(void)
{	
	console_init();
	spi_init(0);
}
