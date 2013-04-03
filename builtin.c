/******************************************
	builtin.c: Built-in functions
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#include "builtin.h"
#include <math.h>
#include "memory.h"
#include <time.h>
#include "io_calls.h"



#define r_FUNC_r(NAME) Data* __builtin__##NAME( Subscript* arg )\
{\
	if( arg->dimensions != 1 )\
		return Raise(ILLEGAL_QUANTITY); \
	if( arg->dimension[0]->type != TYPE_REAL )\
	   	return Raise(TYPE_MISMATCH);\
	return Data_new( TYPE_REAL , (Storage)(double) NAME ( arg->dimension[0]->storage.Real ) , 1);\
}

#define r_FUNC_s(NAME) Data* __builtin__##NAME( Subscript* arg )\
{\
	if( arg->dimensions != 1 )\
		return Raise(ILLEGAL_QUANTITY); \
	else\
		if( arg->dimension[0]->type != TYPE_STRING )\
			return Raise(TYPE_MISMATCH);\
	return Data_new( TYPE_REAL , (Storage)(double)NAME ( arg->dimension[0]->storage.String) , 1 );\
}

#define s_FUNC_r(NAME) Data* __builtin__##NAME( Subscript* arg )\
{\
	if( arg->dimensions != 1 )\
		return Raise(ILLEGAL_QUANTITY); \
	if( arg->dimension[0]->type != TYPE_REAL )\
	   	return Raise(TYPE_MISMATCH);\
	return Data_new( TYPE_STRING , (Storage)NAME ( arg->dimension[0]->storage.Real ) , 1 );\
}

#define s_FUNC_sr(NAME) Data* __builtin__##NAME( Subscript* arg )\
{\
	if( arg->dimensions != 2 )\
		return Raise(ILLEGAL_QUANTITY); \
	if( arg->dimension[0]->type != TYPE_STRING ||\
	    arg->dimension[1]->type != TYPE_REAL )\
	   	return Raise(TYPE_MISMATCH);\
	return Data_new( TYPE_STRING , (Storage)NAME ( arg->dimension[0]->storage.String , arg->dimension[1]->storage.Real )  , 1 );\
}
	
#define s_FUNC_srr(NAME) Data* __builtin__##NAME( Subscript* arg )\
{\
	if( arg->dimensions != 3 )\
		return Raise(ILLEGAL_QUANTITY); \
	if( arg->dimension[0]->type != TYPE_STRING ||\
	    arg->dimension[1]->type != TYPE_REAL ||\
	    arg->dimension[2]->type != TYPE_REAL )\
	   	return Raise(TYPE_MISMATCH);\
	return Data_new( TYPE_STRING , (Storage)NAME ( arg->dimension[0]->storage.String , arg->dimension[1]->storage.Real , arg->dimension[2]->storage.Real ) , 1 );\
}
	
double sign( double v )
{
	if( v < 0 ) return -1;
	if( v > 0 ) return 1;
	return 0;
}
double _random( double seed )
{
	return rand()/(double)0x7fffffffu;
}

double asc( char* string )
{
	return (double)*string;
}
char* chr( double code )
{
	char* p = (char*)malloc(2);
	p[0] = ((int)(code+.01))&0xff;
	p[1] = 0;
	return p;
}
char* left( char* string , double pos )
{
	int P = ((int)(pos+0.01));
	char* p = (char*)malloc(P+1);
	int i;
	for( i = 0 ; i < P ; i ++)
		p[i] = string[i];
	p[P] = 0;
	return p;
}
char* right( char* string , double pos )
{
	int P = ((int)(pos+0.01));
	int L = strlen(string);
	char* p = (char*)malloc(P+1);
	int i;
	for( i = L-P ; i < L ; i ++)
		p[i - L + P] = string[i];
	p[P] = 0;
	return p;
}

char* mid( char* string , double _start , double _len )
{
	int start = ((int)(_start+.01));
	int len   = ((int)(_len+.01));
	char* p = (char*)malloc(len+1);
	int i;
	for( i = 0 ; i < len ; i ++ )
		p[i] = string[ i + start - 1 ];
	p[len] = 0;
	return p;
}
char* str( double val )
{
	char* buf= (char*) malloc(1024);
	sprintf(buf,"%lg",val);
	return buf;
}
double val( char* str )
{
	double ret;
	sscanf( str, "%lf" , &ret );
	return ret;
}
double pos( double dumb )
{
	double ret = *memoryP(CUR_X);
	memoryV();
	return ret;	
}
char* spc(double n)
{
	int N = ((int)(n+0.01));
	char* buf = (char*)malloc( N + 1 );
	memset( buf , ' ' , N );
	buf[N] = 0;
	return buf;
} 
double peek(double addr)
{
	int A = ((int)(addr+0.01));
	int ret = *memoryP(A);
	memoryV();
	return ret;	
}
double eof(double id)
{
	int A = ((int)(id+.01));
	if( file_list[A].used )
		return feof( file_list[A].fp );
	else 
		return 1;
}
double lof(double id)
{
	int A = ((int)(id+.01));
	if( file_list[A].used )
	{
		FILE* fp = file_list[A].fp;
		size_t cur = ftell( fp );
		fseek( fp , 0 , SEEK_END );
		double ret = ftell( file_list[A].fp );
		fseek( fp , cur , SEEK_SET );
		return ret;
	}
	return 0;
}
/*math functions*/
r_FUNC_r(sin)
r_FUNC_r(cos)
r_FUNC_r(tan)
r_FUNC_r(asin)
r_FUNC_r(acos)
r_FUNC_r(atan)
r_FUNC_r(fabs)
r_FUNC_r(sqrt)
r_FUNC_r(floor)
r_FUNC_r(log)
r_FUNC_r(exp)
r_FUNC_r(sign)
r_FUNC_r(_random)

