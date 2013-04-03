/******************************************
	memory.h: Storage Subsystem
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#ifndef __HEADER_MEMORY

#define __HEADER_MEMORY

#include "common.h"
#include "mutex.h"

/*16-bit Addressing Space*/
#define RAM_SIZE 0x10000


/*defination of Memory*/
#ifdef __MEMORY_IMP
uchar ram[ RAM_SIZE ];
#else
extern uchar ram[ RAM_SIZE ];
#endif

/*display buffer*/
#define DISP_BUF 2496
#define DISP_W   160
#define DISP_H   80
#define TEXT_BUF 704
#define TEXT_W   20
#define TEXT_H   5
#define CUR_Y 998
#define CUR_X 999
#define CUR_S 997
#define KBD_STATUS 65530
#define KBD_CODE 65531
#define KBD_SCAN 65532

int   get_pixel( int x , int y );
uchar get_text ( int x , int y );
void  set_pixel( int x , int y , int c 	);
void  set_text ( int x , int y , uchar c);
void  cls      ( );
#endif
