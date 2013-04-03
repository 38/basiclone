/******************************************
	io.h: Input/Output Subsystem
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#ifndef __HEADER_IO
#define __HEADER_IO
#include "common.h"
#include "memory.h"
#include <SDL.h>

#ifdef  __IO_IMP
#define IO_VARIABLE
#else
#define IO_VARIABLE extern
#endif 
void io_init();
#endif
