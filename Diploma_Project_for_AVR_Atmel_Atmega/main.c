#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "types.h"
#include "dataflash_tests.h"
#include "spi.h"
#include "filesystem.h"


void getPrepared(void);


 
int main(void) 
{
	getPrepared();

/*		
	printf("\n");
	for(u16 j = 4001; j < 4007; ++j)
	{
		dataflash_erase(0, 0, j); 
		printPage(j, 20, 0);
	}
	printf("\n");
*/

/*
	printf("\n");

	u8 read_res[300];
	u8 i, j = 0;
	u8 k = 0;

	for(;;) 
	{
		scanf("%c", &i);
		if(i == j && j == '~') break;
		read_res[k] = i;
		j = i;
		++k;
	}

	printf("\n Finished! \n)");



   	for(u8 i = 0; i < k; ++i)
	{
		printf_P(PSTR("%c"), read_res[i]);
	}
*/





	
	u8 buff[6];
	buff[0] = 3;
	buff[1] = 3;
	buff[2] = 3;
	buff[3] = 3;
	buff[4] = 3;
	buff[5] = 3;



	printf_P(PSTR("\n\n Hello! \n"));

	//formatFlash();
	if(partialFlashFormat()) printf_P(PSTR("Even formatting flash failed (( !\n"));  

	DFFS FileSystemObject;
	if(fs_mount(&FileSystemObject)) printf_P(PSTR("Failure!\n"));  

	u16 frp, flc;
	getDebugInfo(&frp, &flc);
	printf_P(PSTR("file count: %d, first free page num: %d\n"), flc, frp);

 	u8 fileNum = 0;
	if(fs_createFile(&fileNum)) printf_P(PSTR("Failure!\n"));
	
	if(fs_openFile(fileNum)) printf_P(PSTR("Failure!\n")); 
	//if(fs_write(buff, 6)) printf_P(PSTR("debug print: write failed\n"));
	fs_writeFromStream(1);
	if(fs_closeFile()) printf_P(PSTR("Failure!\n")); 

		
/*
	if(fs_openFile(fileNum)) printf_P(PSTR("Failure!\n")); 
	u16 fsz = fs_getSize();
	printf_P(PSTR("File size: %d \n"), fsz);
	

	u8 read_res[30];
	if(fs_read(read_res, 0)) printf_P(PSTR("Failure!\n"));
	if(fs_closeFile()) printf_P(PSTR("Failure!\n")); 


	printf_P(PSTR("Your file is: "));
	for(u8 i = 0; i < fsz; ++i)
	{
		printf_P(PSTR("%c"), read_res[i]);
	}
*/

	if(fs_close()) printf_P(PSTR("Failure!\n")); 

	printPage(50, 10, 0);
	printf_P(PSTR("\n Push queue. "));






	return 0;
}
