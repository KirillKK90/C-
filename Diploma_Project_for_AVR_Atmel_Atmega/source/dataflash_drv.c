// Driver for DataFlash 45DB161B

#include <avr/io.h>
#include "spi.h"
#include "dataflash_drv.h"

#define READARRAY_CMD		0xE8
#define READPAGEBYTE_CMD	0xD2
#define READBUFBYTE_1_CMD	0xD1
#define READBUFBYTE_2_CMD	0xD3
#define WRITEBUFBYTE_1_CMD	0x84
#define WRITEBUFBYTE_2_CMD	0x87
#define BUFTOPAGE_1_CMD		0x88
#define BUFTOPAGE_2_CMD		0x89
#define BUFTOPAGE_1_E_CMD	0x83
#define BUFTOPAGE_2_E_CMD	0x86
#define ERASEPAGE_CMD		0x81
#define ERASEBLOCK_CMD		0x50
#define ERASESECTOR_CMD		0x7C
#define ERASECHIP_1_CMD		0xC7
#define ERASECHIP_2_CMD		0x94
#define ERASECHIP_3_CMD		0x80
#define ERASECHIP_4_CMD		0x9A
#define PAGETOBUF_1_CMD		0x53
#define PAGETOBUF_2_CMD		0x55
#define WRITEPAGEBYTE_1_CMD	0x82
#define WRITEPAGEBYTE_2_CMD	0x85
#define READSTATUS_CMD		0xD7
#define IDENT_CMD		0x9F



#define ADDR_MSB(a) ((a)>>6)
#define ADDR_MID(a,b) ((((a)&0x3f)<<2)|(((b)>>8)&0x03))
#define ADDR_LSB(b) ((b)&0xff)

void dataflash_readarray(u8 line, u16 page, u16 byteaddr)
{
	spi_assert(line);
	spi_exchange(READARRAY_CMD);
	spi_exchange(ADDR_MSB(page));
	spi_exchange(ADDR_MID(page,byteaddr));
	spi_exchange(ADDR_LSB(byteaddr));
	spi_exchange(0);
	spi_exchange(0);
	spi_exchange(0);
	spi_exchange(0);
}

void dataflash_readpage(u8 line, u16 page, u16 byteaddr)
{
	spi_assert(line);
	spi_exchange(READPAGEBYTE_CMD);
	spi_exchange(ADDR_MSB(page));
	spi_exchange(ADDR_MID(page,byteaddr));
	spi_exchange(ADDR_LSB(byteaddr));
	spi_exchange(0);
	spi_exchange(0);
	spi_exchange(0);
	spi_exchange(0);
}

u8 dataflash_read()
{
	return spi_exchange(0);
}

void dataflash_close(u8 line)
{
	spi_deassert(line);
}

void dataflash_readbuf(u8 line, u8 buf, u16 byteaddr)
{
	spi_assert(line);
	spi_exchange((buf)?READBUFBYTE_2_CMD:READBUFBYTE_1_CMD);
	spi_exchange(0);
	spi_exchange(ADDR_MID(0,byteaddr));
	spi_exchange(ADDR_LSB(byteaddr));
}

void dataflash_writebuf(u8 line, u8 buf, u16 byteaddr)
{
	spi_assert(line);
	spi_exchange((buf)?WRITEBUFBYTE_2_CMD:WRITEBUFBYTE_1_CMD);
	spi_exchange(0);
	spi_exchange(ADDR_MID(0,byteaddr));
	spi_exchange(ADDR_LSB(byteaddr));
}

void dataflash_write(u8 byte)
{
	spi_exchange(byte);
}

void dataflash_buftopage(u8 line, u8 buf, u16 page, u8 erase)
{
	spi_assert(line);
	spi_exchange((erase)?((buf)?BUFTOPAGE_2_E_CMD:BUFTOPAGE_1_E_CMD):((buf)?BUFTOPAGE_2_CMD:BUFTOPAGE_1_CMD));
	spi_exchange(ADDR_MSB(page));
	spi_exchange(ADDR_MID(page,0));
	spi_exchange(0);
	spi_deassert(line);
}

void dataflash_erase(u8 line, u8 gran, u16 page)
{
	if (gran>ERASE_CHIP) return;
	spi_assert(line);
	if (gran==ERASE_CHIP) {
		spi_exchange(ERASECHIP_1_CMD);
		spi_exchange(ERASECHIP_2_CMD);
		spi_exchange(ERASECHIP_3_CMD);
		spi_exchange(ERASECHIP_4_CMD);
	} else {
		switch(gran) {
		case ERASE_PAGE:
			spi_exchange(ERASEPAGE_CMD);
			break;
		case ERASE_BLOCK:
			spi_exchange(ERASEBLOCK_CMD);
			break;
		case ERASE_SECTOR:
			spi_exchange(ERASESECTOR_CMD);
			break;
		}
		spi_exchange(ADDR_MSB(page));
		spi_exchange(ADDR_MID(page,0));
		spi_exchange(0);
	}
	spi_deassert(line);
}

void dataflash_pagetobuf(u8 line, u8 buf, u16 page)
{
	spi_assert(line);
	spi_exchange((buf)?PAGETOBUF_2_CMD:PAGETOBUF_1_CMD);
	spi_exchange(ADDR_MSB(page));
	spi_exchange(ADDR_MID(page,0));
	spi_exchange(0);
	spi_deassert(line);
}

void dataflash_writepage(u8 line, u16 page, u8 buf, u16 byteaddr)
{
	spi_assert(line);
	spi_exchange((buf)?WRITEPAGEBYTE_2_CMD:WRITEPAGEBYTE_1_CMD);
	spi_exchange(ADDR_MSB(page));
	spi_exchange(ADDR_MID(page,byteaddr));
	spi_exchange(ADDR_LSB(byteaddr));
}

u8 dataflash_status(u8 line)
{
	u8 r;
	spi_assert(line);
	spi_exchange(READSTATUS_CMD);
	r=spi_exchange(0);
	spi_deassert(line);
	return r;
}

void dataflash_waitready(u8 line)
{
	spi_assert(line);
	spi_exchange(READSTATUS_CMD);
	while (!(spi_exchange(0)&0x80));
	spi_deassert(line);
}

u8 dataflash_ident(u8 line, u8 n, u8* result)
{
	u8 i,len=0;
	spi_assert(line);
	spi_exchange(IDENT_CMD);
	for(i=0;i<4;i++) *result++=spi_exchange(0);
	len=*(result-1);
	if (len>n) len=n;
	for(i=0;i<len;i++) *result++=spi_exchange(0);
	spi_deassert(line);
	return len;
}

