/******************************************
	init.h: Initialize all subsystems
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#ifndef __HEADER_INIT
#define __HEADER_INIT
#include "common.h"


#ifdef  __INIT_IMP
#define INIT_VARIABLE
#include "token.h"
#include "buildin.h"
#include "expression.h"
#include "io.h"
#include "display.h"
#include "memory.h"
#include "mutex.h"
#else
#define INIT_VARIABLE extern
#endif 

void init();

#endif
