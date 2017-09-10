#include "filesystem.h"


static DFFS *fs;


//////////////////////////////PUBLIC/////////////////////////////////////////

// 97..4096 data pages
//u8: 0..255
u8 formatFlash() // format into DFFS file system
{
	if(!checkState(0)) return 1; //device busy
	dataflash_erase(0, 3, 0); //clean entire chip

    write(0, MP, 0); // number of files = 0
	write_u16(0, MP, 1); // last written page
//WHAT A F' ?! Don't you want to save the previous action ????!!!
// writing with erasing page
	return 0;
}


// func not to clean the whole chip, but delete only pages I am interested in 
u8 partialFlashFormat()
{
	if(!checkState(0)) return 1; // device busy
	dataflash_erase(0, 0, MP); 
	dataflash_erase(0, 0, FP); 
	for(u16 i = FF; i < FF + 15; ++i)
	{
		dataflash_erase(0, 0, i); 
	}

    write(0, MP, 0); // number of files = 0
	write_u16(0, MP, 1); // last written page

	return 0;
}


u8 fs_mount(DFFS* dffs)	// Mount/Unmount a logical drive
{ 
	if(!checkState(0)) return 1; //device busy
	
	dffs->page_num = 4096;
	dffs->page_size = checkPageSize(0);
	dffs->fatbase = 1;
	dffs->database = FF; // FF..4096 - pages for data
	// frequently varying numbers are written to disk
	dffs->file_count = read(MP, 0);
	dffs->last_written = read_u16(MP, 1);
	dffs->fsize = 0;

	dffs->CH_curr_byte = 0;
	dffs->CH_file_count = 0;
	dffs->CH_last_written = 0;
	
 	fs = dffs;    

	return 0;
}



u8 fs_createFile(u8* id)
{
	if(!checkState(0)) return 1; //device busy
	if(!fs) return 2; // file system not mounted

	dataflash_pagetobuf(0, 0, 1);
	dataflash_waitready(0);

	u16 first_page; // first page of the new file being created
	if(fs->last_written == 0) first_page = FF; else first_page = findNextPage();

	write_u16(first_page, 1, 2 * fs->file_count); // only first page of the file is written here!!

	write_u16(1, first_page, 0);  // first and last file's page yet
	write_u16(SB, first_page, BP);  // first vacant byte's number is SB

	*id = fs->file_count;
	fs->file_count++;
	fs->last_written = first_page;
	printf_P(PSTR("\n fs->last_written: %d \n"), fs->last_written);

	fs->CH_file_count = 1;
	fs->CH_last_written = 1;
	
	return 0;
}


u8 fs_openFile(u8 id)
{
	if(!checkState(0)) return 1; // device busy
	if(!fs) return 2; // file system not mounted
	if(fs->fsize) return 3; // only one file can be opened at a time!
	if(id >= fs->file_count) return 4; // no such file, id: 0..filecount

	fs->start_page = read_u16(FP, id * 2); // 1, id: TODO
	fs->curr_page = fs->start_page;
	fs->curr_byte = read_u16(fs->curr_page, BP); 
	fs->fsize = 1; // if 1, used just as a marker

	fs->CH_curr_byte = 0;

	return 0;
}


u8 fs_write(u8 *input, u16 num) // num - no more than ~500 yet
{
	if(!checkState(0)) return 1; // device busy
	if(!fs) return 2; // file system not mounted
	if(!fs->fsize) return 3; // no opened files 

	dataflash_pagetobuf(0, 0, fs->curr_page);
	dataflash_waitready(0);
	contiguousWrite(input, fs->curr_page, num, fs->curr_byte);

	fs->curr_byte += num;

	fs->CH_curr_byte = 1;
	// fs->CH_last_written = 1; - no! only if written more than 1 page

	return 0;
}

// if print = 0, then don't print anything to terminal
u8 fs_writeFromStream(u8 print) // num - no more than ~500 yet
{
	if(!checkState(0)) return 1; // device busy
	if(!fs) return 2; // file system not mounted
	if(!fs->fsize) return 3; // no opened files 

	dataflash_pagetobuf(0, 0, fs->curr_page);
	dataflash_waitready(0);

	dataflash_writebuf(0,0,fs->curr_byte);
	u16 k = 0;
	u8 i;
	if(print) printf_P(PSTR("\n Writing to chip in real-time. Type data: \n"));
    for(;;)
	{
		scanf("%c", &i);
		if(i == '~') break;
		if(k > 400) break;
		dataflash_write(i);
		if(print) printf_P(PSTR("%c"), i);
		if(fs->curr_byte + k > 527) break;
		++k;
	}
	dataflash_close(0);
	dataflash_buftopage(0,0,fs->curr_page,1);
	dataflash_waitready(0);

	
	fs->curr_byte += k;

	fs->CH_curr_byte = 1;
	// fs->CH_last_written = 1; - no! only if written more than 1 page

	if(print) printf_P(PSTR("\n"));

	return 0;
}



