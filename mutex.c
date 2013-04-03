/******************************************
	mutex.c: Memory access mutex
	by ghost<ghost89413@gmail.com>
		2010.04.23
*******************************************/
#define __MUTEX_IMP
#include "mutex.h"
uchar*   memoryP( Address addr )
{
	if( SDL_mutexP(lock_memory) == -1 )
		return NULL;
	else
		return ram + addr;
}
void	memoryV()
{
	SDL_mutexV(lock_memory);
}
void mutex_init()
{
	if( ( lock_memory = SDL_CreateMutex() ) == NULL )
	{
		perror( "SDL_CreateMutex Error ");
		exit(1);
	}
}