r_FUNC_s(asc)
s_FUNC_r(chr)
r_FUNC_s(strlen)
s_FUNC_sr(left)
s_FUNC_sr(right)
s_FUNC_srr(mid)
s_FUNC_r(str)
r_FUNC_s(val)
r_FUNC_r(pos)
s_FUNC_r(spc)
r_FUNC_r(peek)
r_FUNC_r(eof)
r_FUNC_r(lof)
/*TODO:CVI MKI CVS MKS*/

void builtin_init()
{
	srand( (unsigned)time(NULL));
	reg_foo( "SIN" , __builtin__sin );
	reg_foo( "COS" , __builtin__cos );
	reg_foo( "TAN" , __builtin__tan );
	reg_foo( "ASN" , __builtin__asin );
	reg_foo( "ACS" , __builtin__acos );
	reg_foo( "ATN" , __builtin__atan );
	reg_foo( "ABS" , __builtin__fabs );
	reg_foo( "SQR" , __builtin__sqrt );
	reg_foo( "INT" , __builtin__floor );
	reg_foo( "LOG" , __builtin__log );
	reg_foo( "EXP" , __builtin__exp );
	reg_foo( "SGN" , __builtin__sign );
	reg_foo( "RND" , __builtin___random);
	
	reg_foo( "ASC" , __builtin__asc);
	reg_foo( "CHR$", __builtin__chr);
	reg_foo( "LEN" , __builtin__strlen);
	reg_foo( "LEFT$",__builtin__left);
	reg_foo( "RIGHT$",__builtin__right);
	reg_foo( "MID$",  __builtin__mid);
	reg_foo( "STR$" , __builtin__str);
	reg_foo( "POS" , __builtin__pos );
	reg_foo( "SPC" , __builtin__spc );
	reg_foo( "VAL" , __builtin__val );
	reg_foo( "PEEK", __builtin__peek);
	reg_foo( "EOF" , __builtin__eof );
	reg_foo( "LOF" , __builtin__lof );
}
