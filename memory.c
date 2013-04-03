/******************************************
	memory.c: Storage Subsystem
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#define __MEMORY_IMP
#include "memory.h"
int   get_pixel( int x , int y )
{
	Address addr 	= DISP_BUF + ( y * DISP_W + x ) / 8;
	int	offset	= x & 7;
	int	ret	= ( ( *memoryP(addr) ) & ( 1<<offset ) ) != 0;
	memoryV();
	return ret;
}

uchar get_text ( int x , int y )
{
	Address addr 	= TEXT_BUF + ( y * TEXT_W + x );
	uchar ret = *memoryP(addr);
	memoryV();
	return ret;
}
void  set_pixel( int x , int y , int c 	)
{
	Address addr 	= DISP_BUF + ( y * DISP_W + x ) / 8;
	int	offset	= x & 7;
	memoryP( addr );
	if(c == 1)
		ram[addr] |= 1<<offset;
	else if( c == 0 )
		ram[addr] &= ~(1<<offset);
	else if( c == 2 )
		ram[addr] ^= (1<<offset);
	else if( c == 3 )
		ram[addr] |= (1<<offset);
	else if( c == 4 )
		ram[addr] &= 0xff;
	else if( c == 4 )
		ram[addr] ^= 0xff;
	memoryV( );
}
void  set_text ( int x , int y , uchar c)
{
	Address addr 	= TEXT_BUF + ( y * TEXT_W + x ) ;
	memoryP(addr);
	ram[addr] = c;
	memoryV();
}
void  cls()
{
	memoryP(0);
	memset( ram + TEXT_BUF , 0 , TEXT_H * TEXT_W );
	memset( ram + DISP_BUF , 0 , DISP_H * DISP_W / 8 );
	memoryV();
}

/*Test Code*/
#ifdef MEMORY_TEST
int main()
{
	return 0;
}
#endif

