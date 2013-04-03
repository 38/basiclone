/******************************************
	io_calls.c: Input/Output Calls
	by ghost<ghost89413@gmail.com>
		2010.04.23
*******************************************/
#ifndef __HEADER_MUTEX
#define __HEADER_MUTEX
#include "common.h"
#include "memory.h"
#include <SDL.h>
#ifdef __MUTEX_IMP
#define MUTEX_VARIABLE 
#else
#define MUTEX_VARIABLE extern
#endif
MUTEX_VARIABLE SDL_mutex* lock_memory;

/*read/write operation*/
uchar* memoryP( Address addr );
void   memoryV( );
void mutex_init();
#endif
