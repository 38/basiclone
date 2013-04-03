/******************************************
	io.c: Input/Output Subsystem
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#define __IO_IMP
#include "io.h"
#include "display.h"
#include <math.h>
SDL_Thread * event_thread;
SDL_Event   event;
int io_proc()
{
	while(1)
	{
		usleep(10000);
		while( SDL_PollEvent(&event) )
		{
			usleep(1000);
			
			if( event.type == SDL_KEYDOWN )
			{
				//MessageBoxA(0,"TEST","TEST",0);
				SDL_mutexP( lock_memory );
				ram[KBD_STATUS] = 1;
				ram[KBD_CODE] = event.key.keysym.unicode;
				ram[KBD_SCAN] = event.key.keysym.sym;
				//printf("%d\n", ram[KBD_SCAN]);
				if( ram[KBD_SCAN] == 17 ) ram[KBD_CODE] = 20;
				if( ram[KBD_SCAN] == 18 ) ram[KBD_CODE] = 21;
				if( ram[KBD_SCAN] == 20 ) ram[KBD_CODE] = 23;
				if( ram[KBD_SCAN] == 19 ) ram[KBD_CODE] = 22;
				ram[199] = ram[KBD_CODE]+128;
				SDL_mutexV(lock_memory);
			}
			if( event.type == SDL_KEYUP )
			{
				SDL_mutexP( lock_memory );
				ram[KBD_STATUS] = 0;
				ram[KBD_CODE] = event.key.keysym.unicode;
				ram[KBD_SCAN] = event.key.keysym.sym;
				ram[199] = 0;
				SDL_mutexV(lock_memory);			
			}
		}
	}
}
void io_init()
{
#ifndef __WIN32
	SDL_EnableUNICODE(1);
#endif
	event_thread = SDL_CreateThread( io_proc , NULL );	
}
