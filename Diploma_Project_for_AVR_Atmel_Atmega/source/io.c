#include "io.h"


void merge(u8 a, u8 b, u16 *c)
{
	u16 t = a;
	t = t << 8;
	t |= b;
	*c = t;
}


void split(u8 *a, u8 *b, u16 c)
{ 
	*b = c;
	*a = c >> 8;
}

void id(void) 
{
	u8 id[4];
	dataflash_ident(0,4,id);
	printf_P(PSTR("Identification: %02X %02X %02X %02X\n"),id[0],id[1],id[2],id[3]);
	printf_P(PSTR("Status: %02X\n"),dataflash_status(0));
}

void readall(void) 
{
	u16 page;

	dataflash_readarray(0,0,0);
	for (page = 0; page < 2; page++) 
	{
	    printf_P(PSTR("%04X:  "), page);
		for(u8 i = 0; i < 528; i++) 
		{
			printf_P(PSTR(" %d"), dataflash_read());
		}
	}
	dataflash_close(0);
}


// format: 0 - print ints, 1 - print chars
// bytesToPrint: 0 - entire page
void printPage(u16 page, u16 bytesToPrint, u8 format) 
{
	if(bytesToPrint == 0) bytesToPrint = 528;
	dataflash_readpage(0,page,0);
	printf_P(PSTR("Page number: %d: "), page);
	if(format)
	{
		for(u16 i = 0; i < bytesToPrint; i++) 
		{
			printf_P(PSTR(" %c"),dataflash_read());
		}
	}
	else
	{
		for(u16 i = 0; i < bytesToPrint; i++) 
		{
			printf_P(PSTR(" %d"),dataflash_read());
		}
	}
	printf_P(PSTR("\n"));
	dataflash_close(0);
}


void write(u8 b, u16 page, u16 byteaddr)
{
// page 1..4096 
	dataflash_writebuf(0,0,byteaddr);
	dataflash_write(b);
	dataflash_close(0);
	dataflash_buftopage(0,0,page,1);
	dataflash_waitready(0);
}

u8 read(u16 page, u16 byteaddr)
{
    u8 res = 0;
    dataflash_readpage(0,page,byteaddr);
	res = dataflash_read();
	dataflash_close(0);
	
    return res;
}


// takes two adjacent bytes
void write_u16(u16 c, u16 page, u16 byteaddr)
{
	if(byteaddr == 527) return;

	u8 a, b;
	split(&a, &b, c);
	write(a, page, byteaddr);
	write(b, page, byteaddr + 1);
}



// reads two adjacent bytes
u16 read_u16(u16 page, u16 byteaddr)
{
	if(byteaddr == 527) return 0;
	u8 a, b;
	u16 c = 0;

	a = read(page, byteaddr);
	b = read(page, byteaddr + 1);
	merge(a, b, &c);
		
	return c;
}


void contiguousWrite(u8 *source, u16 page, u16 num, u16 byteaddr)
{
// page 1..4096 
	dataflash_writebuf(0,0,byteaddr);
    for(u16 i = 0; i < num; ++i)
	{
		dataflash_write(*(source + i));
	}
	dataflash_close(0);
	dataflash_buftopage(0,0,page,1);
	dataflash_waitready(0);
}


void contiguousRead(u8 *dest, u16 firstPage, u8 num, u16 byteaddr)
{	
	dataflash_readpage(0,firstPage,byteaddr);
	for(int i = 0; i < num; ++i)
	{
		*(dest + i) = dataflash_read();
	}

	dataflash_close(0);   
}


void writeHugeFile(u8 *source, u16 page, u8 num)
{
// page 1..4096 
	dataflash_writebuf(0,0,0);
    for(int i = 0; i < num; ++i)
	{
		dataflash_write(*(source + i));
	}
	for(int i = 0; i < num; ++i)
	{
		dataflash_write(*(source + i));
	}
	dataflash_close(0);
	dataflash_buftopage(0,0,page,1);
	dataflash_waitready(0);
}

