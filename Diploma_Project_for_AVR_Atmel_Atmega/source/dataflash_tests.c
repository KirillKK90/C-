#include "dataflash_tests.h"

void test1(void)
{
	printf_P(PSTR("Write to buffer 1/2. Write from buffer to page. Read that page. Assert if match initial number.\n"));
	u16 page = 9; // 1..4096
	u8 b = 97;
	u8 res = 0;
  
	dataflash_writebuf(0,0,0);
	dataflash_write(b);
	dataflash_close(0);
	dataflash_buftopage(0,0,page,1);
	dataflash_waitready(0);
	dataflash_readpage(0,page,0);
	res = dataflash_read();
	dataflash_close(0);
	if(res == b) printf_P(PSTR("PASSED.\n")); else printf_P(PSTR("FAILED.\n"));
}


void test2(void)
{
	printf_P(PSTR("Write to buffer 2/2. Write from buffer to page. Read that page. Assert if match initial number.\n"));
	u16 page = 9; // 1..4096
	u8 b = 97;
	u8 res = 0;
  
	dataflash_writebuf(0,1,0);
	dataflash_write(b);
	dataflash_close(0);
	dataflash_buftopage(0,1,page,1);
	dataflash_waitready(0);
	dataflash_readpage(0,page,0);
	res = dataflash_read();
	dataflash_close(0);
	if(res == b) printf_P(PSTR("PASSED.\n")); else printf_P(PSTR("FAILED.\n"));
}
