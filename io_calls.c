/******************************************
	io_calls.c: Input/Output Calls
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#define __IO_CALLS_IMP
#include "io_calls.h"
#include <math.h>
int inkey()
{
	while(1)
	{
		usleep(1000);
		memoryP(0);
		if( ram[KBD_STATUS] && ( ram[KBD_CODE] > 0 || ram[KBD_SCAN] == 17 
					                   || ram[KBD_SCAN] == 18
							   || ram[KBD_SCAN] == 20
							   || ram[KBD_SCAN] == 19))
		{
			ram[KBD_STATUS] = 0;
			memoryV();
			return ram[KBD_CODE];
		}
		memoryV();
	}
}

void scroll_up()
{
	uchar *a , *b;
	int i;
	a = ram + TEXT_BUF;
	b = ram + TEXT_BUF + TEXT_W;
	for( i = 0 ; i < (TEXT_H-1) * TEXT_W ; i++ , a++ , b++ )
	{
		*a = *b;
	}
	for( i = 0 ; i < TEXT_W ; i ++ , a++ )
		*a = ' ';
}

void put_char(char ch)
{
	int x,y;
	memoryP(0);
	x = ram[CUR_X];
	y = ram[CUR_Y];
	if( ch == 13 )
	{
		ram[CUR_X] = 0;
		ram[CUR_Y] ++;
		if( ram[CUR_Y] >= TEXT_H )
		{
			ram[CUR_Y] = TEXT_H - 1;
			y--;
			scroll_up();
	 	}
		memoryV();
		return;		
	} 
	ram[CUR_X] ++;
	if(ram[CUR_X] >= TEXT_W )
	{
		ram[CUR_X] = 0;
		ram[CUR_Y] ++;
		if( ram[CUR_Y] >= TEXT_H )
		{
			ram[CUR_Y] = TEXT_H - 1;
			y--;
			scroll_up();
		}
	}

	memoryV();
	set_text( x , y , ch);
}
void pixel( int x, int y , int color )
{
	set_pixel( x , y , color );
}
void line( int x , int y , int w , int h , int color )
{
	//printf("%d %d %d %d\n",x,y,w,h);
	if( w != 0 )
	{
		int xx = x;
		double yy = y;
		double k = h / (double)w;
		int sign = 1;
		if( w < 0 ) sign = -1;
		for(; xx - x <= w ; xx += sign , yy+= k )
			pixel( xx , (yy+.5) , color );
	}
	else
	{
		int yy = y;
		int sign = 1;
		if( h < 0 ) sign = -1;
		for( ; yy - y <= h ; yy += sign )
			pixel( x , yy , color );
	}
}
void rect( int x, int y , int w , int h , int fill , int color )
{
	if( w < 0 ) x += w , w = -w;
	if( h < 0 ) y += h , h = -h;
	if( fill )
	{
		int dx , dy;
		for( dx = 0 ; dx <= w ; dx ++ )
			for( dy = 0 ; dy <= h ; dy ++ )
				pixel( dx + x , dy + y , color );
	}
	else
	{
		line( x , y , w  , 0 , color);
		line( x , y , 0   , h  ,color);
		line( x+w  ,y , 0 , h  ,color);
		line( x, y + h  , w  , 0 ,color);
		
	} 
}
void ellipse( int x, int y , int a , int b , int fill , int color )
{
	double dx , dy , ang;
	for( ang=0 ; ang < 3.1415926 ; ang += .01 )
	{
		int xx , yy ;
		int ny ;
		int i;
		dx = a * cos(ang);
		dy = b * sin(ang);
		xx = round(x + dx);
		yy = round(y + dy);
		//nx = round(x + dx);
		ny = round(y - dy);
		if( fill )
			for( i = ny ; i <= yy ; i ++ )
				pixel( xx , i , color );
		else
		{
			pixel( xx , yy , color );
			pixel( xx , ny , color );
		}					
	}

}
int open_file( char* file , int mode , int len , int id )
{
	if( id > 255 || id < 0 )
		return ILLEGAL_QUANTITY;
	if( file_list[id].used )
		return FILE_NUMBER;
	file_list[id].mode = mode;
	file_list[id].buffer_length = len;
	file_list[id].buffer = NULL;
	
	if( mode == FILE_MODE_INPUT )
	{
		file_list[id].fp = fopen( file , "r" );
		if( file_list[id].fp == NULL )
			return FILE_NOT_EXIST;
	}
	else if ( mode == FILE_MODE_OUTPUT )
	{
		file_list[id].fp = fopen( file , "w" );
		if( file_list[id].fp == NULL )
			return FILE_OPEN;
	}
	else if( mode == FILE_MODE_APPEND )
	{
		file_list[id].fp = fopen( file , "a" );
		if( file_list[id].fp == NULL )
			return FILE_OPEN;
	}
	else if( mode == FILE_MODE_RANDOM )
	{
		file_list[id].fp = fopen( file , "rwb" );
		if( file_list[id].fp == NULL )
			return FILE_OPEN;
	}
	file_list[id].used = 1;
	return SUCESS;
}
int close_file( int id )
{
	if( id < 0 || id > 255 || !file_list[id].used )
		return FILE_NUMBER;
	File_Buffer* fb = file_list[id].buffer;
	while( fb )
	{
		File_Buffer* temp = fb->next;
		free(fb);
		fb = temp;
	}
	fclose( file_list[id].fp );
	file_list[id].used = 0;
	return SUCESS;
}
