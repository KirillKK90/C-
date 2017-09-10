#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/twi.h>
#include "types.h"
#include "spi.h"
#include "dataflash_drv.h"


void merge(u8, u8, u16 *);

void split(u8 *, u8 *, u16);

void id(void);

void readall(void);

void printPage(u16, u16, u8);

void write(u8, u16, u16);

u8 read(u16, u16);

void write_u16(u16, u16, u16);

u16 read_u16(u16, u16);

void contiguousWrite(u8 *, u16, u16, u16);

void contiguousRead(u8 *, u16, u8, u16);

void writeHugeFile(u8 *, u16, u8 );
