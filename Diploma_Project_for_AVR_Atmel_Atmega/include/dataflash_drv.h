// Driver for DataFlash 45DB161B

#ifndef DATAFLASH_H
#define DATAFLASH_H
#include "types.h"

#define ERASE_PAGE 0
#define ERASE_BLOCK 1
#define ERASE_SECTOR 2
#define ERASE_CHIP 3

void dataflash_readarray(u8 line, u16 page, u16 byteaddr);

void dataflash_readpage(u8 line, u16 page, u16 byteaddr);

u8 dataflash_read();

void dataflash_close(u8 line);

void dataflash_readbuf(u8 line, u8 buf, u16 byteaddr);

void dataflash_writebuf(u8 line, u8 buf, u16 byteaddr);

void dataflash_write(u8 byte);

void dataflash_buftopage(u8 line, u8 buf, u16 page, u8 erase);

void dataflash_erase(u8 line, u8 gran, u16 page);

void dataflash_pagetobuf(u8 line, u8 buf, u16 page);

void dataflash_writepage(u8 line, u16 page, u8 buf, u16 byteaddr);

u8 dataflash_status(u8 line);

void dataflash_waitready(u8 line);

u8 dataflash_ident(u8 line, u8 n, u8* result);

#endif
