/******************************************
	display.h: Display Subsystem
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#ifndef __HEADER_DISPLAY

#define __HEADER_DISPLAY

#include <SDL.h>
#include <SDL_ttf.h>
#include "memory.h"

#ifdef  __DISPLAY_IMP
#define DISPLAY_VARIABLE
#else
#define DISPLAY_VARIABLE extern
#endif 

#define DM_GRAPH_MODE 0
#define DM_TEXT_MODE  1

#define PIXEL_SIZE 4

DISPLAY_VARIABLE SDL_Surface* screen;
DISPLAY_VARIABLE TTF_Font* font;

void display_init();



#endif
