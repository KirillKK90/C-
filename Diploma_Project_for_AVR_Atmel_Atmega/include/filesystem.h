// Kirill K.K. naive filesystem DFFS - DataFlash File System ver 1.0

// filesystem for AT45DB161D

// single partition; equals to entire space on the flash

// allocation unit = page size = 512 / 528

// min file size = one page

// File Allocation Table: 255 - free page


#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/twi.h>
#include "types.h"
#include "spi.h"
#include "dataflash_drv.h"
#include "io.h"


#define MP 0 // Main Page - filecount, firstfreepage there are
#define FP 1 // File Page - FAT
#define FF 50 // First File Page (starting from what page are we putting files on disk ?)
#define SB 5 // - Start Byte| Should be > 3. When file created, initially we are starting to write from this byte (so, (528-SB) bytes of page taken by file itself)
//16

#define BP 2 // current Byte number Position on page, takes 2 adjacent bytes (BP < SB - should be !!! )


typedef struct _DFFS_ {
	u16		page_num;		// Number of pages 
    u16 	page_size;  	// Size of one page (in bytes) 
	u16		fatbase;		// File Allocation Table start page, it takes 8 pages 
	u16		database;		// Data start page 
	u16		last_written;  	// first free page num
	u8		file_count; 	// total number of files, u8 => no more than 256 files - reasonable constraint, hmmm...
	// members for current opened file;
	u32		fsize;			// File size (in bytes), > 1
	u16		start_page;		// File start page 
	u16		curr_page;		// File's last page - TODO use algorithm to calculate when opened
	u16		curr_byte;  	// File's first vacant byte, 

	// we need some more small vars
	// to be sure that we are not rewriting flash with the same number as it was
	u8		CH_curr_byte;	// Bool var to indicate if we have written more to file (CHanged curr_byte)
	u8		CH_file_count;
	u8		CH_last_written;
} DFFS;



///////////////////////PUBLIC//////////////////////////////////////////////

// 97..4096 data pages
//u8: 0..255
u8 formatFlash();  // format into DFFS file system


//func not to clean the whole chip, but delete only pages I am interested in 
u8 partialFlashFormat();


u8 fs_mount(DFFS *);	// Mount/Unmount a logical drive


u8 fs_createFile(u8 *);


u8 fs_openFile(u8);


u8 fs_write(u8 *, u16); // num - no more than 526 yet


u8 fs_writeFromStream(u8);


u8 fs_writeFromStreamMP(u8);


u8 fs_read(u8 *, u16);


u16 fs_getSize(void);


u8 fs_closeFile();


u8 fs_close(void);


/////////////////INTERNAL//////////////////////////


u8 checkState(u8);


u16 checkPageSize(u8);


u16 findNextPage(void);


void getDebugInfo(u16 *, u16 *);



/* 
DFFS ver 1.1
differs from 1.0 in the following ways:

- file reading bug fix (proper number of bytes to read)

- new #define: num of first file byte on page

- now we can use more than 255 bytes of each page (byte counter is u16 now)

- now we can use ENTIRE chip! )) (page counters are u16 now)

- now we can write to flash directly from keyboard! You type smth, send it over COM-port, the device will pick up and write to file


-------------------------------------------------------

DFFS ver 1.0
differs from 0.9 in the following ways:

- huge temp buffer from basic struct removed;

- some system constants are taken away to defines;

- file reading (multiple times) bug fix;

- system writes to flash reduced.

*/
