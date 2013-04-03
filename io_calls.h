/******************************************
	io_calls.h: Input/Output Calls
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
 #ifndef __HEADER_IO_CALLS
#define __HEADER_IO_CALLS
#include "common.h"
#include "mutex.h"
#include "memory.h"
#include "token.h"
#ifdef __IO_CALLS_IMP 
#define IO_CALLS_VARIABLE 
#else
#define IO_CALLS_VARIABLE extern
#endif

int  inkey();
void scroll_up();
void put_char( char ch );
void pixel( int x, int y , int color );
void rect( int x, int y , int w , int h , int fill , int color );
void ellipse( int x, int y , int a , int b , int fill , int color );
void line( int x , int y , int w , int h , int color );

#define FILE_MODE_INPUT   0
#define FILE_MODE_OUTPUT  1
#define FILE_MODE_RANDOM  2
#define FILE_MODE_APPEND  3
typedef struct __s_file_buffer
{ 
	Data* data;
	size_t length;
	struct __s_file_buffer *next;
} File_Buffer;
typedef struct
{ 
	int used;
	FILE* fp;
	int mode;
	size_t buffer_length;
	File_Buffer* buffer; 
} File;
IO_CALLS_VARIABLE File file_list[256];

int open_file ( char* file , int mode , int len , int id );
int close_file( int id );
#endif