// if print = 0, then don't print anything to terminal
u8 fs_writeFromStreamMP(u8 print) // num - no more than ~500 yet
{
	if(!checkState(0)) return 1; // device busy
	if(!fs) return 2; // file system not mounted
	if(!fs->fsize) return 3; // no opened files 

	u8 i;
	u16 k = 0;
	u8 next_page = 0; // flag: 0 || 1
	u16 prev_page_tmp;

	dataflash_pagetobuf(0, 0, fs->curr_page);
	dataflash_waitready(0);

	do
	{
		if(next_page)
		{	
			prev_page_tmp = fs->curr_page;
			fs->curr_page = findNextPage();
			write_u16(fs->curr_page, prev_page_tmp, 0); // try to optimize number of writings to memory !!!
			write_u16(1, fs->curr_page, 0);
			//write_u16(SB, fs->curr_page, BP);
			fs->curr_byte = SB;

			// find next free page 
			// write service info
			// now ready to write user's info
		
		}

		dataflash_writebuf(0,0,fs->curr_byte);
;
		if(print) printf_P(PSTR("\n Writing to chip in real-time. Type data: \n"));

		next_page = 0;
		k = 0;
	    for(;;)
		{
			scanf("%c", &i);
			if(i == '~') break;
			//if(k > 400) break;
			dataflash_write(i);
			if(print) printf_P(PSTR("%c"), i);
			if(fs->curr_byte + k > 526) // number 527 is already written !!! 
			{
				next_page = 1;
				break;
			}
			++k;
		}
		dataflash_close(0);
		dataflash_buftopage(0,0,fs->curr_page,1);  // here we write to memory !!!
		dataflash_waitready(0);
	}
	while(next_page);

	
	fs->curr_byte += k; // if page is not last for file, then don't care about fs->curr_byte value

	fs->CH_curr_byte = 1;
	// fs->CH_last_written = 1; - no! only if written more than 1 page

	if(print) printf_P(PSTR("\n"));

	return 0;
}


// read opened file
// be sure output is relevant size !!!
// num - number of bytes to read from the start
u8 fs_read(u8 *output, u16 num) // num - no more than 526 yet, if num=0 - read entire file
{
	if(!checkState(0)) return 1; // device busy
	if(!fs) return 2; // file system not mounted
	if(!fs->fsize) return 3; // no opened files 

	u16 fileBytesOnPage = fs->curr_byte - SB;
    if(num == 0 || num > fileBytesOnPage) num = fileBytesOnPage; // if changed after opening, this num differs the one wrote yet to last page. After closing, will be overwritten to right.

//------------------------------------------------
	printf_P(PSTR("curr_byte: %d\n"), fs->curr_byte); 
	printf_P(PSTR("start_page: %d\n"), fs->start_page); 
//------------------------------------------------
	contiguousRead(output, fs->start_page, num, SB);
	
	return 0;
}


u16 fs_getSize()
{
	if(fs->fsize == 1 || fs->CH_curr_byte) 
	{
		if(fs->curr_page == fs->start_page) fs->fsize = fs->curr_byte - SB;
		//printf_P(PSTR("curr_byte getsize: %d\n"), fs->curr_byte); 
	}
	

	return fs->fsize;
}


// invocation of this func is mandatory !
u8 fs_closeFile()
{
	if(!checkState(0)) return 1; // device busy
	if(!fs) return 2; // file system not mounted
	if(!fs->fsize) return 3; // no file to close!

	fs->fsize = 0;
	fs->start_page = 0;
	if(fs->CH_curr_byte) write_u16(fs->curr_byte, fs->curr_page, BP); // remember first vacant byte; if !CH_curr_byte then curr_byte didn't change
	printf_P(PSTR("curr_page: %d \n"), fs->curr_page); 

	fs->curr_byte = 0;
	fs->curr_page = 0;

	return 0;
}


// WARNING
// always needs to be invoked after using DFFS,
// otherwise filesystem error will occur ! 
// and format will be essential (!!!) 
u8 fs_close(void)
{
	if(!checkState(0)) return 1; //device busy
	if(!fs) return 2; // file system not mounted

	if(fs->CH_file_count) write(fs->file_count, MP, 0);
	if(fs->CH_last_written) write_u16(fs->last_written, MP, 1);

	return 0;
}



////////////////////INTERNAL///////////////////////////////


u16 findNextPage()
{

/*	
// simplest algorithm yet!	
	u16 t, i = 1;

	do
	{
		t = read(fs->last_written + i, 0);
		++i;
	}
	while(t != 0 || t != 255);
	

	return fs->last_written + i - 1;
*/

	// NO algorithm yet! :-(
	return fs->last_written + 1;
}	


u8 checkState(u8 line)
{
	// 1 - not busy, 0 - busy state
	if((dataflash_status(line) & 128) > 0) return 1; else return 0;
}


u16 checkPageSize(u8 line)
{
	if((dataflash_status(line) & 1) > 0) return 512; else return 528;
}


void getDebugInfo(u16 *last_written, u16 *file_count)
{
	*last_written = fs->last_written;
	*file_count = fs->file_count;
}
