/******************************************
	init.c: Initialize all subsystems
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#define INIT_VARIABLE
#include "init.h"

void init()
{
	mutex_init();
	display_init();
	io_init();
	builtin_init();
	token_init();
}
